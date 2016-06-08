/***************************************************************************//**
********************************************************************************
**
** @file SlowControlEvent.h
** @author James Till Matta
** @date 07 June, 2016
** @brief
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** @details Holds the definition of the class that implements events from slow
** controls
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHID_SRC_EVENTS_SLOWCONTROLEVENT_H
#define ORCHID_SRC_EVENTS_SLOWCONTROLEVENT_H
// includes for C system headers
// includes for C++ system headers
// includes from other libraries
// includes from ORCHID
#include"EventInterface.h"
#include"SlowControls/HVLib/VoltageData.h"

namespace Events
{

class SlowControlsEvent : public EventInterface
{
public:
    SlowControlsEvent(int numVolChannels, int numTempChannels);
    virtual ~SlowControlsEvent();
    
    //functions to retrieve the binary representation
    virtual int getSizeOfBinaryRepresentation();
    //the buffer provided to this function will be at *least* the size
    //returned by getSizeOfBinaryRepresentation
    virtual void getBinaryRepresentation(char* buff);
    
    void ReadVoltageData(const SlowControls::VoltageData& voltageData);
    //void ReadTemperatureData()
private:
    
    //MPOD Information
    //Per Channel information
    float*         terminalVoltage;
    float*         senseVoltage;
    float*         setVoltage;
    float*         current;
    float*         rampUpRate;
    float*         rampDownRate;
    float*         maxCurrent;
    float*         maxVoltage;
    int*           currentTripTime;
    int*           temperature;
    int*           maxTemperature;
    bool*          outputSwitch;
    ChannelStatus* channelStatus;
    CrateStatus      crateStatus;

    //Temperature Sensor Information

    //general information these do not need to be atomic as they are written
    //once at object creation and never again
    int numVoltageChannels;
    int numTemperatureChannels;
};

}

#endif //ORCHID_SRC_EVENTS_SLOWCONTROLEVENT_H
