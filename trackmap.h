#ifndef TRACKMAP_H
#define TRACKMAP_H

#include <QGraphicsItem>
#include <QPainter>
#include <QList>
#include <QVector3D>

class trackmap : public QGraphicsItem
{
public:
    explicit trackmap();

    QRectF boundingRect() const;

    // overriding paint()
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);


    QVector3D worldToViewCoordinates(QVector3D realCoordinates);
    void loadTrackmap(QString path);
    bool isValid();


private:
    void normalizeCoordinates();

    // Latitude, Longitude, Altitude
    QList<QVector3D*> outsidePath;
    QList<QVector3D*> insidePath;

    float normalizeXmin;
    float normalizeXmax;

    float normalizeYmin;
    float normalizeYmax;

    float scaling;

    bool trackValidity;
};

#endif // TRACKMAP_H
