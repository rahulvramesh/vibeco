#ifndef SHORTCUTMANAGER_H
#define SHORTCUTMANAGER_H

#include <QObject>
#include <QHotkey>
#include <QSystemTrayIcon>

// Forward declaration
class SystemTrayHandler;

class ShortcutManager : public QObject
{
    Q_OBJECT

public:
    explicit ShortcutManager(SystemTrayHandler* trayHandler, QObject *parent = nullptr);
    ~ShortcutManager();
    Q_INVOKABLE void testNotification();

private slots:
    void onHotkeyActivated();
    void handleHotkeyPressed();

private:
    bool isFnKeyAvailable();
    void checkForConflictingShortcuts();
    QHotkey* m_hotkey;
    SystemTrayHandler* m_trayHandler;
};

#endif // SHORTCUTMANAGER_H 