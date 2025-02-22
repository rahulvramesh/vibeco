#ifndef SYSTEMTRAYHANDLER_H
#define SYSTEMTRAYHANDLER_H

#include <QObject>
#include <QSystemTrayIcon>
#include <QMenu>

class ShortcutManager;
class AudioHandler;

class SystemTrayHandler : public QObject
{
    Q_OBJECT

public:
    explicit SystemTrayHandler(QObject *parent = nullptr);
    ~SystemTrayHandler();

    QSystemTrayIcon* trayIcon() const { return m_trayIcon; }

private slots:
    void trayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void quit();
    void startRecording();
    void stopRecording();
    void handleTranscription(const QString& text);

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
