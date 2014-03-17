#-------------------------------------------------
#
# Project created by QtCreator 2014-02-25T23:02:45
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets serialport xml printsupport

TARGET = RTKdashboard
TEMPLATE = app

ICON = RTKdashboard.icns


SOURCES += main.cpp\
        rtkdashboard.cpp \
    serialport.cpp \
    gpsmsg.cpp \
    serial_settingsdialog.cpp \
    debugdialog.cpp \
    trackmap.cpp \
    car.cpp \
    qcustomplot.cpp \
    receiverdetails.cpp \
    laptimer.cpp

HEADERS  += rtkdashboard.h \
    serialport.h \
    gpsmsg.h \
    serial_settingsdialog.h \
    debugdialog.h \
    trackmap.h \
    car.h \
    qcustomplot.h \
    receiverdetails.h \
    laptimer.h

FORMS    += rtkdashboard.ui \
    serial_settingsdialog.ui \
    debugdialog.ui \
    receiverdetails.ui

OTHER_FILES += \
    RTKdashboard.icns \
    RTKdashboard.png \
    RTKdashboard.ico

RESOURCES += \
    resources.qrc
