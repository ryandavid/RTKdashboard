#ifndef CAR_H
#define CAR_H
#include <QGraphicsItem>
#include <QPainter>

class car : public QGraphicsItem
{
public:
    car(QBrush carColor);

    QRectF boundingRect() const;

    // overriding paint()
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    QBrush color;

};

#endif // CAR_H
