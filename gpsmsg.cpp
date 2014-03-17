#include "gpsmsg.h"
#include "serialport.h"
#include <QObject>

gpsMsg::gpsMsg(){
    port = new serialPort();
    rawDataBuffer = new QByteArray;
    rxRecordData = new QByteArray;

    parserStateMachine = 0;
    numReceivedPackets = 0;
    numReceivedRecords = 0;

    QObject::connect(port, SIGNAL(newDataAvailable()), this, SLOT(rawDataAvailable()));
}

gpsMsg::~gpsMsg(){
    delete port;
}

bool gpsMsg::isConnected(){
    return port->isConnected();
}

void gpsMsg::connect(){
    port->connect();

    emit connectionChange(this->isConnected());    // Send off the signal
}

void gpsMsg::disconnect(){
    port->disconnect();

    emit connectionChange(this->isConnected());    // Send off the signal
}

bool gpsMsg::showSettingsDialog(){
    return port->showSettingsDialog();
}

int gpsMsg::getRxPackets(){
    return numReceivedPackets;
}

int gpsMsg::getRxRecords(){
    return numReceivedRecords;
}

void gpsMsg::rawDataAvailable(){
    uint rxData;

    rawDataBuffer->append(port->readData());

    // Lets loop through the bytes until we run the buffer dry
    while(rawDataBuffer->length() > 0){

        // Basically 'Popping' the data
        rxData = (rawDataBuffer->at(0) & 0xFF);
        rawDataBuffer->remove(0, 1);

        switch(parserStateMachine){

            // Initialization state... waiting for message
            case(0):
                if(rxData == 0x02){
                    parserStateMachine = 1;
                }else{
                    parserStateMachine = 0;
                }
                break;

            // Stash away the potential 'STATUS' byte
            case(1):
                rxRcvrStatus = rxData;
                parserStateMachine = 2;
                break;

            // Check for packet type (fixed 0x40 = GENOUT)
            case(2):
                if(rxData == 0x40){
                    parserStateMachine = 3;
                }else{
                    // Expected GENOUT byte, received something else
                    // Indicates we didnt properly sync onto a frame
                    emit rxDataError(0);
                    parserStateMachine = 0;
                }
                break;

            case(3):
                rxPacketLength = rxData;
                parserStateMachine = 4;
                break;

            case(4):
                rxTransNumber = rxData;
                parserStateMachine = 5;
                break;

            case(5):
                rxPageIndex = rxData;
                parserStateMachine = 6;
                break;

            case(6):
                rxMaxPageIndex = rxData;
                parserStateMachine = 7;
                break;

            case(7):
                if(rxRecordData->length() < (rxPacketLength - 3)){
                    rxRecordData->append(rxData);
                }else{
                    // This byte is the checksum
                    rxChecksum = rxData;

                    // Need to verify the checksum
                    calculatedChecksum = rxRcvrStatus + 0x40 + rxPacketLength + rxTransNumber + rxPageIndex + rxMaxPageIndex;
                    for(int i = 0; i < rxRecordData->length(); i++){
                        calculatedChecksum += (rxRecordData->at(i) & 0xFF);
                    }
                    calculatedChecksum %= 256;

                    if(calculatedChecksum != rxChecksum){
                        emit rxDataError(3);
                    }

                    parserStateMachine = 8;
                }
                break;

            case(8):
                if(rxData != 0x03){
                    // Expected ETX byte, got something else
                    emit rxDataError(1);

                }else{
                    // Lets use the data
                    numReceivedPackets += 1;
                    emit packetReceived(rxPacketLength);
                    processPacket();

                }

                // Lets get ready for the next message
                rxRecordData->truncate(0);
                parserStateMachine = 0;
                break;


            default:
                // Should never reach this
                parserStateMachine = 0;
                break;
        }

    }

}


void gpsMsg::processPacket(){
    int index = 0;
    gsof recordType;
    int recordLength;
    UN_DOUBLE tempDouble;
    UN_FLOAT tempFloat;

    while( index < rxRecordData->length() ){
        numReceivedRecords += 1;

        recordType = (gsof)rxRecordData->at(index);
        recordLength = rxRecordData->at(index+1);

        emit recordReceived((int)recordType);

        switch(recordType){
        case(PositionTime):

            break;

        case(LLH):
            tempDouble.bDat[0] = rxRecordData->at(index + 9) & 0xFF;
            tempDouble.bDat[1] = rxRecordData->at(index + 8) & 0xFF;
            tempDouble.bDat[2] = rxRecordData->at(index + 7) & 0xFF;
            tempDouble.bDat[3] = rxRecordData->at(index + 6) & 0xFF;
            tempDouble.bDat[4] = rxRecordData->at(index + 5) & 0xFF;
            tempDouble.bDat[5] = rxRecordData->at(index + 4) & 0xFF;
            tempDouble.bDat[6] = rxRecordData->at(index + 3) & 0xFF;
            tempDouble.bDat[7] = rxRecordData->at(index + 2) & 0xFF;
            llh.latitude = tempDouble.dData * 57.2957795f;  // TODO: Change to constant

            tempDouble.bDat[0] = rxRecordData->at(index + 17) & 0xFF;
            tempDouble.bDat[1] = rxRecordData->at(index + 16) & 0xFF;
            tempDouble.bDat[2] = rxRecordData->at(index + 15) & 0xFF;
            tempDouble.bDat[3] = rxRecordData->at(index + 14) & 0xFF;
            tempDouble.bDat[4] = rxRecordData->at(index + 13) & 0xFF;
            tempDouble.bDat[5] = rxRecordData->at(index + 12) & 0xFF;
            tempDouble.bDat[6] = rxRecordData->at(index + 11) & 0xFF;
            tempDouble.bDat[7] = rxRecordData->at(index + 10) & 0xFF;
            llh.longitude = tempDouble.dData * 57.2957795f; // TODO: Change to constant

            tempDouble.bDat[0] = rxRecordData->at(index + 25) & 0xFF;
            tempDouble.bDat[1] = rxRecordData->at(index + 24) & 0xFF;
            tempDouble.bDat[2] = rxRecordData->at(index + 23) & 0xFF;
            tempDouble.bDat[3] = rxRecordData->at(index + 22) & 0xFF;
            tempDouble.bDat[4] = rxRecordData->at(index + 21) & 0xFF;
            tempDouble.bDat[5] = rxRecordData->at(index + 20) & 0xFF;
            tempDouble.bDat[6] = rxRecordData->at(index + 19) & 0xFF;
            tempDouble.bDat[7] = rxRecordData->at(index + 18) & 0xFF;
            llh.height = tempDouble.dData;

            emit newPositionAvailable();
            break;

        case(SVBrief):
            svInfo[0].numSv = rxRecordData->at(index + 2) & 0xFF;

            for(int i = 0; i < svInfo[0].numSv; i++){
                svInfo[i].numSv = svInfo[0].numSv;
                svInfo[i].prn = rxRecordData->at( index + 3 + (i*3) ) & 0xFF;

                svInfo[i].satelliteAboveHorizon = (bool)( rxRecordData->at( index + 4 + (i*3)) & 0b00000001);
                svInfo[i].assignedToChannel = (bool)( rxRecordData->at( index + 4 + (i*3)) & 0b00000010);
                svInfo[i].trackedL1 = (bool)( rxRecordData->at( index + 4 + (i*3)) & 0b00000100);
                svInfo[i].trackedL2 = (bool)( rxRecordData->at( index + 4 + (i*3)) & 0b00001000);
                svInfo[i].trackedAtBaseL1 = (bool)( rxRecordData->at( index + 4 + (i*3)) & 0b00010000);
                svInfo[i].trackedAtBaseL2 = (bool)( rxRecordData->at( index + 4 + (i*3)) & 0b00100000);
                svInfo[i].usedInSolution = (bool)( rxRecordData->at( index + 4 + (i*3)) & 0b01000000);
                svInfo[i].usedInRTK = (bool)( rxRecordData->at( index + 4 + (i*3)) & 0b10000000);

                svInfo[i].trackedPcodeL1 = (bool)( rxRecordData->at( index + 5 + (i*3)) & 0b00000001);
                svInfo[i].trackedPcodeL2 = (bool)( rxRecordData->at( index + 5 + (i*3)) & 0b00000010);
                svInfo[i].elevation = 0;    // Not supported in this message
                svInfo[i].azimuth = 0;      // Not supported in this message
                svInfo[i].snrL1 = 0;        // Not supported in this message
                svInfo[i].snrL2 = 0;        // Not supported in this message
            }

            // Lets zero out the remaining SV info
            for(int i = svInfo[0].numSv; i < 24; i++){
                svInfo[i].numSv = 0;
                svInfo[i].prn = 0;

                svInfo[i].satelliteAboveHorizon = 0;
                svInfo[i].assignedToChannel = 0;
                svInfo[i].trackedL1 = 0;
                svInfo[i].trackedL2 = 0;
                svInfo[i].trackedAtBaseL1 = 0;
                svInfo[i].trackedAtBaseL2 = 0;
                svInfo[i].usedInSolution = 0;
                svInfo[i].usedInRTK = 0;

                svInfo[i].trackedPcodeL1 = 0;
                svInfo[i].trackedPcodeL2 = 0;

                svInfo[i].elevation = 0;
                svInfo[i].azimuth = 0;
                svInfo[i].snrL1 = 0;
                svInfo[i].snrL2 = 0;
            }
            emit newSVinfoAvailable();
            break;

        case(SVDetail):
            svInfo[0].numSv = rxRecordData->at(index + 2) & 0xFF;

            for(int i = 0; i < svInfo[0].numSv; i++){
                svInfo[i].numSv = svInfo[0].numSv;
                svInfo[i].prn = rxRecordData->at( index + 3 + (i*8) ) & 0xFF;

                svInfo[i].satelliteAboveHorizon = (bool)( rxRecordData->at( index + 4 + (i*8)) & 0b00000001);
                svInfo[i].assignedToChannel = (bool)( rxRecordData->at( index + 4 + (i*8)) & 0b00000010);
                svInfo[i].trackedL1 = (bool)( rxRecordData->at( index + 4 + (i*8)) & 0b00000100);
                svInfo[i].trackedL2 = (bool)( rxRecordData->at( index + 4 + (i*8)) & 0b00001000);
                svInfo[i].trackedAtBaseL1 = (bool)( rxRecordData->at( index + 4 + (i*8)) & 0b00010000);
                svInfo[i].trackedAtBaseL2 = (bool)( rxRecordData->at( index + 4 + (i*8)) & 0b00100000);
                svInfo[i].usedInSolution = (bool)( rxRecordData->at( index + 4 + (i*8)) & 0b01000000);
                svInfo[i].usedInRTK = (bool)( rxRecordData->at( index + 4 + (i*8)) & 0b10000000);

                svInfo[i].trackedPcodeL1 = (bool)( rxRecordData->at( index + 5 + (i*8)) & 0b00000001);
                svInfo[i].trackedPcodeL2 = (bool)( rxRecordData->at( index + 5 + (i*8)) & 0b00000010);

                svInfo[i].elevation = rxRecordData->at( index + 6 + (i*8)) & 0xFF;
                svInfo[i].azimuth = ((rxRecordData->at( index + 7 + (i*8)) << 8) & 0xFF) + (rxRecordData->at( index + 8 + (i*8)) & 0xFF);
                svInfo[i].snrL1 = ((float)(rxRecordData->at( index + 9 + (i*8)) & 0xFF)) / 4.0f;
                svInfo[i].snrL2 = ((float)(rxRecordData->at( index + 10 + (i*8)) & 0xFF)) / 4.0f;
            }

            // Lets zero out the remaining SV info
            for(int i = svInfo[0].numSv; i < 24; i++){
                svInfo[i].numSv = 0;
                svInfo[i].prn = 0;

                svInfo[i].satelliteAboveHorizon = 0;
                svInfo[i].assignedToChannel = 0;
                svInfo[i].trackedL1 = 0;
                svInfo[i].trackedL2 = 0;
                svInfo[i].trackedAtBaseL1 = 0;
                svInfo[i].trackedAtBaseL2 = 0;
                svInfo[i].usedInSolution = 0;
                svInfo[i].usedInRTK = 0;

                svInfo[i].trackedPcodeL1 = 0;
                svInfo[i].trackedPcodeL2 = 0;

                svInfo[i].elevation = 0;
                svInfo[i].azimuth = 0;
                svInfo[i].snrL1 = 0;
                svInfo[i].snrL2 = 0;
            }
            emit newSVinfoAvailable();
            break;

        case(PositionTimeUTC):
            positionTimeUTC.millisecondsOfWeek =
                        ((rxRecordData->at(index + 2) & 0xFF) << 24) +
                        ((rxRecordData->at(index + 3) & 0xFF) << 16) +
                        ((rxRecordData->at(index + 4) & 0xFF) << 8) +
                        ((rxRecordData->at(index + 5) & 0xFF)<< 0);

            positionTimeUTC.gpsWeekNumber =
                    ((int)rxRecordData->at(index + 6) << 8) +
                    ((int)rxRecordData->at(index + 7) << 0);

            positionTimeUTC.numOfSV = rxRecordData->at(index + 8);
            positionTimeUTC.newPosition = (bool)(rxRecordData->at(index + 8) & 0b00000001);
            positionTimeUTC.newClockThisSolution = (bool)(rxRecordData->at(index + 8) & 0b00000010);
            positionTimeUTC.newHorizontalThisSolution = (bool)(rxRecordData->at(index + 8) & 0b00000100);
            positionTimeUTC.newHeightThisSolution = (bool)(rxRecordData->at(index + 8) & 0b00010000);
            positionTimeUTC.usesLeastSquaresPosition = (bool)(rxRecordData->at(index + 8) & 0b00100000);
            positionTimeUTC.usesFilteredL1 = (bool)(rxRecordData->at(index + 8) & 0b10000000);

            positionTimeUTC.isDifferential = (bool)(rxRecordData->at(index + 9) & 0b00000001);
            positionTimeUTC.isPhase = (bool)(rxRecordData->at(index + 9) & 0b00000010);
            positionTimeUTC.isFixedInteger = (bool)(rxRecordData->at(index + 9) & 0b00000100);
            positionTimeUTC.isOmnistar = (bool)(rxRecordData->at(index + 9) & 0b00001000);
            positionTimeUTC.isStatic = (bool)(rxRecordData->at(index + 9) & 0b00010000);
            positionTimeUTC.isNetworkRTK = (bool)(rxRecordData->at(index + 9) & 0b00100000);

            positionTimeUTC.initCounter = rxRecordData->at(index + 10) & 0xFF;

            emit newTimeAvailable();
            break;

        case(PositionSigma):
            tempFloat.bDat[0] = rxRecordData->at(index + 5) & 0xFF;
            tempFloat.bDat[1] = rxRecordData->at(index + 4) & 0xFF;
            tempFloat.bDat[2] = rxRecordData->at(index + 3) & 0xFF;
            tempFloat.bDat[3] = rxRecordData->at(index + 2) & 0xFF;
            sigma.rms = tempFloat.fData;

            tempFloat.bDat[0] = rxRecordData->at(index + 9) & 0xFF;
            tempFloat.bDat[1] = rxRecordData->at(index + 8) & 0xFF;
            tempFloat.bDat[2] = rxRecordData->at(index + 7) & 0xFF;
            tempFloat.bDat[3] = rxRecordData->at(index + 6) & 0xFF;
            sigma.sigmaEast = tempFloat.fData;

            tempFloat.bDat[0] = rxRecordData->at(index + 13) & 0xFF;
            tempFloat.bDat[1] = rxRecordData->at(index + 12) & 0xFF;
            tempFloat.bDat[2] = rxRecordData->at(index + 11) & 0xFF;
            tempFloat.bDat[3] = rxRecordData->at(index + 10) & 0xFF;
            sigma.sigmaNorth = tempFloat.fData;

            tempFloat.bDat[0] = rxRecordData->at(index + 17) & 0xFF;
            tempFloat.bDat[1] = rxRecordData->at(index + 16) & 0xFF;
            tempFloat.bDat[2] = rxRecordData->at(index + 15) & 0xFF;
            tempFloat.bDat[3] = rxRecordData->at(index + 14) & 0xFF;
            sigma.sigmaUp = tempFloat.fData;

            tempFloat.bDat[0] = rxRecordData->at(index + 21) & 0xFF;
            tempFloat.bDat[1] = rxRecordData->at(index + 20) & 0xFF;
            tempFloat.bDat[2] = rxRecordData->at(index + 19) & 0xFF;
            tempFloat.bDat[3] = rxRecordData->at(index + 18) & 0xFF;
            sigma.majorAxis = tempFloat.fData;

            tempFloat.bDat[0] = rxRecordData->at(index + 25) & 0xFF;
            tempFloat.bDat[1] = rxRecordData->at(index + 24) & 0xFF;
            tempFloat.bDat[2] = rxRecordData->at(index + 23) & 0xFF;
            tempFloat.bDat[3] = rxRecordData->at(index + 22) & 0xFF;
            sigma.minorAxis = tempFloat.fData;

            tempFloat.bDat[0] = rxRecordData->at(index + 29) & 0xFF;
            tempFloat.bDat[1] = rxRecordData->at(index + 28) & 0xFF;
            tempFloat.bDat[2] = rxRecordData->at(index + 27) & 0xFF;
            tempFloat.bDat[3] = rxRecordData->at(index + 26) & 0xFF;
            sigma.orientation = tempFloat.fData;

            tempFloat.bDat[0] = rxRecordData->at(index + 33) & 0xFF;
            tempFloat.bDat[1] = rxRecordData->at(index + 32) & 0xFF;
            tempFloat.bDat[2] = rxRecordData->at(index + 31) & 0xFF;
            tempFloat.bDat[3] = rxRecordData->at(index + 30) & 0xFF;
            sigma.unitVariance = tempFloat.fData;

            sigma.numEpochs =   ((int)(rxRecordData->at(index + 34) & 0xFF) << 8) +
                                ((int)(rxRecordData->at(index + 35) & 0xFF) << 0);

            emit newSigmaAvailable();
            break;

        case(Velocity):
            velocity.velocityValid = (bool)(rxRecordData->at(index + 2) & 0b00000001);
            velocity.computeMethod = (bool)(rxRecordData->at(index + 2) & 0b00000001);

            tempFloat.bDat[0] = rxRecordData->at(index + 6) & 0xFF;
            tempFloat.bDat[1] = rxRecordData->at(index + 5) & 0xFF;
            tempFloat.bDat[2] = rxRecordData->at(index + 4) & 0xFF;
            tempFloat.bDat[3] = rxRecordData->at(index + 3) & 0xFF;
            velocity.horizontalVelocity = tempFloat.fData;

            tempFloat.bDat[0] = rxRecordData->at(index + 10) & 0xFF;
            tempFloat.bDat[1] = rxRecordData->at(index +  9) & 0xFF;
            tempFloat.bDat[2] = rxRecordData->at(index +  8) & 0xFF;
            tempFloat.bDat[3] = rxRecordData->at(index +  7) & 0xFF;
            velocity.heading = tempFloat.fData * 57.2957795f; // TODO: Change to constant

            tempFloat.bDat[0] = rxRecordData->at(index + 14) & 0xFF;
            tempFloat.bDat[1] = rxRecordData->at(index + 13) & 0xFF;
            tempFloat.bDat[2] = rxRecordData->at(index + 12) & 0xFF;
            tempFloat.bDat[3] = rxRecordData->at(index + 11) & 0xFF;
            velocity.verticalVelocity = tempFloat.fData;

            emit newVelocityAvailable();
            break;

        }

        // Next record starts at the previous plus the type and header bytes
        index += recordLength + 2;
    }
}
