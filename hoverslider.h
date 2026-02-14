#ifndef HOVERSLIDER_H
#define HOVERSLIDER_H

#include <QSlider>
#include <QMouseEvent>

class HoverSlider : public QSlider
{
    Q_OBJECT
public:
    HoverSlider();
    explicit HoverSlider(QWidget* parent = nullptr);

signals:
    void onLeave();
    void onHover(int x, int value);
protected:
    void leaveEvent(QEvent * event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
};

#endif // HOVERSLIDER_H
