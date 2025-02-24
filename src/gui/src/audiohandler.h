#ifndef AUDIOHANDLER_H
#define AUDIOHANDLER_H

#include <portaudio.h>
#include <QObject>
#include <QFile>
#include <QDateTime>
#include <QElapsedTimer>
#include "transcriptionservice.h"

class AudioHandler : public QObject
{
    Q_OBJECT

public:
    explicit AudioHandler(QObject *parent = nullptr);
    ~AudioHandler();

    bool initialize();
    bool startRecording();
    bool stopRecording();
    bool isRecording() const { return m_isRecording; }
    QString getLastRecordingPath() const { return m_currentFilePath; }
    void setAutoTranscribe(bool enabled) { m_autoTranscribe = enabled; }
    bool autoTranscribe() const { return m_autoTranscribe; }
    double getLastRecordingDuration() const { return m_lastRecordingDuration; }

    signals:
        void recordingStarted();
    void recordingStopped();
    void audioDataReady(const QByteArray& data);
    void transcriptionReceived(const QString& text);

private:
    static int recordCallback(const void *inputBuffer, void *outputBuffer,
                            unsigned long framesPerBuffer,
                            const PaStreamCallbackTimeInfo* timeInfo,
                            PaStreamCallbackFlags statusFlags,
                            void *userData);

    void processAudioData(const float* inputBuffer, unsigned long framesPerBuffer);
    bool writeWavHeader();
    void updateWavHeader();

    PaStream *m_stream;
    bool m_isRecording;
    bool m_isInitialized;

    QFile m_outputFile;
    QString m_currentFilePath;
    qint64 m_dataSize;
    const int m_sampleRate = 44100;
    const int m_numChannels = 1;
    const int m_bitsPerSample = 32;
    TranscriptionService* m_transcriptionService;
    bool m_autoTranscribe;

    // Recording duration tracking
    QElapsedTimer m_recordingTimer;
    double m_lastRecordingDuration;
};

#endif // AUDIOHANDLER_H