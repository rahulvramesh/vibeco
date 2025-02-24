#ifndef SYSTEMTRAYHANDLER_H
#define SYSTEMTRAYHANDLER_H

#include <QObject>
#include <QSystemTrayIcon>
#include <QMenu>
#include "QmlDictationManager.h"

class ShortcutManager;
class AudioHandler;
struct TranscriptionResult;

class SystemTrayHandler : public QObject
{
    Q_OBJECT

public:
    explicit SystemTrayHandler(QQmlApplicationEngine* engine, QObject* parent = nullptr);
    ~SystemTrayHandler();

    QSystemTrayIcon* trayIcon() const { return m_trayIcon; }
    void setQmlEngine(QQmlApplicationEngine* engine);

    public slots:
        void showTranscriptionComplete(const QString& text);
    void showTranscriptionComplete(const TranscriptionResult& result);
    void showSettings();
    void startRecording();
    void stopRecording();

    private slots:
        void trayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void quit();
    void onDictationWidgetClicked();

    signals:
        void recordingStarted();
    void recordingStopped();

private:
    void createActions();
    void createTrayIcon();
    void showDictationWidget();
    void hideDictationWidget();
    void setupQmlDictationManager();

    QSystemTrayIcon *m_trayIcon;
    QMenu *trayIconMenu;
    QAction *quitAction;
    QAction *startRecordingAction;
    QAction *stopRecordingAction;
    QAction *autoTranscribeAction;
    ShortcutManager* m_shortcutManager;
    AudioHandler* m_audioHandler;
    QmlDictationManager* m_dictationManager;
    QQmlApplicationEngine* m_qmlEngine;
};

#endif // SYSTEMTRAYHANDLER_H