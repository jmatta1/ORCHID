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

#include"SlowControlsEvent.h"
// includes for C system headers
// includes for C++ system headers
#include<sstream>
#include<iomanip>
// includes from other libraries
// includes from ORCHID
#include"EventCodes.h"

namespace Events
{

SlowControlsEvent::SlowControlsEvent(int numVolChannels, int numTempChannels):
    numVoltageChannels(numVolChannels), numTemperatureChannels(numTempChannels),
    binarySize(0)
{
    this->boardNumber     = new int[numVolChannels];
    this->channelNumber   = new int[numVolChannels];
    this->terminalVoltage = new float[numVolChannels];
    this->senseVoltage    = new float[numVolChannels];
    this->setVoltage      = new float[numVolChannels];
    this->current         = new float[numVolChannels];
    this->rampUpRate      = new float[numVolChannels];
    this->rampDownRate    = new float[numVolChannels];
    this->maxCurrent      = new float[numVolChannels];
    this->maxVoltage      = new float[numVolChannels];
    this->currentTripTime = new int[numVolChannels];
    this->temperature     = new int[numVolChannels];
    this->maxTemperature  = new int[numVolChannels];
    this->outputSwitch    = new bool[numVolChannels];
    this->channelStatus   = new SlowControls::ChannelStatus[numVolChannels];
    //calculate the size of an event
    //size of the event, event ident, num voltage channels, num temp channels
    int overhead = 4*sizeof(int);
    //2 ints + 8 floats + 3 ints + 1 bool + 1 unsigned int per vol channel
    int sizeOfSingleVoltageChannel = 2*sizeof(int) + 8*sizeof(float) +
                                     3*sizeof(int) + sizeof(bool) +
                                     sizeof(unsigned int);
    //there are numVoltageChannels worth of this to store, plus the crate status
    //which fits into an int
    int sizeOfVoltage = sizeof(unsigned int) +
                        (this->numVoltageChannels*sizeOfSingleVoltageChannel);
    //no temperature data at the moment
    int sizeOfSingleTemperatureChannel = 0;
    int sizeOfTemperature = this->numTemperatureChannels*sizeOfSingleTemperatureChannel;
    binarySize = (overhead + sizeOfVoltage + sizeOfTemperature);
}

SlowControlsEvent::~SlowControlsEvent()
{
    delete[] this->terminalVoltage;
    delete[] this->senseVoltage;
    delete[] this->setVoltage;
    delete[] this->current;
    delete[] this->rampUpRate;
    delete[] this->rampDownRate;
    delete[] this->maxCurrent;
    delete[] this->maxVoltage;
    delete[] this->currentTripTime;
    delete[] this->temperature;
    delete[] this->maxTemperature;
    delete[] this->outputSwitch;
    delete[] this->channelStatus;
}


int SlowControlsEvent::getSizeOfBinaryRepresentation()
{
    return binarySize;
    
}

void SlowControlsEvent::getBinaryRepresentation(char* buff)
{
    //here we write, in binary the data in the event to the given buffer
    //lots of pointer tricks to come
    size_t index = 0;
    //first write the size of the event
    *(reinterpret_cast<int*>(&(buff[index]))) = this->binarySize;
    index += sizeof(int);
    *(reinterpret_cast<int*>(&(buff[index]))) = Codes::SlowControlsEventCode;
    index += sizeof(int);
    *(reinterpret_cast<int*>(&(buff[index]))) = this->numVoltageChannels;
    index += sizeof(int);
    *(reinterpret_cast<int*>(&(buff[index]))) = this->numTemperatureChannels;
    index += sizeof(int);
    //now write the power crate information
    *(reinterpret_cast<unsigned int*>(&(buff[index]))) = this->crateStatus.giveIntRepresentation();
    index += sizeof(unsigned int);
    //now write the rest of the power information channel by channel
    for(int i =0; i<numVoltageChannels; ++i)
    {
        *(reinterpret_cast<int*>(&(buff[index])))   = this->boardNumber[i];
        index += sizeof(int);
        *(reinterpret_cast<int*>(&(buff[index])))   = this->channelNumber[i];
        index += sizeof(int);
        *(reinterpret_cast<float*>(&(buff[index]))) = this->terminalVoltage[i];
        index += sizeof(float);
        *(reinterpret_cast<float*>(&(buff[index]))) = this->senseVoltage[i];
        index += sizeof(float);
        *(reinterpret_cast<float*>(&(buff[index]))) = this->setVoltage[i];
        index += sizeof(float);
        *(reinterpret_cast<float*>(&(buff[index]))) = this->current[i];
        index += sizeof(float);
        *(reinterpret_cast<float*>(&(buff[index]))) = this->rampUpRate[i];
        index += sizeof(float);
        *(reinterpret_cast<float*>(&(buff[index]))) = this->rampDownRate[i];
        index += sizeof(float);
        *(reinterpret_cast<float*>(&(buff[index]))) = this->maxCurrent[i];
        index += sizeof(float);
        *(reinterpret_cast<float*>(&(buff[index]))) = this->maxVoltage[i];
        index += sizeof(float);
        *(reinterpret_cast<int*>(&(buff[index])))   = this->currentTripTime[i];
        index += sizeof(int);
        *(reinterpret_cast<int*>(&(buff[index])))   = this->temperature[i];
        index += sizeof(int);
        *(reinterpret_cast<int*>(&(buff[index])))   = this->maxTemperature[i];
        index += sizeof(int);
        *(reinterpret_cast<bool*>(&(buff[index])))  = this->outputSwitch[i];
        index += sizeof(bool);
        *(reinterpret_cast<unsigned int*>(&(buff[index]))) = this->channelStatus[i].giveIntRepresentation();
        index += sizeof(unsigned int);
    }
    //now write the the temperature information
}


void SlowControlsEvent::ReadVoltageData(const SlowControls::VoltageData& data)
{
    //set up the loop for transfering per channel information
    std::size_t i=0, j=0;
    std::size_t dataSize = data.terminalVoltage.size();
    //loop and transfer per channel information
    while((i < dataSize) && (j < numVoltageChannels))
    {
        if(data.outputSwitch[i])
        {
            this->boardNumber[j]        = data.boardNumber[i];
            this->channelNumber[j]      = data.channelNumber[i];
            this->terminalVoltage[j]    = data.terminalVoltage[i];
            this->senseVoltage[j]       = data.senseVoltage[i];
            this->setVoltage[j]         = data.setVoltage[i];
            this->current[j]            = data.current[i];
            this->rampUpRate[j]         = data.rampUpRate[i];
            this->rampDownRate[j]       = data.rampDownRate[i];
            this->maxCurrent[j]         = data.maxCurrent[i];
            this->maxVoltage[j]         = data.maxVoltage[i];
            this->currentTripTime[j]    = data.currentTripTime[i];
            this->temperature[j]        = data.temperature[i];
            this->maxTemperature[j]     = data.maxTemperature[i];
            this->outputSwitch[j]       = data.outputSwitch[i];
            this->channelStatus[j]      = data.channelStatus[i];
            ++j;
        }
        ++i;
    }
    //now transfer crate information
    this->crateStatus = data.crateStatus;
}

//void SlowControlsEvent::ReadTemperatureData(const SlowControls::TemperatureData& tempData)
}
