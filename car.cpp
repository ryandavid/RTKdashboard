#include "car.h"

car::car(QBrush carColor){
    color = carColor;
}

QRectF car::boundingRect() const {
    return QRect(-10, -10, 20, 20);
}

void car::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setBrush(color);
    painter->drawEllipse(QPointF(0,0), 10, 10 );

}
