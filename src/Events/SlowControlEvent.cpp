/***************************************************************************//**
********************************************************************************
**
** @file SlowControlEvent.cpp
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
** @details Holds the definition of the interface superclass that all events
** being passed to the file IO thread must implement
**
********************************************************************************
*******************************************************************************/

#include"SlowControlEvent.h"
// includes for C system headers
// includes for C++ system headers
// includes from other libraries
// includes from ORCHID
#include"EventCodes.h"

namespace Events
{

SlowControlsEvent::SlowControlsEvent(int numVolChannels, int numTempChannels):
    numVoltageChannels(numVolChannels), numTemperatureChannels(numTempChannels)
{
    terminalVoltage = new float[numVolChannels];
    senseVoltage    = new float[numVolChannels];
    setVoltage      = new float[numVolChannels];
    current         = new float[numVolChannels];
    rampUpRate      = new float[numVolChannels];
    rampDownRate    = new float[numVolChannels];
    maxCurrent      = new float[numVolChannels];
    maxVoltage      = new float[numVolChannels];
    currentTripTime = new int[numVolChannels];
    temperature     = new int[numVolChannels];
    maxTemperature  = new int[numVolChannels];
    outputSwitch    = new bool[numVolChannels];
    channelStatus   = new ChannelStatus[numVolChannels];
}

SlowControlsEvent::~SlowControlsEvent()
{
    delete[] terminalVoltage;
    delete[] senseVoltage;
    delete[] setVoltage;
    delete[] current;
    delete[] rampUpRate;
    delete[] rampDownRate;
    delete[] maxCurrent;
    delete[] maxVoltage;
    delete[] currentTripTime;
    delete[] temperature;
    delete[] maxTemperature;
    delete[] outputSwitch;
    delete[] channelStatus;
}


int SlowControlsEvent::getSizeOfBinaryRepresentation()
{
    //event ident, num voltage channels, num temp channels
    int overhead = sizeof(SlowControlsEventCode) + 2*sizeof(int);
    //8 floats + 3 ints + 1 bool per vol channel
    int sizeOfSingleVoltageChannel = 8*sizeof(float) + 3*sizeof(int) + sizeof(bool);
    //1 channel data per vol channel, it has 20 bits, so it fits into an int
    sizeOfSingleVoltageChannel += 8*sizeof(int);
    //there are numVoltageChannels worth of this to store, plus the crate status
    //which also fits into an int
    int sizeOfVoltage = numVoltageChannels*sizeOfSingleVoltageChannel + sizeof(int);
    //nada at the moment
    int sizeOfSingleTemperatureChannel = 0;
    int sizeOfTemperature = numTemperatureChannels*sizeOfSingleTemperatureChannel;
    
    return (overhead + sizeOfVoltage + sizeOfTemperature);
    
}

void SlowControlsEvent::getBinaryRepresentation(char* buff)
{
    
}


int SlowControlsEvent::getSizeOfAsciiRepresentation()
{
    
}

void SlowControlsEvent::getAsciiRepresentation(char* buff)
{
    
}

void SlowControlsEvent::ReadVoltageData(const SlowControls::VoltageData& voltageData)
{
    
}

//void SlowControlsEvent::ReadTemperatureData()
}
