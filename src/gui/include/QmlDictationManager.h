#ifndef QMLDICTATIONMANAGER_H
#define QMLDICTATIONMANAGER_H

#include <QObject>
#include <QQmlApplicationEngine>
#include <QQuickWindow>

class QmlDictationManager : public QObject
{
    Q_OBJECT

public:
    explicit QmlDictationManager(QQmlApplicationEngine *engine, QObject *parent = nullptr);
    ~QmlDictationManager();

    public slots:
        void showDictationWidget();
    void hideDictationWidget();
    void setRecordingState(bool recording);

    signals:
        void dictationWidgetClicked();

private:
    QQmlApplicationEngine *m_engine;
    QQuickWindow *m_dictationWindow;

    void createDictationWindow();
};

#endif // QMLDICTATIONMANAGER_H