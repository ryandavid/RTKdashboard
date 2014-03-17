#ifndef RECEIVERDETAILS_H
#define RECEIVERDETAILS_H

#include <QDialog>
#include "qcustomplot.h"
#include "gpsmsg.h"

namespace Ui {
class receiverDetails;
}

class receiverDetails : public QDialog
{
    Q_OBJECT

public:
    explicit receiverDetails(QWidget *parent = 0, gpsMsg *gps = 0);
    ~receiverDetails();

private slots:
    void newGpsPositionAvailable();
    void newGpsSvInfoAvailable();
    void newGpsSigmaAvailable();
    void newGpsTimeAvailable();
    void newGpsVelocityAvailable();

private:
    Ui::receiverDetails *ui;

    QCPBars *snrL1Bars, *snrL2Bars;
    gpsMsg *gps;
};

#endif // RECEIVERDETAILS_H
