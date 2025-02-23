#include "dictationwidget.h"
#include <QVBoxLayout>
#include <QPainter>
#include <QScreen>
#include <QApplication>
#include <QMouseEvent>
#include <QPropertyAnimation>

DictationWidget::DictationWidget(QWidget *parent)
    : QWidget(parent, Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::WindowDoesNotAcceptFocus)
    , m_textLabel(new QLabel("Record", this))
    , m_isHovered(false)
    , m_animation(new QPropertyAnimation(this, "height"))
{
    // Set individual attributes (corrected from setAttributes)
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_ShowWithoutActivating);
    setAttribute(Qt::WA_MacAlwaysShowToolWindow);
    setAttribute(Qt::WA_NoSystemBackground);
    setMouseTracking(true);

    // Simplified layout
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(2, 2, 2, 2);
    layout->setSpacing(0);
    m_textLabel->setStyleSheet("QLabel { color: white; font-size: 10px; }");
    m_textLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(m_textLabel);

    // Set initial size
    setFixedWidth(100);
    setFixedHeight(5);

    // Configure animation
    m_animation->setDuration(150);
    m_animation->setEasingCurve(QEasingCurve::OutCubic);

    // Initially hide the label
    m_textLabel->setVisible(false);
}

DictationWidget::~DictationWidget() {
    // No manual cleanup needed; Qt handles child objects
}

void DictationWidget::showAtBottom() {
    if (const QScreen *screen = QApplication::primaryScreen()) {
        const QRect screenGeometry = screen->geometry();
        const int x = (screenGeometry.width() - width()) / 2;
        const int y = screenGeometry.height() - height() - 30;
        move(x, y);
    }
    show();
}

void DictationWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(0, 0, 0, 180));
    painter.drawRoundedRect(rect(), 10, 10);
}

void DictationWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton && m_isHovered) {
        emit clicked();
    }
    QWidget::mousePressEvent(event);
}

void DictationWidget::enterEvent(QEnterEvent *event) {
    if (!m_isHovered) {
        m_isHovered = true;
        m_animation->stop();
        m_animation->setStartValue(height());
        m_animation->setEndValue(35); // Hover height set to 35
        m_animation->start();

        QTimer::singleShot(50, this, [this]() {
            if (m_isHovered) {
                m_textLabel->setVisible(true);
            }
        });
    }
    QWidget::enterEvent(event);
}

void DictationWidget::leaveEvent(QEvent *event) {
    if (m_isHovered) {
        m_isHovered = false;
        m_animation->stop();
        m_textLabel->setVisible(false);
        m_animation->setStartValue(height());
        m_animation->setEndValue(5);
        m_animation->start();
    }
    QWidget::leaveEvent(event);
}