#include "audiohandler.h"
#include <QDebug>
#include <QStandardPaths>
#include <QDir>
#include "transcriptionservice.h"

AudioHandler::AudioHandler(QObject *parent)
    : QObject(parent)
    , m_stream(nullptr)
    , m_isRecording(false)
    , m_isInitialized(false)
    , m_dataSize(0)
    , m_transcriptionService(new TranscriptionService(this))
    , m_autoTranscribe(false)
{
    connect(m_transcriptionService, &TranscriptionService::transcriptionComplete,
            this, &AudioHandler::transcriptionReceived);
    connect(m_transcriptionService, &TranscriptionService::transcriptionError,
            [this](const QString& error) {
                qDebug() << "Transcription error:" << error;
            });
}

AudioHandler::~AudioHandler()
{
    if (m_isInitialized) {
        Pa_Terminate();
    }
    if (m_outputFile.isOpen()) {
        updateWavHeader();
        m_outputFile.close();
    }
}

bool AudioHandler::initialize()
{
    PaError err = Pa_Initialize();
    if (err != paNoError) {
        qDebug() << "PortAudio error:" << Pa_GetErrorText(err);
        return false;
    }

    m_isInitialized = true;
    return true;
}

bool AudioHandler::startRecording()
{
    if (!m_isInitialized) {
        return false;
    }

    // Create recordings directory if it doesn't exist
    QString recordingsPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) 
                           + "/Vibeco/Recordings";
    QDir().mkpath(recordingsPath);

    // Create filename with timestamp
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss");
    m_currentFilePath = recordingsPath + "/recording_" + timestamp + ".wav";
    
    m_outputFile.setFileName(m_currentFilePath);
    if (!m_outputFile.open(QIODevice::WriteOnly)) {
        qDebug() << "Failed to open output file:" << m_currentFilePath;
        return false;
    }

    // Write WAV header
    if (!writeWavHeader()) {
        m_outputFile.close();
        return false;
    }

    m_dataSize = 0;

    PaError err = Pa_OpenDefaultStream(&m_stream,
                                     m_numChannels,  // input channels
                                     0,              // output channels
                                     paFloat32,      // sample format
                                     m_sampleRate,   // sample rate
                                     256,           // frames per buffer
                                     recordCallback,
                                     this);

    if (err != paNoError) {
        qDebug() << "PortAudio error:" << Pa_GetErrorText(err);
        m_outputFile.close();
        return false;
    }

    err = Pa_StartStream(m_stream);
    if (err != paNoError) {
        qDebug() << "PortAudio error:" << Pa_GetErrorText(err);
        m_outputFile.close();
        return false;
    }

    m_isRecording = true;
    emit recordingStarted();
    return true;
}

bool AudioHandler::stopRecording()
{
    if (!m_isRecording) {
        return true;
    }

    PaError err = Pa_StopStream(m_stream);
    if (err != paNoError) {
        qDebug() << "PortAudio error:" << Pa_GetErrorText(err);
        return false;
    }

    err = Pa_CloseStream(m_stream);
    if (err != paNoError) {
        qDebug() << "PortAudio error:" << Pa_GetErrorText(err);
        return false;
    }

    // Update WAV header with final size
    updateWavHeader();
    m_outputFile.close();

    m_isRecording = false;
    emit recordingStopped();

    if (m_autoTranscribe) {
        m_transcriptionService->transcribeAudioFile(m_currentFilePath);
    }

    return true;
}

int AudioHandler::recordCallback(const void *inputBuffer, void *outputBuffer,
                               unsigned long framesPerBuffer,
                               const PaStreamCallbackTimeInfo* timeInfo,
                               PaStreamCallbackFlags statusFlags,
                               void *userData)
{
    AudioHandler* handler = static_cast<AudioHandler*>(userData);
    const float* in = static_cast<const float*>(inputBuffer);
    
    if (handler && in) {
        handler->processAudioData(in, framesPerBuffer);
    }

    return paContinue;
}

void AudioHandler::processAudioData(const float* inputBuffer, unsigned long framesPerBuffer)
{
    if (!m_outputFile.isOpen()) return;

    // Write the audio data to file
    qint64 bytesWritten = m_outputFile.write(
        reinterpret_cast<const char*>(inputBuffer),
        framesPerBuffer * sizeof(float)
    );
    
    if (bytesWritten > 0) {
        m_dataSize += bytesWritten;
    }

    emit audioDataReady(QByteArray(reinterpret_cast<const char*>(inputBuffer),
                                 framesPerBuffer * sizeof(float)));
}

bool AudioHandler::writeWavHeader()
{
    // RIFF header
    m_outputFile.write("RIFF", 4);
    m_outputFile.write("\x00\x00\x00\x00", 4); // Size (to be updated later)
    m_outputFile.write("WAVE", 4);

    // Format chunk
    m_outputFile.write("fmt ", 4);
    qint32 subchunk1Size = 16;
    m_outputFile.write(reinterpret_cast<const char*>(&subchunk1Size), 4);
    qint16 audioFormat = 3; // IEEE float
    m_outputFile.write(reinterpret_cast<const char*>(&audioFormat), 2);
    m_outputFile.write(reinterpret_cast<const char*>(&m_numChannels), 2);
    m_outputFile.write(reinterpret_cast<const char*>(&m_sampleRate), 4);
    qint32 byteRate = m_sampleRate * m_numChannels * (m_bitsPerSample/8);
    m_outputFile.write(reinterpret_cast<const char*>(&byteRate), 4);
    qint16 blockAlign = m_numChannels * (m_bitsPerSample/8);
    m_outputFile.write(reinterpret_cast<const char*>(&blockAlign), 2);
    m_outputFile.write(reinterpret_cast<const char*>(&m_bitsPerSample), 2);

    // Data chunk header
    m_outputFile.write("data", 4);
    m_outputFile.write("\x00\x00\x00\x00", 4); // Size (to be updated later)

    return true;
}

void AudioHandler::updateWavHeader()
{
    if (!m_outputFile.isOpen()) return;

    // Update RIFF chunk size
    qint32 riffSize = m_dataSize + 36; // Total size - 8 bytes
    m_outputFile.seek(4);
    m_outputFile.write(reinterpret_cast<const char*>(&riffSize), 4);

    // Update data chunk size
    m_outputFile.seek(40);
    m_outputFile.write(reinterpret_cast<const char*>(&m_dataSize), 4);
} 