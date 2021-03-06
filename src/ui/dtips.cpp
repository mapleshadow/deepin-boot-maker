#include "dtips.h"
#include <QPropertyAnimation>
#include <QSizePolicy>
#include <QPainter>

#include "dwindowui.h"
#include "dui.h"

DTips::DTips(QWidget *parent):
    QLabel(parent)
{
    m_AttachWidget = parent;
    this->setParent(DWindowUI::CurrentWindow());
    QString qss =
        "DTips { "
        "color:#ebab4c;"
        "font-size: 10px;"
        "margin-bottom: 7px;"
        "}";
    this->setAlignment(Qt::AlignCenter);
    this->setStyleSheet(qss);
    this->setFocusPolicy(Qt::NoFocus);
    this->setFixedHeight(8 + 24);
    this->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding));
    this->setWindowFlags(Qt::WindowStaysOnTopHint );
    m_active = false;
    this->setFrameStyle(Qt::FramelessWindowHint);
}

QPainterPath DrawTipsPath(const QRect& rect, int radius) {
    int triHeight = 8;
    int triWidth = 12;

    QPoint topLeft(rect.x(), rect.y());
    QPoint topRight(rect.x() + rect.width(), rect.y());
    QPoint bottomRight(rect.x() + rect.width(), rect.y() + rect.height() - triHeight);
    QPoint bottomLeft(rect.x(), rect.y() + rect.height() - triHeight);
    QPoint cornerPoint(rect.x() + rect.width() / 2, rect.y() + rect.height());
    QPainterPath border;
    border.moveTo(topLeft.x() + radius, topLeft.y());
    border.lineTo(topRight.x() - radius, topRight.y());
    border.arcTo(topRight.x() - 2 * radius, topRight.y(), 2 * radius, 2 * radius, 90, -90);
    border.lineTo(bottomRight.x(), bottomRight.y() - radius);
    border.arcTo(bottomRight.x() - 2 * radius, bottomRight.y() - 2 * radius, 2 * radius, 2 * radius, 0, -90);
    border.lineTo(cornerPoint.x() + triWidth / 2, cornerPoint.y() - triHeight);
    border.lineTo(cornerPoint);
    border.lineTo(cornerPoint.x() - triWidth / 2, cornerPoint.y() - triHeight);
    border.lineTo(bottomLeft.x() + radius, bottomLeft.y());
    border.arcTo(bottomLeft.x(), bottomLeft.y() - 2 * radius, 2 * radius, 2 * radius, -90, -90);
    border.lineTo(topLeft.x(), topLeft.y() + radius);
    border.arcTo(topLeft.x(), topLeft.y(), 2 * radius, 2 * radius, 180, -90);
    return border;
}

void DTips::paintEvent(QPaintEvent *e) {
    QPainter painter(this);
    painter.save();
    painter.setRenderHint(QPainter::Antialiasing);

    QRect rect = QLabel::rect();

    QPainterPath border = DrawTipsPath(rect, 4);
    QRect shadowRect = QRect(rect.x() + 1, rect.y()+1, rect.width() - 2, rect.height() - 1);
    QPainterPath shadowBorder = DrawTipsPath(shadowRect, 4);

    QPen borderPen(DUI::TipsBorderColor);
    painter.strokePath(border, borderPen);

    painter.fillPath(shadowBorder, QBrush(DUI::TipsBackground));

    QLabel::paintEvent(e);
}

void DTips::pack() {
    this->hide();
}

void DTips::pop() {
    if (m_active){
        return;
    }
    m_active = true;
    this->show();
    QPropertyAnimation *movie = new QPropertyAnimation(this, "geometry");
    movie->setDuration(300);
    movie->setEasingCurve(QEasingCurve::InOutCubic);
    QPoint pos = m_AttachWidget->mapToGlobal(m_AttachWidget->pos()) - m_AttachWidget->pos();
    QPoint attachPos = this->mapFromGlobal(pos);
    QSize szLabel = m_AttachWidget->size();

    QSize sz = this->size();
    movie->setStartValue(QRect(attachPos.x() + szLabel.width() / 2 , attachPos.y() - szLabel.height()/2 + 5, 0, 0));
    movie->setEndValue(QRect(attachPos.x() + szLabel.width() / 2 - sz.width()/2,
                                 attachPos.y() - sz.height() + 5,
                                 sz.width(),
                                 sz.height()));
    movie->start();
    connect(movie, SIGNAL(finished()), this, SLOT(popDone()));
}

void DTips::popDone(){
    m_active = false;
    emit poped();
}
