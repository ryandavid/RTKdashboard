#ifndef RTKDASHBOARD_H
#define RTKDASHBOARD_H

#include <QMainWindow>
#include "gpsmsg.h"
#include "debugdialog.h"
#include <QLabel>
#include <QGraphicsScene>
#include "trackmap.h"
#include "car.h"
#include "laptimer.h"
#include <qcustomplot.h>
#include "receiverdetails.h"

namespace Ui {
class RTKdashboard;
}


class SettingsDialog;

class RTKdashboard : public QMainWindow
{
    Q_OBJECT

public:
    explicit RTKdashboard(QWidget *parent = 0);
    ~RTKdashboard();

private slots:
    void on_actionPorts_triggered();
    void on_actionConnected_triggered();
    void gpsIsConnected_change(bool newState);
    void gpsPacketReceived(int length);
    void gpsRecordReceived(int id);
    void gpsDataError(int error);
    void newGpsPositionAvailable();

    void lapTimerUpdateCurrentTime();
    void lapTimerNewLap();

    void on_actionDebug_triggered();
    void on_actionReset_triggered();

    void on_verticalSlider_sliderMoved(int position);

    void on_actionOpen_Track_Map_triggered();
    void on_actionReceiver_Details_triggered();

private:
    QLabel *dataStatistics;
    int msgsReceived;

    Ui::RTKdashboard *ui;
    gpsMsg *gps;
    debugDialog *debug;

    trackmap *track;
    car *currentCar;
    car *ghostCar;
    QGraphicsScene *scene;
    laptimer *lap;

    receiverDetails *rcvrDetails;

};

#endif // RTKDASHBOARD_H
