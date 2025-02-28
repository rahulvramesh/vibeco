#ifndef SYSTEMTRAYHANDLER_H
#define SYSTEMTRAYHANDLER_H

#include "QmlDictationManager.h"
#include <QDateTime>
#include <QMenu>
#include <QObject>
#include <QSystemTrayIcon>

class ShortcutManager;
class AudioHandler;
struct TranscriptionResult;

class SystemTrayHandler : public QObject {
    Q_OBJECT

  public:
    explicit SystemTrayHandler(QQmlApplicationEngine* engine, QObject* parent = nullptr);
    ~SystemTrayHandler();

    QSystemTrayIcon* trayIcon() const {
        return m_trayIcon;
    }
    void setQmlEngine(QQmlApplicationEngine* engine);
    void setMainWindow(QObject* mainWindow);

  public slots:
    void showTranscriptionComplete(const QString& text);
    void showTranscriptionComplete(const TranscriptionResult& result);
    void showSettings();
    void startRecording();
    void stopRecording();
    void handleTranscriptionReceived(const QString& text);

  private slots:
    void trayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void quit();
    void onDictationWidgetClicked();

  signals:
    void recordingStarted();
    void recordingStopped();
    void transcriptionReceived(const QString& text, double duration, const QString& language);

  private:
    void createActions();
    void createTrayIcon();
    void showDictationWidget();
    void hideDictationWidget();
    void setupQmlDictationManager();

    QSystemTrayIcon* m_trayIcon;
    QMenu* trayIconMenu;
    QAction* quitAction;
    QAction* startRecordingAction;
    QAction* stopRecordingAction;
    QAction* autoTranscribeAction;
    ShortcutManager* m_shortcutManager;
    AudioHandler* m_audioHandler;
    QmlDictationManager* m_dictationManager;
    QQmlApplicationEngine* m_qmlEngine;
    QObject* m_mainWindow; // Reference to the main QML window

    void handleApplicationStateChanged(Qt::ApplicationState state);
};

#endif // SYSTEMTRAYHANDLER_H
