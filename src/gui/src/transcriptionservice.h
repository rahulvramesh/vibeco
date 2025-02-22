#ifndef TRANSCRIPTIONSERVICE_H
#define TRANSCRIPTIONSERVICE_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>

struct TranscriptionResult {
    QString text;           // The transcribed text
    QString language;       // Detected language
    double duration;        // Audio duration in seconds
    QString task;          // Task type (e.g., "transcribe")
    QString requestId;      // Request ID from Groq
    
    // Segment details (for the first segment)
    struct Segment {
        double start;
        double end;
        double avgLogProb;
        double noSpeechProb;
        double temperature;
        double compressionRatio;
    } segment;
};

class TranscriptionService : public QObject
{
    Q_OBJECT

public:
    explicit TranscriptionService(QObject *parent = nullptr);
    void transcribeAudioFile(const QString& filePath);
    
    // Available Whisper models
    static QStringList availableModels();
    QString currentModel() const;
    void setModel(const QString& model);

signals:
    void transcriptionComplete(const QString& text);
    void transcriptionComplete(const TranscriptionResult& result);
    void transcriptionError(const QString& error);
    void uploadProgress(qint64 bytesSent, qint64 bytesTotal);
    void processingStarted();
    void processingFinished();

private slots:
    void handleTranscriptionResponse(QNetworkReply* reply);
    void handleUploadProgress(qint64 bytesSent, qint64 bytesTotal);

private:
    QNetworkAccessManager* m_networkManager;
    const QString API_URL = "https://api.groq.com/openai/v1/audio/transcriptions";
    static const QStringList AVAILABLE_MODELS;
};

#endif // TRANSCRIPTIONSERVICE_H 