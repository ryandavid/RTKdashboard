#include "laptimer.h"
#include "gpsmsg.h"
#include <QVector3D>
#include <QTimer>

laptimer::laptimer(QWidget *parent, gpsMsg *gps) {
    this->gps = gps;
    timer = new QTimer(this);

    Q_UNUSED(parent);

    line1.setX(0);
    line1.setY(0);
    line1.setZ(0);

    line2.setX(0);
    line2.setY(0);
    line2.setZ(0);

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
    line1 = pos1;
    line2 = pos2;

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

void laptimer::gpsNewPositionAvailable(){
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
