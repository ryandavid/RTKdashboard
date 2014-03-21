#include "rtkdashboard.h"
#include "ui_rtkdashboard.h"
#include "gpsmsg.h"
#include <QMessageBox>
#include "debugdialog.h"
#include "math.h"
#include <QGraphicsScene>
#include <QGraphicsView>
#include "trackmap.h"
#include "car.h"
#include "laptimer.h"
#include <QPalette>
#include <QDialog>
#include <QDir>

RTKdashboard::RTKdashboard(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::RTKdashboard)
{
    ui->setupUi(this);

    gps = new gpsMsg;
    lap = new laptimer(this, gps);
    debug = new debugDialog(this, gps);
    rcvrDetails = new receiverDetails(this, gps);

    scene = new QGraphicsScene();
    ui->graphicsView->setScene(scene);
    ui->graphicsView->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::HighQualityAntialiasing);
    ui->graphicsView->show();

    currentCar = new car(Qt::red);
    track = new trackmap();

    scene->addItem(currentCar);
    scene->addItem(track);

    dataStatistics = new QLabel(this);
    dataStatistics->setText( "Rx: 0 messages" );
    ui->statusBar->addPermanentWidget(dataStatistics, 0);
    ui->statusBar->showMessage("Disconnected");

    // Connect the signals and slots
    connect(gps, SIGNAL(connectionChange(bool)), this, SLOT(gpsIsConnected_change(bool)) );
    connect(gps, SIGNAL(packetReceived(int)), this, SLOT(gpsPacketReceived(int)) );
    connect(gps, SIGNAL(recordReceived(int)), this, SLOT(gpsRecordReceived(int)) );
    connect(gps, SIGNAL(rxDataError(int)), this, SLOT(gpsDataError(int)) );
    connect(gps, SIGNAL(newPositionAvailable()), this, SLOT(newGpsPositionAvailable()) );

    connect(lap, SIGNAL(newCurrentLapTime()), this, SLOT(lapTimerUpdateCurrentTime()) );
    connect(lap, SIGNAL(newLap()), this, SLOT(lapTimerNewLap()) );
}

RTKdashboard::~RTKdashboard(){
    delete gps;
    delete debug;
    delete rcvrDetails;
    delete lap;
    delete ui;
}

// SLOT
void RTKdashboard::on_actionPorts_triggered(){
    gps->showSettingsDialog();
}

// SLOT
void RTKdashboard::on_actionConnected_triggered(){
    if( !gps->isConnected() ){
        gps->connect();
    }else{
        gps->disconnect();

    }
}


// SLOT
void RTKdashboard::gpsIsConnected_change(bool newState){
    if(newState == true){
        lap->initTimer( QVector3D(0, 0, 0), QVector3D(0,0,0) );
        ui->statusBar->showMessage("Connected");
    }else{
        lap->stopTimer();
        ui->statusBar->showMessage("Disconnected");
    }

    // Force the menu bar icon to reflect the correct state
    ui->actionPorts->setEnabled(!newState);
    ui->actionConnected->setChecked(newState);
}

// SLOT
void RTKdashboard::gpsPacketReceived(int length){
    length = 0;
    dataStatistics->setText( tr("Rx: %1 packets").arg(gps->getRxPackets()) );
}

// SLOT
void RTKdashboard::gpsRecordReceived(int id){
    Q_UNUSED(id)
}

// SLOT
void RTKdashboard::gpsDataError(int error){
    Q_UNUSED(error)
}

void RTKdashboard::on_actionDebug_triggered()
{
    debug->show();
}

void RTKdashboard::on_actionReset_triggered(){
    ui->graphicsView->setTransform( QTransform::fromScale(1, 1) );
    ui->graphicsView->centerOn(0, 0);
    ui->verticalSlider->setValue(50);
}

void RTKdashboard::on_verticalSlider_sliderMoved(int position)
{
    ui->graphicsView->setTransform(QTransform::fromScale( (float)position/50.0f, (float)position/50.0f));
}

void RTKdashboard::on_actionOpen_Track_Map_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this, "Open Trackmap", QDir::homePath(), tr("Trackmaps (*.xml)"));

    if(filename != QString::null){
        track->loadTrackmap(filename);

        ui->graphicsView->setTransform( QTransform::fromScale(1, 1) );
        ui->graphicsView->centerOn(0, 0);
        ui->verticalSlider->setValue(50);
    }
}

void RTKdashboard::on_actionReceiver_Details_triggered()
{
    rcvrDetails->show();
}

void RTKdashboard::newGpsPositionAvailable(){
    QVector3D carViewCoordinates;

    carViewCoordinates = track->worldToViewCoordinates(QVector3D(gps->llh.longitude, gps->llh.latitude, 0));
    currentCar->setPos(carViewCoordinates.x(), carViewCoordinates.y());
}

void RTKdashboard::lapTimerUpdateCurrentTime(){
    ui->label_currentLapTime->setText( QString::number(lap->readCurrentLapTime(), 'f', 1) );
}

void RTKdashboard::lapTimerNewLap(){
    ui->label_currentLap->setText( QString::number(lap->readCurrentLapNum()) );
    ui->label_currentLapTime->setText( QString::number(lap->readCurrentLapTime(), 'f', 1) );

    if(lap->readCurrentLapNum() == 0){
        ui->label_BestLap->setText( tr("N/A") );
        ui->label_BestLapTime->setText( "N/A" );
        ui->label_lastLapTime->setText( "N/A" );
    }else{
        ui->label_BestLap->setText( QString::number(lap->readBestLapNum()) );
        ui->label_BestLapTime->setText( QString::number(lap->readBestLapTime(), 'f', 3) );
        ui->label_lastLapTime->setText( QString::number(lap->readLastLapTime()) );
    }

}
