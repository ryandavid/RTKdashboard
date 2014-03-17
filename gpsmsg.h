#ifndef GPSMSG_H
#define GPSMSG_H

#include <QObject>
#include "serialport.h"

class GSOF_2 {
public:
    double latitude;    // Latitude in WGS-84 datum
    double longitude;   // Longitude in WGS084 datum
    double height;      // Height in WGS-84 datum

    GSOF_2(){
        latitude = 0;
        longitude = 0;
        height = 0;
    }
};

class GSOF_8 {
public:
    bool velocityValid;
    bool computeMethod;

    float horizontalVelocity;   // Horizontal velocity in m/s
    float heading;              // WGS-84 referenced true north heading
    float verticalVelocity;     // Vertical velocity in m/s

    GSOF_8(){
        velocityValid = false;
        computeMethod = false;

        horizontalVelocity = 0;
        heading = 0;
        verticalVelocity = 0;
    }
};

class GSOF_12 {
public:
    float rms;
    float sigmaEast;
    float sigmaNorth;
    float covariance;
    float sigmaUp;
    float majorAxis;
    float minorAxis;
    float orientation;
    float unitVariance;
    int numEpochs;

    GSOF_12(){
        rms = 0.0f;
        sigmaEast = 0.0f;
        sigmaNorth = 0.0f;
        covariance = 0.0f;
        sigmaUp = 0.0f;
        majorAxis = 0.0f;
        minorAxis = 0.0f;
        orientation = 0.0f;
        unitVariance = 0.0f;
        numEpochs = 0;
    }
};

class GSOF_14 {
public:
    int numSv;
    int prn;

    bool satelliteAboveHorizon;
    bool assignedToChannel;
    bool trackedL1;
    bool trackedL2;
    bool trackedAtBaseL1;
    bool trackedAtBaseL2;
    bool usedInSolution;
    bool usedInRTK;

    bool trackedPcodeL1;
    bool trackedPcodeL2;

    int elevation;
    int azimuth;
    float snrL1;
    float snrL2;

    GSOF_14(){
        numSv = 0;
        prn = 0;
        satelliteAboveHorizon = false;
        assignedToChannel = false;
        trackedL1 = false;
        trackedL2 = false;
        trackedAtBaseL1 = false;
        trackedAtBaseL2 = false;
        usedInSolution = false;
        usedInRTK = false;
        trackedPcodeL1 = false;
        trackedPcodeL2 = false;
        elevation = 0;
        azimuth = 0;
        snrL1 = 0;
        snrL2 = 0;
    }

};

class GSOF_26 {
public:
    int millisecondsOfWeek;
    int gpsWeekNumber;
    int numOfSV;

    bool newPosition;
    bool newClockThisSolution;
    bool newHorizontalThisSolution;
    bool newHeightThisSolution;
    bool usesLeastSquaresPosition;
    bool usesFilteredL1;

    bool isDifferential;
    bool isPhase;
    bool isFixedInteger;
    bool isOmnistar;
    bool isStatic;
    bool isNetworkRTK;

    int initCounter;

    GSOF_26(){
        millisecondsOfWeek = 0;
        gpsWeekNumber = 0;
        numOfSV = 0;

        newPosition = 0;
        newClockThisSolution = 0;
        newHorizontalThisSolution = 0;
        newHeightThisSolution = 0;
        usesLeastSquaresPosition = 0;
        usesFilteredL1 = 0;

        isDifferential = 0;
        isPhase = 0;
        isFixedInteger = 0;
        isOmnistar = 0;
        isStatic = 0;
        isNetworkRTK = 0;
        initCounter = 0;
    }
};


class gpsMsg : public QObject
{
    Q_OBJECT

public:
    gpsMsg();
    ~gpsMsg();

    bool isConnected();
    void connect();
    void disconnect();
    bool showSettingsDialog();
    int getRxPackets();
    int getRxRecords();

    GSOF_2 llh;
    GSOF_12 sigma;
    GSOF_14 svInfo[24];      // SV Information
    GSOF_26 positionTimeUTC;
    GSOF_8 velocity;

    typedef union _un_double_
    {
            unsigned char bDat[8];
            double dData;
    }UN_DOUBLE;

    typedef union _un_float_
    {
            unsigned char bDat[4];
            float fData;
    }UN_FLOAT;





signals:
    void connectionChange(bool newState);
    void packetReceived(int length);
    void recordReceived(int id);
    void rxDataError(int error);

    void newPositionAvailable();
    void newVelocityAvailable();
    void newFixAvailable();
    void newTimeAvailable();
    void newSVinfoAvailable();
    void newSigmaAvailable();

public slots:
    void rawDataAvailable();

private:
    serialPort *port;
    QByteArray *rawDataBuffer;

    int parserStateMachine;
    int numReceivedRecords;
    int numReceivedPackets;

    int rxRcvrStatus;
    int rxPacketLength;
    int rxTransNumber;
    int rxPageIndex;
    int rxMaxPageIndex;
    int rxRecordType;
    int rxRecordLength;
    QByteArray *rxRecordData;
    int rxChecksum;


    int calculatedChecksum;

    enum gsof {
        PositionTime        = 1,
        LLH                 = 2,
        ECEF                = 3,
        LocalDatum          = 4,
        LocalZone           = 5,
        ECEFDetla           = 6,
        TangentPlaneDelta   = 7,
        Velocity            = 8,
        PDOP                = 9,
        Clock               = 10,
        PositionVCV         = 11,
        PositionSigma       = 12,
        SVBrief             = 13,
        SVDetail            = 14,
        ReceiverSerial      = 15,
        CurrentTime         = 16,
        PositionTimeUTC     = 26,
        Attitude            = 27,
        AllSVBrief          = 33,
        AllDVDetailed       = 34,
        ReceivedBaseInfo    = 35,
        BasePositionQuality = 41
    };

    void processPacket();


};



#endif // GPSMSG_H
