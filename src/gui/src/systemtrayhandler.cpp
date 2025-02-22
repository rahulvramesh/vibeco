#include "SystemTrayHandler.h"
#include <QApplication>
#include <QDebug>
#include <QFile>
#include "ShortcutManager.h"
#include "audiohandler.h"

SystemTrayHandler::SystemTrayHandler(QObject* parent)
    : QObject(parent),
      m_trayIcon(new QSystemTrayIcon(this)),
      trayIconMenu(new QMenu()),
      quitAction(new QAction(tr("&Quit"), this)),
      startRecordingAction(new QAction(tr("&Start Recording"), this)),
      stopRecordingAction(new QAction(tr("&Stop Recording"), this)),
      autoTranscribeAction(new QAction(tr("&Auto Transcribe"), this)),
      m_shortcutManager(nullptr),
      m_audioHandler(nullptr)
{
    createActions();
    createTrayIcon();

    m_trayIcon->show();

    m_shortcutManager = new ShortcutManager(this, this);

    m_audioHandler = new AudioHandler(this);
    m_audioHandler->initialize();
    
    connect(m_audioHandler, &AudioHandler::transcriptionReceived,
            this, &SystemTrayHandler::handleTranscription);
}

SystemTrayHandler::~SystemTrayHandler()
{
    if (m_audioHandler && m_audioHandler->isRecording()) {
        m_audioHandler->stopRecording();
    }
    delete m_trayIcon;
    delete trayIconMenu;
    delete m_audioHandler;
}

void SystemTrayHandler::createActions()
{
    // Implement action creation and connections here
}

void SystemTrayHandler::createTrayIcon()
{
    // Create the context menu
    trayIconMenu->addAction(startRecordingAction);
    trayIconMenu->addAction(stopRecordingAction);
    trayIconMenu->addAction(autoTranscribeAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    // Set up the tray icon
    m_trayIcon->setContextMenu(trayIconMenu);
    m_trayIcon->setToolTip(tr("Vibeco"));

    // Load the icon from resources
    QIcon icon(":/icons/app-icon.png");
    if (icon.isNull()) {
        qWarning() << "Failed to load icon from resource, using fallback icon";
        icon = QIcon::fromTheme("system-run"); // Fallback icon
    }
    m_trayIcon->setIcon(icon);

    // Connect activation signal
    connect(m_trayIcon, &QSystemTrayIcon::activated, 
            this, &SystemTrayHandler::trayIconActivated);
}

void SystemTrayHandler::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
        case QSystemTrayIcon::Trigger:
            // Handle single click
            break;
        case QSystemTrayIcon::DoubleClick:
            // Handle double click
            break;
        default:
            break;
    }
}

void SystemTrayHandler::startRecording()
{
    if (m_audioHandler && m_audioHandler->startRecording()) {
        startRecordingAction->setEnabled(false);
        stopRecordingAction->setEnabled(true);
        m_trayIcon->showMessage(tr("Recording"), tr("Audio recording started"));
        emit recordingStarted();
    } else {
        m_trayIcon->showMessage(tr("Error"), tr("Failed to start recording"),
                                  QSystemTrayIcon::Critical);
    }
}

void SystemTrayHandler::stopRecording()
{
    if (m_audioHandler && m_audioHandler->stopRecording()) {
        startRecordingAction->setEnabled(true);
        stopRecordingAction->setEnabled(false);
        QString message = tr("Audio recording stopped\nSaved to: %1")
                         .arg(m_audioHandler->getLastRecordingPath());
        m_trayIcon->showMessage(tr("Recording"), message);
        emit recordingStopped();
    } else {
        m_trayIcon->showMessage(tr("Error"), tr("Failed to stop recording"),
                                  QSystemTrayIcon::Critical);
    }
}

void SystemTrayHandler::quit()
{
    QApplication::quit();
}

void SystemTrayHandler::handleTranscription(const QString& text)
{
    m_trayIcon->showMessage(tr("Transcription Complete"), text);
} 
