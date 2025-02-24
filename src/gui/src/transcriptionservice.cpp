#include "transcriptionservice.h"
#include "audiohandler.h"
#include "config.h"
#include <QFile>
#include <QFileInfo>
#include <QHttpMultiPart>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QUrl>

// Define available models
const QStringList TranscriptionService::AVAILABLE_MODELS = {
    "whisper-large-v3-turbo",
    "whisper-large-v3",
    "whisper-medium",
    "whisper-small",
    "whisper-base"
};

TranscriptionService::TranscriptionService(QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
{
}

QStringList TranscriptionService::availableModels()
{
    return AVAILABLE_MODELS;
}

QString TranscriptionService::currentModel() const
{
    return Config::instance().getModel();
}

void TranscriptionService::setModel(const QString& model)
{
    if (AVAILABLE_MODELS.contains(model)) {
        Config::instance().setModel(model);
    }
}

void TranscriptionService::transcribeAudioFile(const QString& filePath)
{
    QString apiKey = Config::instance().getApiKey();
    qDebug() << "Starting transcription. API key exists:" << !apiKey.isEmpty();

    if (apiKey.isEmpty()) {
        emit transcriptionError("API key not set. Please set your Groq API key in Settings.");
        return;
    }

    // Basic validation of API key format
    if (!apiKey.startsWith("gsk_") || apiKey.length() < 20) {
        emit transcriptionError("Invalid API key format. Please check your API key in Settings.");
        return;
    }

    // Store the file path for reference later
    m_currentFilePath = filePath;

    QFile* file = new QFile(filePath);
    if (!file->open(QIODevice::ReadOnly)) {
        emit transcriptionError("Could not open audio file");
        delete file;
        return;
    }

    // Create multipart request
    QHttpMultiPart* multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    // Add file part
    QHttpPart filePart;
    filePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("audio/wav"));
    filePart.setHeader(QNetworkRequest::ContentDispositionHeader,
                      QVariant("form-data; name=\"file\"; filename=\"" +
                      QFileInfo(filePath).fileName() + "\""));
    filePart.setBodyDevice(file);
    file->setParent(multiPart); // Delete file with multiPart
    multiPart->append(filePart);

    // Add model part
    QHttpPart modelPart;
    modelPart.setHeader(QNetworkRequest::ContentDispositionHeader,
                       QVariant("form-data; name=\"model\""));
    modelPart.setBody(currentModel().toUtf8());
    multiPart->append(modelPart);

    // Add response_format part
    QHttpPart formatPart;
    formatPart.setHeader(QNetworkRequest::ContentDispositionHeader,
                        QVariant("form-data; name=\"response_format\""));
    formatPart.setBody("verbose_json");
    multiPart->append(formatPart);

    // Create request
    QUrl url(API_URL);
    QNetworkRequest request(url);
    request.setRawHeader("Authorization", "Bearer " + apiKey.toUtf8());

    qDebug() << "Sending transcription request to:" << url.toString();
    qDebug() << "Using model:" << currentModel();

    // Send request
    QNetworkReply* reply = m_networkManager->post(request, multiPart);
    multiPart->setParent(reply); // Delete multiPart with reply

    // Connect signals for progress reporting
    connect(reply, &QNetworkReply::uploadProgress,
            this, &TranscriptionService::handleUploadProgress);

    connect(reply, &QNetworkReply::finished,
            this, [this, reply]() { handleTranscriptionResponse(reply); });

    emit processingStarted();
}

void TranscriptionService::handleUploadProgress(qint64 bytesSent, qint64 bytesTotal)
{
    emit uploadProgress(bytesSent, bytesTotal);
}

void TranscriptionService::handleTranscriptionResponse(QNetworkReply* reply) {
    reply->deleteLater();

    // Log response details
    qDebug() << "\n=== Transcription API Response ===";
    qDebug() << "Status Code:" << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    qDebug() << "Content Type:" << reply->header(QNetworkRequest::ContentTypeHeader).toString();

    // Log response headers
    qDebug() << "\nResponse Headers:";
    const QList<QByteArray>& headerList = reply->rawHeaderList();
    for(const QByteArray& header : headerList) {
        qDebug() << header << ":" << reply->rawHeader(header);
    }

    if (reply->error() != QNetworkReply::NoError) {
        QString errorString = reply->errorString();
        qDebug() << "\nNetwork Error:" << errorString;
        qDebug() << "Error Code:" << reply->error();

        // Try to read error response body
        QByteArray errorData = reply->readAll();
        qDebug() << "Error Response Body:" << errorData;

        emit transcriptionError(errorString);
        emit processingFinished();
        return;
    }

    QByteArray data = reply->readAll();
    qDebug() << "\nResponse Body:" << data;

    QJsonDocument doc = QJsonDocument::fromJson(data);
    qDebug() << "\nParsed JSON:" << doc.toJson(QJsonDocument::Indented);

    if (!doc.isObject()) {
        qDebug() << "Error: Response is not a valid JSON object";
        emit transcriptionError("Invalid response format");
        emit processingFinished();
        return;
    }

    QJsonObject obj = doc.object();
    if (!obj.contains("text")) {
        qDebug() << "Error: No 'text' field in response";
        emit transcriptionError("No transcription in response");
        emit processingFinished();
        return;
    }

    // Parse the detailed result
    TranscriptionResult result;
    result.text = obj["text"].toString();
    result.language = obj["language"].toString();

    // Use API-provided duration or estimate from audio file
    if (obj.contains("duration")) {
        result.duration = obj["duration"].toDouble();
    } else {
        // Get duration from AudioHandler if possible
        AudioHandler* audioHandler = qobject_cast<AudioHandler*>(parent());
        if (audioHandler) {
            result.duration = audioHandler->getLastRecordingDuration();
        } else {
            result.duration = 0.0;
        }
    }

    result.task = obj["task"].toString();

    // Get request ID from x_groq object
    if (obj.contains("x_groq")) {
        QJsonObject groqObj = obj["x_groq"].toObject();
        result.requestId = groqObj["id"].toString();
    }

    // Parse first segment details if available
    if (obj.contains("segments") && obj["segments"].isArray()) {
        QJsonArray segments = obj["segments"].toArray();
        if (!segments.isEmpty()) {
            QJsonObject segment = segments.first().toObject();
            result.segment.start = segment["start"].toDouble();
            result.segment.end = segment["end"].toDouble();
            result.segment.avgLogProb = segment["avg_logprob"].toDouble();
            result.segment.noSpeechProb = segment["no_speech_prob"].toDouble();
            result.segment.temperature = segment["temperature"].toDouble();
            result.segment.compressionRatio = segment["compression_ratio"].toDouble();
        }
    }

    // Emit both the simple text and detailed result
    emit transcriptionComplete(result.text);
    emit transcriptionComplete(result);

    qDebug() << "=== End of Response ===\n";
    emit processingFinished();
}