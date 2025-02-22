#include "transcriptionservice.h"
#include <QFile>
#include <QFileInfo>
#include <QHttpMultiPart>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSettings>
#include <QDebug>

TranscriptionService::TranscriptionService(QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
{
    loadApiKey();
}

void TranscriptionService::loadApiKey()
{
    // Load API key from settings
    QSettings settings("Vibeco", "Vibeco");
    m_apiKey = "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";
    //settings.value("GroqApiKey").toString();
}

void TranscriptionService::transcribeAudioFile(const QString& filePath)
{
    if (m_apiKey.isEmpty()) {
        emit transcriptionError("API key not set");
        return;
    }

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
    modelPart.setBody(MODEL.toUtf8());
    multiPart->append(modelPart);

    // Fix QNetworkRequest creation
    QNetworkRequest request((QUrl(API_URL)));  // Note the extra parentheses
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", "Bearer " + m_apiKey.toUtf8());

    // Send request
    QNetworkReply* reply = m_networkManager->post(request, multiPart);
    multiPart->setParent(reply); // Delete multiPart with reply

    connect(reply, &QNetworkReply::finished,
            this, [this, reply]() { handleTranscriptionResponse(reply); });
}

void TranscriptionService::handleTranscriptionResponse(QNetworkReply* reply)
{
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError) {
        emit transcriptionError(reply->errorString());
        return;
    }

    QByteArray data = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    
    if (!doc.isObject()) {
        emit transcriptionError("Invalid response format");
        return;
    }

    QJsonObject obj = doc.object();
    if (obj.contains("text")) {
        emit transcriptionComplete(obj["text"].toString());
    } else {
        emit transcriptionError("No transcription in response");
    }
} 