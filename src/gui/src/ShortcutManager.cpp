#include "ShortcutManager.h"
#include "SystemTrayHandler.h"
#include <QDebug>

#ifdef Q_OS_MAC
extern bool checkAccessibilityPermissions();
#endif

ShortcutManager::ShortcutManager(SystemTrayHandler* trayHandler, QObject *parent)
    : QObject(parent), m_trayHandler(trayHandler), m_hotkey(nullptr)
{
    checkForConflictingShortcuts();

#ifdef Q_OS_MAC
    if (!checkAccessibilityPermissions()) {
        qDebug() << "Accessibility permissions not granted!";
        return;
    }

    if (!isFnKeyAvailable()) {
        qDebug() << "Fn key is not available on this system!";
        return;
    }
#endif

    // Choose appropriate shortcut based on platform
#ifdef Q_OS_MAC
    // Try different ways to specify the shortcut with Fn key
    QKeySequence sequences[] = {
        QKeySequence("Meta+Space"),      // Command + Space
        QKeySequence("Alt+Space"),       // Option + Space // This Works
        QKeySequence("Meta+Alt+Space"),  // Command + Option + Space
        QKeySequence(Qt::META | Qt::Key_Space),  // Alternative syntax
        QKeySequence(Qt::ALT | Qt::Key_Space)    // Alternative syntax
    };
#else
    // For other platforms, use standard shortcuts
    QKeySequence sequences[] = {
        QKeySequence("Ctrl+Space"),
        QKeySequence(Qt::CTRL | Qt::Key_Space)
    };
#endif

    bool registered = false;
    for (size_t i = 0; i < sizeof(sequences)/sizeof(sequences[0]); i++) {
        const auto& sequence = sequences[i];
        m_hotkey = new QHotkey(sequence, true, this);
        if (m_hotkey->isRegistered()) {
            qDebug() << "Hotkey registered successfully with sequence:" << sequence.toString();
            registered = true;
            break;
        }
        qDebug() << "Failed to register hotkey with sequence:" << sequence.toString();
        delete m_hotkey;
        m_hotkey = nullptr;
    }

    if (registered) {
        connect(m_hotkey, &QHotkey::activated, this, &ShortcutManager::onHotkeyActivated);
    } else {
        qDebug() << "Failed to register hotkey with any sequence!";
    }
}

ShortcutManager::~ShortcutManager()
{
    delete m_hotkey;
}

void ShortcutManager::onHotkeyActivated()
{
    qDebug() << "Hotkey activated!";

    if (m_trayHandler && m_trayHandler->trayIcon()) {
        QString message;
#ifdef Q_OS_MAC
        message = "You pressed Command+Space!";  // Update based on which shortcut gets registered
#else
        message = "You pressed Ctrl+Space!";
#endif

        m_trayHandler->trayIcon()->showMessage(
            "Shortcut Activated", 
            message,
            QSystemTrayIcon::Information,
            3000
        );
    } else {
        qDebug() << "Tray icon not available!";
    }
}

void ShortcutManager::testNotification()
{
    qDebug() << "Testing notification system";
    if (m_trayHandler && m_trayHandler->trayIcon()) {
        m_trayHandler->trayIcon()->showMessage(
            "Test Notification", 
            "This is a test notification",
            QSystemTrayIcon::Information,
            3000
        );
    }
}

void ShortcutManager::checkForConflictingShortcuts()
{
#ifdef Q_OS_MAC
    // This function will be implemented in macOSShortcutManager.mm
#endif
}

bool ShortcutManager::isFnKeyAvailable()
{
#ifdef Q_OS_MAC
    // Check if the Fn key is available on this system
    return true; // Assume it's available on macOS
#else
    return false; // Fn key is typically not available on non-Mac systems
#endif
}

void ShortcutManager::handleHotkeyPressed()
{
    if (m_trayHandler) {
        QSystemTrayIcon* trayIcon = m_trayHandler->trayIcon();
        if (trayIcon) {
            trayIcon->showMessage("Hotkey Pressed", "The hotkey was activated!");
        }
    }
} 
