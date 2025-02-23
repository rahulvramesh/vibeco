#include "systemtrayhandler.h"
#include <QApplication>
#include <QDebug>
#include <QFile>
#include "ShortcutManager.h"
#include "audiohandler.h"
#include "settingsdialog.h"
#include <QMessageBox>
#include "dictationwidget.h"

SystemTrayHandler::SystemTrayHandler(QObject* parent)
    : QObject(parent),
      m_trayIcon(new QSystemTrayIcon(this)),
      trayIconMenu(new QMenu()),
      quitAction(new QAction(tr("&Quit"), this)),
      startRecordingAction(new QAction(tr("&Start Recording"), this)),
      stopRecordingAction(new QAction(tr("&Stop Recording"), this)),
      autoTranscribeAction(new QAction(tr("&Auto Transcribe"), this)),
      m_shortcutManager(nullptr),
      m_audioHandler(nullptr),
      m_dictationWidget(new DictationWidget())
{
    createActions();
    createTrayIcon();

    m_trayIcon->show();

    m_shortcutManager = new ShortcutManager(this, this);

    m_audioHandler = new AudioHandler(this);
    m_audioHandler->initialize();

    // Connect dictation widget signals
    connect(m_dictationWidget, &DictationWidget::clicked, this, &SystemTrayHandler::startRecording);

    connect(m_audioHandler, &AudioHandler::transcriptionReceived,
            this, static_cast<void (SystemTrayHandler::*)(const QString&)>(&SystemTrayHandler::showTranscriptionComplete));

    connect(this, &SystemTrayHandler::recordingStarted, this, [this](){
        startRecordingAction->setEnabled(false);
        stopRecordingAction->setEnabled(true);
        //hideDictationWidget();
    });

    connect(this, &SystemTrayHandler::recordingStopped, this, [this](){
        startRecordingAction->setEnabled(true);
        stopRecordingAction->setEnabled(false);
        //showDictationWidget();
    });

    // Show the dictation widget initially
    showDictationWidget();
}

SystemTrayHandler::~SystemTrayHandler()
{
    if (m_audioHandler && m_audioHandler->isRecording()) {
        m_audioHandler->stopRecording();
    }
    delete m_trayIcon;
    delete trayIconMenu;
    delete m_audioHandler;
    delete m_dictationWidget;
}

void SystemTrayHandler::createActions()
{
    // Connect all actions
    connect(quitAction, &QAction::triggered, this, &SystemTrayHandler::quit);
    connect(startRecordingAction, &QAction::triggered, this, &SystemTrayHandler::startRecording);
    connect(stopRecordingAction, &QAction::triggered, this, &SystemTrayHandler::stopRecording);
    connect(autoTranscribeAction, &QAction::triggered, this, [this]() {
        if (m_audioHandler) {
            m_audioHandler->setAutoTranscribe(!m_audioHandler->autoTranscribe());
            autoTranscribeAction->setChecked(m_audioHandler->autoTranscribe());
        }
    });

    // Set initial state
    stopRecordingAction->setEnabled(false);
    autoTranscribeAction->setCheckable(true);
    if (m_audioHandler) {
        autoTranscribeAction->setChecked(m_audioHandler->autoTranscribe());
    }
}

void SystemTrayHandler::createTrayIcon()
{
    // Create the context menu
    trayIconMenu->addAction(startRecordingAction);
    trayIconMenu->addAction(stopRecordingAction);
    trayIconMenu->addAction(autoTranscribeAction);
    trayIconMenu->addSeparator();
    
    // Add settings action
    QAction* settingsAction = new QAction(tr("Settings"), this);
    connect(settingsAction, &QAction::triggered, this, &SystemTrayHandler::showSettings);
    trayIconMenu->addAction(settingsAction);
    
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

void SystemTrayHandler::showTranscriptionComplete(const QString& text)
{
    m_trayIcon->showMessage(tr("Transcription Complete"), text);
}

void SystemTrayHandler::showTranscriptionComplete(const TranscriptionResult& result)
{
    // Create a detailed message
    QString details = tr("Transcription Details:\n\n");
    details += tr("Text: %1\n\n").arg(result.text);
    details += tr("Language: %1\n").arg(result.language);
    details += tr("Duration: %1 seconds\n").arg(result.duration);
    details += tr("Task: %1\n").arg(result.task);
    details += tr("Request ID: %1\n\n").arg(result.requestId);
    
    details += tr("Segment Details:\n");
    details += tr("- Time: %1s to %2s\n")
        .arg(result.segment.start)
        .arg(result.segment.end);
    details += tr("- Avg Log Probability: %1\n")
        .arg(result.segment.avgLogProb);
    details += tr("- No Speech Probability: %1\n")
        .arg(result.segment.noSpeechProb);
    details += tr("- Temperature: %1\n")
        .arg(result.segment.temperature);
    details += tr("- Compression Ratio: %1")
        .arg(result.segment.compressionRatio);

    // Show brief notification in system tray
    m_trayIcon->showMessage(tr("Transcription Complete"), result.text);

    // Show detailed results in a message box
    QMessageBox::information(nullptr, tr("Transcription Details"), details);
}

void SystemTrayHandler::showSettings()
{
    SettingsDialog dialog;
    dialog.exec();
}

void SystemTrayHandler::showDictationWidget()
{
    if (m_dictationWidget) {
        m_dictationWidget->showAtBottom();
    }
}

void SystemTrayHandler::hideDictationWidget()
{
    if (m_dictationWidget) {
        m_dictationWidget->hide();
    }
} 
