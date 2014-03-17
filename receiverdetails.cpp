#include "receiverdetails.h"
#include "ui_receiverdetails.h"
#include "qcustomplot.h"
#include "gpsmsg.h"

receiverDetails::receiverDetails(QWidget *parent, gpsMsg *gps) :
    QDialog(parent),
    ui(new Ui::receiverDetails)
{
    ui->setupUi(this);
    this->gps = gps;

    snrL1Bars = new QCPBars(ui->snrBars->xAxis, ui->snrBars->yAxis);
    snrL2Bars = new QCPBars(ui->snrBars->xAxis, ui->snrBars->yAxis);

    ui->snrBars->addPlottable(snrL1Bars);
    ui->snrBars->addPlottable(snrL2Bars);
    ui->snrBars->setBackground( this->palette().color(QWidget::backgroundRole()) );

    QPen pen;
    pen.setWidthF(1);

    pen.setColor(QColor(255, 131, 0));
    snrL1Bars->setPen(pen);
    snrL1Bars->setBrush(QColor(255, 131, 0, 50));

    pen.setColor(QColor(1, 92, 191));
    snrL2Bars->setPen(pen);
    snrL2Bars->setBrush(QColor(1, 92, 191, 50));

    //ui->snrBars->xAxis->setRange(0.5, 12.5);
    ui->snrBars->xAxis->setAutoTickStep(false);
    ui->snrBars->xAxis->setAutoTickLabels(false);
    ui->snrBars->xAxis->setTickStep(1);
    ui->snrBars->xAxis->setSubTickLength(0);
    ui->snrBars->xAxis->setSubTickCount(0);
    ui->snrBars->xAxis->setLabel("PRN");

    ui->snrBars->yAxis->setRange(0, 65);
    ui->snrBars->yAxis->setLabel("L1/L2 SNR");



    ui->snrBars->xAxis->setTickVectorLabels(QVector<QString>() << "" << "" << "" << "" << "" << "" << "" << "" << "" << ""<< "" << "" << "");
    QVector<double> receivedL1SNR, receivedL2SNR, ticks;

    ticks << 1 << 2 << 3 << 4 << 5 << 6 << 7 << 8 << 9 << 10 << 11 << 12;
    receivedL1SNR << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0;
    receivedL2SNR << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0;

    snrL1Bars->setData(ticks, receivedL1SNR);
    snrL2Bars->setData(ticks, receivedL2SNR);

    ui->snrBars->replot();


    connect(gps, SIGNAL(newPositionAvailable()), this, SLOT(newGpsPositionAvailable()) );
    connect(gps, SIGNAL(newSVinfoAvailable()), this, SLOT(newGpsSvInfoAvailable()) );
    connect(gps, SIGNAL(newSigmaAvailable()), this, SLOT(newGpsSigmaAvailable()) );
    connect(gps, SIGNAL(newTimeAvailable()), this, SLOT(newGpsTimeAvailable()) );
    connect(gps, SIGNAL(newVelocityAvailable()), this, SLOT(newGpsVelocityAvailable()) );

}

receiverDetails::~receiverDetails()
{
    delete ui;
}


void receiverDetails::newGpsPositionAvailable(){
    ui->labelLatitude->setText( QString::number(gps->llh.latitude, 'f', 8) + " deg" );
    ui->labelLongitude->setText( QString::number(gps->llh.longitude, 'f', 8) + " deg" );
    ui->labelAltitude->setText( QString::number(gps->llh.height, 'f', 3) + " m" );
}


void receiverDetails::newGpsSvInfoAvailable(){
    QVector<QString> prnText;
    QVector<double> receivedL1SNR, receivedL2SNR, ticks;

    ui->labelSVs->setText( QString::number(gps->svInfo[0].numSv) );

    prnText.append(""); // Blank out the first X Axis label
    for(int i = 0; i < gps->svInfo[0].numSv; i++){
        prnText.append( QString::number(gps->svInfo[i].prn & 0xFF) );
        receivedL1SNR.append( gps->svInfo[i].snrL1 );
        receivedL2SNR.append( gps->svInfo[i].snrL2 );
        ticks.append(i);
    }

    ui->snrBars->xAxis->setRange(0.5f, gps->svInfo[0].numSv + 0.5f);
    ui->snrBars->xAxis->setTickVectorLabels(prnText);

    snrL1Bars->setData(ticks, receivedL1SNR);
    snrL2Bars->setData(ticks, receivedL2SNR);

    ui->snrBars->replot();
}


void receiverDetails::newGpsSigmaAvailable(){
    ui->labelSigmaEast->setText( QString::number(gps->sigma.sigmaEast, 'f', 3) + " m" );
    ui->labelSigmaNorth->setText( QString::number(gps->sigma.sigmaNorth, 'f', 3) + " m" );
    ui->labelSigmaUp->setText( QString::number(gps->sigma.sigmaUp, 'f', 3) + " m" );
    ui->labelRMS->setText( QString::number(gps->sigma.rms, 'f', 3) + " m" );
}

void receiverDetails::newGpsTimeAvailable(){
    ui->labelTime->setText( QString::number(gps->positionTimeUTC.millisecondsOfWeek) );


    if( gps->positionTimeUTC.isDifferential & gps->positionTimeUTC.isPhase & gps->positionTimeUTC.isFixedInteger){
        ui->labelMode->setText("RTK Fixed Integer");

    }else if( gps->positionTimeUTC.isDifferential & gps->positionTimeUTC.isPhase & !gps->positionTimeUTC.isFixedInteger){
        ui->labelMode->setText("RTK Float");

    }else if( !gps->positionTimeUTC.isDifferential & !gps->positionTimeUTC.isFixedInteger & !gps->positionTimeUTC.isStatic){
        ui->labelMode->setText("WAAS");
    }
    else if( gps->positionTimeUTC.isDifferential & !gps->positionTimeUTC.isFixedInteger & !gps->positionTimeUTC.isStatic){
        ui->labelMode->setText("Autonomous");
    }else{
        ui->labelMode->setText("Invalid");
    }


}

void receiverDetails::newGpsVelocityAvailable(){
    ui->labelHeading->setText( QString::number(gps->velocity.heading, 'f', 1) + " deg" );
    ui->labelVelocity->setText( QString::number(gps->velocity.horizontalVelocity, 'f', 3) + " m/s" );
}
