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
#include<sstream>
#include<iomanip>
// includes from other libraries
// includes from ORCHID
#include"EventCodes.h"

namespace Events
{

SlowControlsEvent::SlowControlsEvent(int numVolChannels, int numTempChannels):
    numVoltageChannels(numVolChannels), numTemperatureChannels(numTempChannels)
{
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
    //size of the event, event ident, num voltage channels, num temp channels
    int overhead = sizeof(int) + 2*sizeof(int) + sizeof(int);
    //8 floats + 3 ints + 1 bool per vol channel
    int sizeOfSingleVoltageChannel = 8*sizeof(float) + 3*sizeof(int) + sizeof(bool);
    //1 channel data per vol channel, it has 20 bits, so it fits into an int
    sizeOfSingleVoltageChannel += 8*sizeof(int);
    //there are numVoltageChannels worth of this to store, plus the crate status
    //which also fits into an int
    int sizeOfVoltage = this->numVoltageChannels*sizeOfSingleVoltageChannel + sizeof(int);
    //nada at the moment
    int sizeOfSingleTemperatureChannel = 0;
    int sizeOfTemperature = this->numTemperatureChannels*sizeOfSingleTemperatureChannel;
    
    return (overhead + sizeOfVoltage + sizeOfTemperature);
    
}

void SlowControlsEvent::getBinaryRepresentation(char* buff)
{
    //here we write, in binary the data in the event to the given buffer
    //lots of pointer tricks to come
    size_t index = 0;
    //first write the size of the event
    *(reinterpret_cast<int*>(&(buff[index]))) = this->getSizeOfBinaryRepresentation();
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
        *(reinterpret_cast<int*>(&(buff[index]))) = this->currentTripTime[i];
        index += sizeof(int);
        *(reinterpret_cast<int*>(&(buff[index]))) = this->temperature[i];
        index += sizeof(int);
        *(reinterpret_cast<int*>(&(buff[index]))) = this->maxTemperature[i];
        index += sizeof(int);
        *(reinterpret_cast<bool*>(&(buff[index]))) = this->outputSwitch[i];
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
            this->terminalVoltage[j] = data.terminalVoltage[i];
            this->senseVoltage[j] = data.senseVoltage[i];
            this->setVoltage[j] = data.setVoltage[i];
            this->current[j] = data.current[i];
            this->rampUpRate[j] = data.rampUpRate[i];
            this->rampDownRate[j] = data.rampDownRate[i];
            this->maxCurrent[j] = data.maxCurrent[i];
            this->maxVoltage[j] = data.maxVoltage[i];
            this->currentTripTime[j] = data.currentTripTime[i];
            this->temperature[j] = data.temperature[i];
            this->maxTemperature[j] = data.maxTemperature[i];
            this->outputSwitch[j] = data.outputSwitch[i];

            this->channelStatus[j] = data.channelStatus[i];
            ++j;
        }
        ++i;
    }
    //now transfer crate information
    this->crateStatus = data.crateStatus;
}

//void SlowControlsEvent::ReadTemperatureData(const SlowControls::TemperatureData& tempData)
}
