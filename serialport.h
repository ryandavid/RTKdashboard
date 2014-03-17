#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <QObject>

#include <QtSerialPort/QSerialPort>
#include <serial_settingsdialog.h>

class serialPort : public QObject
{
    Q_OBJECT

public:
    serialPort();
    ~serialPort();

    bool isConnected();      // Read current state, serial port open or closed

    bool showSettingsDialog();

    QString *getPortName();      // Read ASCII port name
    void setPortName(QString &newName);      // Set ASCII port name

    int getBaudRate();      // Read current baud rate
    void setBaudRate(int newRate);      // Set current baud rate

    int getDataBits();      // Get data bits per transfer
    void setDataBits(int bits);      // Set data bits per transfer

    int getParity();
    void setParity(int par);

    int getStopBits();
    void setStopBits(int bits);

    bool getFlowControl();
    void setFlowControl(bool fc);

    void connect();
    void disconnect();

    QByteArray readData();
    void sendData(QByteArray &data);


private:
    bool connected;
    QString *portName;
    int baudRate;
    int parity;
    int stopBits;
    int dataBits;
    bool flowControl;

    QSerialPort *serial;
    SettingsDialog *settings;


public slots:
    void readyRead();

signals:
    void newDataAvailable();


};

#endif // SERIALPORT_H
