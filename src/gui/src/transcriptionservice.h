#ifndef TRANSCRIPTIONSERVICE_H
#define TRANSCRIPTIONSERVICE_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class TranscriptionService : public QObject
{
    Q_OBJECT

public:
    explicit TranscriptionService(QObject *parent = nullptr);
    void transcribeAudioFile(const QString& filePath);

signals:
    void transcriptionComplete(const QString& text);
    void transcriptionError(const QString& error);

private slots:
    void handleTranscriptionResponse(QNetworkReply* reply);

private:
    QNetworkAccessManager* m_networkManager;
    QString m_apiKey;
    const QString API_URL = "https://api.groq.com/openai/v1/audio/transcriptions";
    const QString MODEL = "whisper-large-v3-turbo";

    void loadApiKey();
};

#endif // TRANSCRIPTIONSERVICE_H 