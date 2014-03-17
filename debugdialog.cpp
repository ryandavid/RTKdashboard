#include "debugdialog.h"
#include "ui_debugdialog.h"
#include "gpsmsg.h"

debugDialog::debugDialog(QWidget *parent, gpsMsg *gps) :
    QDialog(parent),
    ui(new Ui::debugDialog)
{
    ui->setupUi(this);

    showInfoMsgs = true;
    showWarningMsgs = true;
    showErrorMsgs = true;

    // Connect the signals and slots
    connect(gps, SIGNAL(connectionChange(bool)), this, SLOT(gpsIsConnected_change(bool)) );
    connect(gps, SIGNAL(packetReceived(int)), this, SLOT(gpsPacketReceived(int)) );
    connect(gps, SIGNAL(recordReceived(int)), this, SLOT(gpsRecordReceived(int)) );
    connect(gps, SIGNAL(rxDataError(int)), this, SLOT(gpsDataError(int)) );

}

debugDialog::~debugDialog()
{
    delete ui;
}

void debugDialog::writeText(int level, QString text){
    switch(level){
        case(0):
            if(showInfoMsgs) ui->console->appendPlainText(text);
            break;

        case(1):
            if(showWarningMsgs) ui->console->appendPlainText(text);
            break;

        case(2):
            if(showErrorMsgs) ui->console->appendPlainText(text);
            break;

        default:
            ui->console->appendPlainText(text);
    }
}


// SLOT
void debugDialog::gpsIsConnected_change(bool newState){
    if(newState == true){
        writeText(0, tr("Connected!") );
    }else{
        writeText(0, tr("Disconnected!") );
    }
}

// SLOT
void debugDialog::gpsPacketReceived(int length){
    writeText(0, tr("Received packet (%1 bytes)!").arg(length) );
}

// SLOT
void debugDialog::gpsRecordReceived(int id){
    writeText(0, tr("Received record (ID: %1)!").arg(id) );
}

// SLOT
void debugDialog::gpsDataError(int error){
    writeText(2, tr("GPS experienced an error (ID: %1)!").arg(error) );
}

void debugDialog::on_infoCheckbox_toggled(bool checked)
{
    showInfoMsgs = checked;
}

void debugDialog::on_warningCheckbox_toggled(bool checked)
{
    showWarningMsgs = checked;
}

void debugDialog::on_errorCheckbox_toggled(bool checked)
{
    showErrorMsgs = checked;
}

void debugDialog::on_clearButton_clicked()
{
    ui->console->clear();
}
