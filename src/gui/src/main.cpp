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
#include "QmlDictationManager.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Set application information
    app.setApplicationName("Vibeco");
    app.setOrganizationName("Vibeco");
    app.setApplicationVersion("0.1.0");

    QQmlApplicationEngine engine;

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

    // Create system tray handler with engine and app
    SystemTrayHandler* trayHandler = new SystemTrayHandler(&engine, &app);
    engine.rootContext()->setContextProperty("trayHandler", trayHandler);

    // Create and register ShortcutManager
    ShortcutManager* shortcutManager = new ShortcutManager(trayHandler, &app);
    engine.rootContext()->setContextProperty("shortcutManager", shortcutManager);

    const QUrl url(QStringLiteral("qrc:/main.qml"));

    // Connect to objectCreated signal to get a reference to the main window
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url, trayHandler](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl) {
            QCoreApplication::exit(-1);
            return;
        }

        if (url == objUrl) {
            // obj is the main window
            trayHandler->setMainWindow(obj);

            // Connect signal for transcription reception
            QObject::connect(trayHandler, &SystemTrayHandler::transcriptionReceived,
                             obj, [obj](const QString& text, double duration, const QString& language) {
                // Call the QML method to add a transcription
                QVariantMap transcription;
                transcription["text"] = text;
                transcription["timestamp"] = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
                transcription["duration"] = duration;
                transcription["language"] = language;

                QMetaObject::invokeMethod(obj, "onTranscriptionReceived",
                                          Q_ARG(QVariant, QVariant::fromValue(transcription)));
            });
        }
    }, Qt::QueuedConnection);

    engine.load(url);

    return app.exec();
}