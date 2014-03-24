#include "trackmap.h"
#include <QXmlStreamReader>
#include <QFile>
#include <QDomDocument>
#include <QVector3D>

trackmap::trackmap() {
    trackValidity = false;
}

QRectF trackmap::boundingRect() const {
    return QRect(0, 0, 2000, 2000);
}

void trackmap::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
    //QRectF rect = boundingRect();
    float x1, y1, x2, y2;

    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen(Qt::white);

    for(int i = 0; i < (outsidePath.length() - 1); i++){
        x1 = (outsidePath.at(i)->x() - normalizeXmin) * scaling;
        y1 = (normalizeYmax - outsidePath.at(i)->y()) * scaling;
        x2 = (outsidePath.at(i+1)->x() - normalizeXmin) * scaling;
        y2 = (normalizeYmax - outsidePath.at(i+1)->y()) * scaling;
        painter->drawLine(x1, y1, x2, y2);
    }

    for(int i = 0; i < (insidePath.length() - 1); i++){
        x1 = (insidePath.at(i)->x() - normalizeXmin) * scaling;
        y1 = (normalizeYmax - insidePath.at(i)->y()) * scaling;
        x2 = (insidePath.at(i+1)->x() - normalizeXmin) * scaling;
        y2 = (normalizeYmax - insidePath.at(i+1)->y()) * scaling;
        painter->drawLine(x1, y1, x2, y2);
    }

    // Draw the finish line
    painter->setPen(Qt::red);
    painter->drawLine((finishPoint1.x() - normalizeXmin) * scaling,
                      (normalizeYmax - finishPoint1.y()) * scaling,
                      (finishPoint2.x() - normalizeXmin) * scaling,
                      (normalizeYmax - finishPoint2.y()) * scaling);

}

void trackmap::loadTrackmap(QString path){
    QFile *file = new QFile( path );

    QList<QString> outsideLine;
    QList<QString> insideLine;
    QList<QString> coordinatePairs;
    QDomDocument doc;

    outsidePath.clear();
    insidePath.clear();

    file->open(QIODevice::ReadOnly | QIODevice::Text);

    doc.setContent(file);

    QDomElement root = doc.firstChildElement();

    QDomElement outsideCoordinates = root.elementsByTagName("outsidePath").at(0).firstChildElement("coordinates");
    QDomElement insideCoordinates = root.elementsByTagName("insidePath").at(0).firstChildElement("coordinates");
    QDomElement finishPoint1coordinates = root.elementsByTagName("finishPoint1").at(0).firstChildElement("coordinates");
    QDomElement finishPoint2coordinates = root.elementsByTagName("finishPoint2").at(0).firstChildElement("coordinates");

    outsideLine = outsideCoordinates.text().split("\n");
    insideLine = insideCoordinates.text().split("\n");

    for(int i = 0; i < outsideLine.length(); i++){
        coordinatePairs = outsideLine.at(i).split(",");
        outsidePath.append( new QVector3D( coordinatePairs.at(0).toFloat(), coordinatePairs.at(1).toFloat(), coordinatePairs.at(2).toFloat() ) );
    }

    for(int i = 0; i < insideLine.length(); i++){
        coordinatePairs = insideLine.at(i).split(",");
        insidePath.append( new QVector3D( coordinatePairs.at(0).toFloat(), coordinatePairs.at(1).toFloat(), coordinatePairs.at(2).toFloat() ) );
    }

    coordinatePairs = finishPoint1coordinates.text().split(",");
    finishPoint1.setX(coordinatePairs.at(0).toFloat());
    finishPoint1.setY(coordinatePairs.at(1).toFloat());
    finishPoint1.setZ(coordinatePairs.at(2).toFloat());

    coordinatePairs = finishPoint2coordinates.text().split(",");
    finishPoint2.setX(coordinatePairs.at(0).toFloat());
    finishPoint2.setY(coordinatePairs.at(1).toFloat());
    finishPoint2.setZ(coordinatePairs.at(2).toFloat());

    file->close();

    normalizeCoordinates();
    trackValidity = true;
}


void trackmap::normalizeCoordinates(){
    float tempScalingX, tempScalingY;

    normalizeXmin = 999.9f;
    normalizeXmax = -999.9f;
    normalizeYmin = 999.9f;
    normalizeYmax = -999.9f;

    for(int i = 0; i < outsidePath.length(); i++){
        if(outsidePath.at(i)->x() > normalizeXmax) normalizeXmax = outsidePath.at(i)->x();
        if(outsidePath.at(i)->x() < normalizeXmin) normalizeXmin = outsidePath.at(i)->x();

        if(outsidePath.at(i)->y() > normalizeYmax) normalizeYmax = outsidePath.at(i)->y();
        if(outsidePath.at(i)->y() < normalizeYmin) normalizeYmin = outsidePath.at(i)->y();
    }

    for(int i = 0; i < insidePath.length(); i++){
        if(insidePath.at(i)->x() > normalizeXmax) normalizeXmax = insidePath.at(i)->x();
        if(insidePath.at(i)->x() < normalizeXmin) normalizeXmin = insidePath.at(i)->x();

        if(insidePath.at(i)->y() > normalizeYmax) normalizeYmax = insidePath.at(i)->y();
        if(insidePath.at(i)->y() < normalizeYmin) normalizeYmin = insidePath.at(i)->y();
    }

    QRectF bounding = boundingRect();

    tempScalingX = ((float)bounding.width() - 100.0f) / (normalizeXmax - normalizeXmin);
    if (tempScalingX < 0) tempScalingX *= -1.0f;

    tempScalingY = ((float)bounding.height() - 100.0f) / (normalizeYmax - normalizeYmin);
    if (tempScalingY < 0) tempScalingY *= -1.0f;

    if(tempScalingX < tempScalingY){
        scaling = tempScalingX;
    }else{
        scaling = tempScalingY;
    }
}


QVector3D trackmap::worldToViewCoordinates(QVector3D realCoordinates){
    if(isValid()){
        return QVector3D( ((float)realCoordinates.x() - (float)normalizeXmin) * scaling,
                          ((float)normalizeYmax - (float)realCoordinates.y()) * scaling,
                          0.0f);
    }else{
        return QVector3D( 0, 0, 0);
    }
}

bool trackmap::isValid(){
    return trackValidity;
}

QList<QVector3D> trackmap::readFinishLine(){
    QList<QVector3D> finishLine;

    finishLine.append(finishPoint1);
    finishLine.append(finishPoint2);

    return finishLine;
}
