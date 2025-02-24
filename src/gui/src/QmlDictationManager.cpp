#include "QmlDictationManager.h"
#include <QQmlComponent>
#include <QDebug>

QmlDictationManager::QmlDictationManager(QQmlApplicationEngine *engine, QObject *parent)
    : QObject(parent)
    , m_engine(engine)
    , m_dictationWindow(nullptr)
{
    createDictationWindow();
}

QmlDictationManager::~QmlDictationManager()
{
    // The m_dictationWindow is owned by QML and will be destroyed automatically
}

void QmlDictationManager::createDictationWindow()
{
    QQmlComponent component(m_engine, QUrl("qrc:/DictationWindow.qml"));

    if (component.isError()) {
        qWarning() << "Error loading DictationWindow.qml:" << component.errors();
        return;
    }

    QObject *object = component.create();
    if (!object) {
        qWarning() << "Failed to create DictationWindow object";
        return;
    }

    m_dictationWindow = qobject_cast<QQuickWindow*>(object);
    if (!m_dictationWindow) {
        qWarning() << "Created object is not a QQuickWindow";
        delete object;
        return;
    }

    // Connect signals
    QObject::connect(object, SIGNAL(dictationClicked()),
                     this, SIGNAL(dictationWidgetClicked()));

    qDebug() << "Dictation window created successfully";
}

void QmlDictationManager::showDictationWidget()
{
    if (m_dictationWindow) {
        QMetaObject::invokeMethod(m_dictationWindow, "show");
        qDebug() << "Showing dictation widget";
    } else {
        qWarning() << "Cannot show dictation widget: window not created";
    }
}

void QmlDictationManager::hideDictationWidget()
{
    if (m_dictationWindow) {
        QMetaObject::invokeMethod(m_dictationWindow, "hide");
        qDebug() << "Hiding dictation widget";
    }
}

void QmlDictationManager::setRecordingState(bool recording)
{
    if (m_dictationWindow) {
        QMetaObject::invokeMethod(m_dictationWindow, "setRecording",
                                  Q_ARG(QVariant, recording));
        qDebug() << "Setting dictation widget recording state:" << recording;
    }
}