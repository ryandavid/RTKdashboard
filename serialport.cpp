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

int serialPort::getBaudRate(){
    return baudRate;
}

void serialPort::setBaudRate(int newRate){
    switch(newRate){
        case(9600):
            baudRate = 9600;
            break;

        case(38400):
            baudRate = 38400;
            break;

        case(57600):
            baudRate = 57600;
            break;

        case(115200):
            baudRate = 115200;
            break;

        default:
            baudRate = 115200;
    }
}

int serialPort::getParity(){
    return parity;
}

void serialPort::setParity(int par){

}


QString *serialPort::getPortName(){
    return portName;
}

void serialPort::setPortName(QString &newName){
    //portName = *newName;
}

int serialPort::getStopBits(){
    return stopBits;
}

void serialPort::setStopBits(int bits){
    stopBits = bits;    // TODO: Validate this input
}


int serialPort::getDataBits(){
    return dataBits;
}

void serialPort::setDataBits(int bits){
    dataBits = bits;    // TODO: Validate this input
}

bool serialPort::getFlowControl(){
    return flowControl;
}

void serialPort::setFlowControl(bool fc){
    flowControl = fc;
}


void serialPort::connect(){
    if(!connected){
        serial->setPortName("serial1");

        if( serial->open(QIODevice::ReadWrite) ){
            connected = true;
        }else{
            connected = false;
        }

        serial->setBaudRate(QSerialPort::Baud115200);
        serial->setDataBits(QSerialPort::Data8);
        serial->setParity(QSerialPort::NoParity);
        serial->setStopBits(QSerialPort::OneStop);
        serial->setFlowControl(QSerialPort::NoFlowControl);
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

