#include "hoverslider.h"

HoverSlider::HoverSlider() {}

HoverSlider::HoverSlider(QWidget *parent) : QSlider(parent)
{
    setMouseTracking(true);
}

void HoverSlider::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton){
        double ratio = (double)event->pos().x() / (double)this->width();
        int newVal = ratio * (this->maximum() - this->minimum()) + this->minimum();
        this->setValue(newVal);

        emit sliderMoved(newVal);
        emit valueChanged(newVal);
    }

    QSlider::mousePressEvent(event);
}

void HoverSlider::mouseMoveEvent(QMouseEvent *event)
{
    double ratio = (double)event->pos().x() / (double)this->width();
    int currTime = (int)(ratio * this->maximum() / 1000);
    // qDebug() << currTime;
    emit onHover(event->pos().x(), currTime);

    QSlider::mouseMoveEvent(event);
}

void HoverSlider::leaveEvent(QEvent* event){
    emit onLeave();
    QSlider::leaveEvent(event);
}
