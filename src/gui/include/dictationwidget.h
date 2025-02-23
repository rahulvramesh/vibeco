#ifndef DICTATIONWIDGET_H
#define DICTATIONWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QTimer>

class DictationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DictationWidget(QWidget *parent = nullptr);
    ~DictationWidget();

    void showAtBottom();
    void updateDots();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    QLabel *m_textLabel;
    QLabel *m_dotsLabel;
    QTimer *m_dotsTimer;
    int m_dotCount;
    QString m_dots;

signals:
    void clicked();
};

#endif // DICTATIONWIDGET_H 