#include "laptimer.h"
#include "gpsmsg.h"
#include <QVector3D>
#include <QTimer>

laptimer::laptimer(QWidget *parent, gpsMsg *gps) {
    this->gps = gps;
    timer = new QTimer(this);

    Q_UNUSED(parent);

    finishPoint1.setX(0);
    finishPoint1.setY(0);
    finishPoint1.setZ(0);

    finishPoint2.setX(0);
    finishPoint2.setY(0);
    finishPoint2.setZ(0);

    lastPosition.setX(0);
    lastPosition.setY(0);
    lastPosition.setZ(0);

    lastLapTime = 0.0f;
    bestLapTime = 0.0f;
    currentTime = 0.0f;

    bestLapNum = 0;
    currentLapNum = 0;

    connect(timer, SIGNAL(timeout()), this, SLOT(timerTimeout()) );
    connect(gps, SIGNAL(newPositionAvailable()), this, SLOT(gpsNewPositionAvailable()) );
    //connect(gps, SIGNAL(newTimeAvailable()), this, SLOT(gpsNewTimeAvailable()) );
}

laptimer::~laptimer(){
    lastLapTime = 0;
}

void laptimer::initTimer(QVector3D pos1, QVector3D pos2){
    finishPoint1 = pos1;
    finishPoint2 = pos2;

    lastLapTime = 0.0f;
    bestLapTime = 0.0f;
    currentTime = 0.0f;

    bestLapNum = 0;
    currentLapNum = 0;

    lastPosition.setX( 0 );
    lastPosition.setY( 0 );
    lastPosition.setZ( 0 );

    timer->start(100);

    emit newLap();
}

void laptimer::stopTimer(){
    timer->stop();
}

int laptimer::readCurrentLapNum(){
    return currentLapNum;
}

int laptimer::readBestLapNum(){
    return bestLapNum;
}

float laptimer::readCurrentLapTime(){
    return currentTime;
}

float laptimer::readBestLapTime(){
    return bestLapTime;
}

float laptimer::readLastLapTime(){
    return lastLapTime;
}

void laptimer::gpsNewPositionAvailable(){
    if( isNewLap( QVector3D(gps->llh.longitude, gps->llh.latitude, gps->llh.height), lastPosition) == true ){
        if(currentTime < bestLapTime){
            bestLapTime = currentTime;
            bestLapNum = currentLapNum;
        }

        lastLapTime = currentTime;

        currentLapNum += 1;
        currentTime = 0;

        emit newLap();
    }

    lastPosition.setX( (float)gps->llh.longitude );
    lastPosition.setY( (float)gps->llh.latitude );
    lastPosition.setZ( (float)gps->llh.height );
}

void laptimer::gpsNewTimeAvailable(){
    //currentTime = gps->positionTimeUTC.millisecondsOfWeek;
}


void laptimer::timerTimeout(){
    currentTime += 0.1f;
    emit newCurrentLapTime();
}


bool laptimer::isNewLap(QVector3D currentPosition, QVector3D lastPosition){
    // (x1, y1) and (x2, y2) are points for line along traveled path
    // (x3, y3) and (x4, y4) are points for the threshold line

    // Temporary storage
    float ua, ub, denominator;

    // Calculate the denominator
    denominator = (lastPosition.x() - currentPosition.x())*(finishPoint2.y() - finishPoint1.y()) - (lastPosition.y() - currentPosition.y())*(finishPoint2.x() - finishPoint1.x());

    ua = ((finishPoint2.x() - finishPoint1.x()) * (currentPosition.y() - finishPoint1.y()) - (finishPoint2.y() - finishPoint1.y()) * (currentPosition.x() - finishPoint1.x())) / denominator;
    ub = ((lastPosition.x() - currentPosition.x()) * (currentPosition.y() - finishPoint1.y()) - (lastPosition.y() - currentPosition.y()) * (currentPosition.x() - finishPoint1.x())) / denominator;

    // Calulate the point of intersection
    //long = x1+ua*(x2 - x1);
    //lat = y1+ua*(y2 - y1);

    if( (ua >= 0) && (ua <= 1) && (ub >= 0) && (ub <= 1) ){
        // Paths intersected!
        // TODO: Check heading
        return true;
    }

    return false;
}
