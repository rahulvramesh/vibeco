#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QDir>
#include "systemtrayhandler.h"
#include <QQmlEngine>
#include <QQmlContext>
#include "ShortcutManager.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    QQmlApplicationEngine engine;
    
    SystemTrayHandler trayHandler;

    // Add debug output
    qDebug() << "Current working directory:" << QDir::currentPath();
    qDebug() << "QML File exists:" << QFile::exists(":/main.qml");
    qDebug() << "Available resource files:";
    QDir resourceDir(":/");
    for(const QString &file : resourceDir.entryList()) {
        qDebug() << "  -" << file;
    }
    
    // Register Style singleton
    qmlRegisterSingletonType(QUrl("qrc:/Style.qml"), "com.vibeco.style", 1, 0, "Style");
    
    // Create and register ShortcutManager
    ShortcutManager* shortcutManager = new ShortcutManager(&trayHandler, &app);
    engine.rootContext()->setContextProperty("shortcutManager", shortcutManager);
    
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    
    engine.load(url);

    return app.exec();
}
