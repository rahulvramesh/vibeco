#include "systemtrayhandler.h"
#include "QmlDictationManager.h"
#include "ShortcutManager.h"
#include "audiohandler.h"
#include "settingsdialog.h"
#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QMessageBox>
#include <QQmlProperty>
#include <QTimer>

SystemTrayHandler::SystemTrayHandler(QQmlApplicationEngine* engine, QObject* parent)
    : QObject(parent), m_trayIcon(new QSystemTrayIcon(this)), trayIconMenu(new QMenu()),
      quitAction(new QAction(tr("&Quit"), this)),
      startRecordingAction(new QAction(tr("&Start Recording"), this)),
      stopRecordingAction(new QAction(tr("&Stop Recording"), this)),
      autoTranscribeAction(new QAction(tr("&Auto Transcribe"), this)), m_shortcutManager(nullptr),
      m_audioHandler(nullptr), m_dictationManager(nullptr), m_qmlEngine(engine),
      m_mainWindow(nullptr) {

    createActions();
    createTrayIcon();
    setupQmlDictationManager();

    m_trayIcon->show();
    m_shortcutManager = new ShortcutManager(this, this);
    m_audioHandler = new AudioHandler(this);
    m_audioHandler->initialize();

    connect(m_audioHandler, &AudioHandler::transcriptionReceived, this,
            &SystemTrayHandler::handleTranscriptionReceived);

    connect(this, &SystemTrayHandler::recordingStarted, this, [this]() {
        startRecordingAction->setEnabled(false);
        stopRecordingAction->setEnabled(true);

        if (m_dictationManager) {
            m_dictationManager->setRecordingState(true);
        }
    });

    connect(this, &SystemTrayHandler::recordingStopped, this, [this]() {
        startRecordingAction->setEnabled(true);
        stopRecordingAction->setEnabled(false);

        if (m_dictationManager) {
            m_dictationManager->setRecordingState(false);
        }
    });

    // Add this connection to handle application activation for keeping dictation window on top
    // Use string-based connection to avoid type issues between QApplication and QGuiApplication
    connect(qApp, SIGNAL(applicationStateChanged(Qt::ApplicationState)), this,
            SLOT(handleApplicationStateChanged(Qt::ApplicationState)));

    // Show the dictation widget initially
    // showDictationWidget();
}

SystemTrayHandler::~SystemTrayHandler() {
    if (m_audioHandler && m_audioHandler->isRecording()) {
        m_audioHandler->stopRecording();
    }
    delete m_trayIcon;
    delete trayIconMenu;
    delete m_audioHandler;
    // m_dictationManager is deleted by QObject parent-child relationship
}

void SystemTrayHandler::setMainWindow(QObject* mainWindow) {
    m_mainWindow = mainWindow;
}

void SystemTrayHandler::setupQmlDictationManager() {
    if (m_qmlEngine) {
        m_dictationManager = new QmlDictationManager(m_qmlEngine, this);
        connect(m_dictationManager, &QmlDictationManager::dictationWidgetClicked, this,
                &SystemTrayHandler::onDictationWidgetClicked);
        qDebug() << "QML Dictation Manager created";
    } else {
        qWarning() << "Cannot create QML Dictation Manager: QML engine is null";
    }
}

void SystemTrayHandler::setQmlEngine(QQmlApplicationEngine* engine) {
    if (!m_qmlEngine && engine) {
        m_qmlEngine = engine;
        setupQmlDictationManager();
    }
}

void SystemTrayHandler::createActions() {
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

void SystemTrayHandler::createTrayIcon() {
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
    connect(m_trayIcon, &QSystemTrayIcon::activated, this, &SystemTrayHandler::trayIconActivated);
}

void SystemTrayHandler::trayIconActivated(QSystemTrayIcon::ActivationReason reason) {
    switch (reason) {
    case QSystemTrayIcon::Trigger:
        // Handle single click - toggle main window visibility
        if (m_mainWindow) {
            bool isVisible = QQmlProperty::read(m_mainWindow, "visible").toBool();
            QQmlProperty::write(m_mainWindow, "visible", !isVisible);
        }
        break;
    case QSystemTrayIcon::DoubleClick:
        // Handle double click - start/stop recording
        if (m_audioHandler) {
            if (m_audioHandler->isRecording()) {
                stopRecording();
            } else {
                startRecording();
            }
        }
        break;
    default:
        break;
    }
}

void SystemTrayHandler::handleTranscriptionReceived(const QString& text) {
    // Show notification
    m_trayIcon->showMessage(tr("Transcription Complete"), text);

    // Add to main window history if available
    if (m_mainWindow) {
        // Create a QVariantMap for the transcription data
        QVariantMap transcription;
        transcription["text"] = text;
        transcription["timestamp"] = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        transcription["duration"] = 0.0;  // This would need to be tracked in AudioHandler
        transcription["language"] = "en"; // This would come from the actual transcription

        // Use QMetaObject::invokeMethod to call the QML method
        QMetaObject::invokeMethod(m_mainWindow, "onTranscriptionReceived",
                                  Q_ARG(QVariant, QVariant::fromValue(transcription)));
    }

    // Also emit the signal for any connected slots
    emit transcriptionReceived(text, 0.0, "en");
}

void SystemTrayHandler::showTranscriptionComplete(const QString& text) {
    m_trayIcon->showMessage(tr("Transcription Complete"), text);

    // Also add to main window history
    handleTranscriptionReceived(text);
}

void SystemTrayHandler::showTranscriptionComplete(const TranscriptionResult& result) {
    // Add to main window history if available
    if (m_mainWindow) {
        // Create a QVariantMap for the transcription data
        QVariantMap transcription;
        transcription["text"] = result.text;
        transcription["timestamp"] = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        transcription["duration"] = result.duration;
        transcription["language"] = result.language;

        // Use QMetaObject::invokeMethod to call the QML method
        QMetaObject::invokeMethod(m_mainWindow, "onTranscriptionReceived",
                                  Q_ARG(QVariant, QVariant::fromValue(transcription)));
    }

    // Also emit the signal for any connected slots
    emit transcriptionReceived(result.text, result.duration, result.language);

    // Create a detailed message
    QString details = tr("Transcription Details:\n\n");
    details += tr("Text: %1\n\n").arg(result.text);
    details += tr("Language: %1\n").arg(result.language);
    details += tr("Duration: %1 seconds\n").arg(result.duration);
    details += tr("Task: %1\n").arg(result.task);
    details += tr("Request ID: %1\n\n").arg(result.requestId);

    details += tr("Segment Details:\n");
    details += tr("- Time: %1s to %2s\n").arg(result.segment.start).arg(result.segment.end);
    details += tr("- Avg Log Probability: %1\n").arg(result.segment.avgLogProb);
    details += tr("- No Speech Probability: %1\n").arg(result.segment.noSpeechProb);
    details += tr("- Temperature: %1\n").arg(result.segment.temperature);
    details += tr("- Compression Ratio: %1").arg(result.segment.compressionRatio);

    // Show brief notification in system tray
    m_trayIcon->showMessage(tr("Transcription Complete"), result.text);

    // Show detailed results in a message box
    QMessageBox::information(nullptr, tr("Transcription Details"), details);
}

void SystemTrayHandler::startRecording() {
    if (m_audioHandler && m_audioHandler->startRecording()) {
        startRecordingAction->setEnabled(false);
        stopRecordingAction->setEnabled(true);

        showDictationWidget();

        // Remove this distraction later
        m_trayIcon->showMessage(tr("Recording"), tr("Audio recording started"));
        emit recordingStarted();
    } else {
        m_trayIcon->showMessage(tr("Error"), tr("Failed to start recording"),
                                QSystemTrayIcon::Critical);
    }
}

void SystemTrayHandler::stopRecording() {
    if (m_audioHandler && m_audioHandler->stopRecording()) {
        startRecordingAction->setEnabled(true);
        stopRecordingAction->setEnabled(false);

        hideDictationWidget();
        QString message =
            tr("Audio recording stopped\nSaved to: %1").arg(m_audioHandler->getLastRecordingPath());
        m_trayIcon->showMessage(tr("Recording"), message);
        emit recordingStopped();
    } else {
        m_trayIcon->showMessage(tr("Error"), tr("Failed to stop recording"),
                                QSystemTrayIcon::Critical);
    }
}

void SystemTrayHandler::quit() {
    QApplication::quit();
}

void SystemTrayHandler::showSettings() {
    SettingsDialog dialog;
    dialog.exec();
}

void SystemTrayHandler::showDictationWidget() {
    if (m_dictationManager) {
        m_dictationManager->showDictationWidget();
    }
}

void SystemTrayHandler::hideDictationWidget() {
    if (m_dictationManager) {
        m_dictationManager->hideDictationWidget();
    }
}

void SystemTrayHandler::onDictationWidgetClicked() {
    qDebug() << "Dictation widget clicked";
    if (m_audioHandler) {
        if (m_audioHandler->isRecording()) {
            stopRecording();
        } else {
            startRecording();
        }
    }
}

void SystemTrayHandler::handleApplicationStateChanged(Qt::ApplicationState state) {
    if (state == Qt::ApplicationActive) {
        // If we're recording and the dictation manager exists, ensure the window is on top
        if (m_audioHandler && m_audioHandler->isRecording() && m_dictationManager) {
            // Use a short delay to let the OS finish its window management first
            QTimer::singleShot(100, this, [this]() { m_dictationManager->showDictationWidget(); });
        }
    }
}
