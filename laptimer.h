#ifndef LAPTIMER_H
#define LAPTIMER_H

#include "gpsmsg.h"
#include <QVector3D>
#include <QTimer>

class laptimer : public QObject
{
    Q_OBJECT

public:
    explicit laptimer(QWidget *parent = 0, gpsMsg *gps = 0);
    ~laptimer();

    void initTimer(QVector3D pos1, QVector3D pos2);
    void stopTimer();
    float readCurrentLapTime();
    float readBestLapTime();
    int readCurrentLapNum();
    int readBestLapNum();

signals:
    void newCurrentLapTime();
    void newLap();

private slots:
    void gpsNewPositionAvailable();
    void gpsNewTimeAvailable();
    void timerTimeout();

private:
    gpsMsg *gps;
    QTimer *timer;

    float currentTime, bestLapTime, lastLapTime;

    QVector3D lastPosition;
    QVector3D line1, line2;

    int currentLapNum, bestLapNum;
};

#endif // LAPTIMER_H
