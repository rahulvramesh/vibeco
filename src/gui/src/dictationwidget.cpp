#include "dictationwidget.h"
#include <QVBoxLayout>
#include <QPainter>
#include <QScreen>
#include <QApplication>
#include <QMouseEvent>

DictationWidget::DictationWidget(QWidget *parent)
    : QWidget(parent)
    , m_textLabel(new QLabel("Click to begin dictation", this))
    , m_dotsLabel(new QLabel(this))
    , m_dotsTimer(new QTimer(this))
    , m_dotCount(0)
    , m_dots("")
{
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::WindowTransparentForInput | Qt::WindowDoesNotAcceptFocus);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_ShowWithoutActivating);
    setAttribute(Qt::WA_MacAlwaysShowToolWindow);
    setAttribute(Qt::WA_NoSystemBackground);
    
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(20, 10, 20, 10);
    layout->setSpacing(0);
    
    // Style the text label
    m_textLabel->setStyleSheet("QLabel { color: white; font-size: 16px; }");
    m_textLabel->setAlignment(Qt::AlignCenter);
    
    // Style the dots label
    m_dotsLabel->setStyleSheet("QLabel { color: white; font-size: 16px; }");
    m_dotsLabel->setAlignment(Qt::AlignCenter);
    
    layout->addWidget(m_textLabel);
    layout->addWidget(m_dotsLabel);
    
    // Set up the dots animation timer
    connect(m_dotsTimer, &QTimer::timeout, this, &DictationWidget::updateDots);
    m_dotsTimer->start(500); // Update every 500ms
    
    // Set a fixed size for the widget
    setFixedSize(200, 80);
}

DictationWidget::~DictationWidget()
{
    m_dotsTimer->stop();
}

void DictationWidget::showAtBottom()
{
    if (const QScreen *screen = QApplication::primaryScreen()) {
        const QRect screenGeometry = screen->geometry();
        const int x = (screenGeometry.width() - width()) / 2;
        const int y = screenGeometry.height() - height() - 50; // 50 pixels from bottom
        move(x, y);
    }
    show();
}

void DictationWidget::updateDots()
{
    m_dotCount = (m_dotCount + 1) % 4;
    m_dots = QString(".").repeated(m_dotCount);
    m_dotsLabel->setText(m_dots);
}

void DictationWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Draw rounded rectangle background with semi-transparency
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(0, 0, 0, 180));
    painter.drawRoundedRect(rect(), 10, 10);
}

void DictationWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        emit clicked();
    }
    QWidget::mousePressEvent(event);
} 