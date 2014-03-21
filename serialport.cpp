#include "serialport.h"

#include <QtSerialPort/QSerialPort>
#include <QObject>
#include <serial_settingsdialog.h>

serialPort::serialPort(){
    connected = false;
    portName = new QString("serial1");       // Default to serial1
    baudRate = 115200;          // Default to 115200 baud
    parity = 0;                 // Default to no parity
    stopBits = 0;               // Default to no stop bits
    dataBits = 8;               // Default to 8 bits
    flowControl = false;        // Default to no flow control

    serial = new QSerialPort();
    settings = new SettingsDialog();

    QObject::connect(serial, SIGNAL(readyRead()), this, SLOT(readyRead()));
}

serialPort::~serialPort(){
    delete settings;

    if(connected){
        serial->close();
    }
}


bool serialPort::isConnected(){
    return connected;
}


bool serialPort::showSettingsDialog(){
    if(connected){
        return false;
    }else{
        settings->show();
        return true;
    }
}

void serialPort::connect(){
    SettingsDialog::Settings currentSettings = settings->settings();

    if(!connected){
        //serial->setPortName("serial1");
        serial->setPortName(currentSettings.name);

        if( serial->open(QIODevice::ReadWrite) ){
            connected = true;
        }else{
            connected = false;
        }

        serial->setBaudRate(currentSettings.baudRate);
        serial->setDataBits(currentSettings.dataBits);
        serial->setParity(currentSettings.parity);
        serial->setStopBits(currentSettings.stopBits);
        serial->setFlowControl(currentSettings.flowControl);
    }
}

void serialPort::disconnect(){
    serial->close();
    connected = false;
}

QByteArray serialPort::readData(){
    return serial->readAll();
}


void serialPort::readyRead(){
    emit newDataAvailable();
}

