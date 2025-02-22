#ifndef SYSTEMTRAYHANDLER_H
#define SYSTEMTRAYHANDLER_H

#include <QObject>
#include <QSystemTrayIcon>
#include <QMenu>
#include "transcriptionservice.h"

class ShortcutManager;
class AudioHandler;

class SystemTrayHandler : public QObject
{
    Q_OBJECT

public:
    explicit SystemTrayHandler(QObject *parent = nullptr);
    ~SystemTrayHandler();

    QSystemTrayIcon* trayIcon() const { return m_trayIcon; }

public slots:
    void showTranscriptionComplete(const QString& text);
    void showTranscriptionComplete(const TranscriptionResult& result);
    void showSettings();

private slots:
    void trayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void quit();
    void startRecording();
    void stopRecording();

signals:
    void recordingStarted();
    void recordingStopped();

private:
    void createActions();
    void createTrayIcon();

    QSystemTrayIcon *m_trayIcon;
    QMenu *trayIconMenu;
    QAction *quitAction;
    QAction *startRecordingAction;
    QAction *stopRecordingAction;
    QAction *autoTranscribeAction;
    ShortcutManager* m_shortcutManager;
    AudioHandler* m_audioHandler;
};

#endif // SYSTEMTRAYHANDLER_H 
