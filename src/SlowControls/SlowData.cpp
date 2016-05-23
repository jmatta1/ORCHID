/***************************************************************************//**
********************************************************************************
**
** @file SlowData.cpp
** @author James Till Matta
** @date 27 Apr, 2016
** @brief
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** @details Holds the implementation of the struct that stores the data
** garnished from the slow controls like voltage and temperature
**
********************************************************************************
*******************************************************************************/

#include"SlowData.h"
// includes for C system headers
// includes for C++ system headers
// includes from other libraries
// includes from ORCHID
SlowData::SlowData(int numVolChan, int numTempChan)
{
    this->numVoltageChannels.store(numVolChan);
    this->numTemperatureChannels.store(numTempChan);
    this->terminalVoltage                 = new std::atomic<float>[numVoltageChannels];
    this->senseVoltage                    = new std::atomic<float>[numVoltageChannels];
    this->setVoltage                      = new std::atomic<float>[numVoltageChannels];
    this->current                         = new std::atomic<float>[numVoltageChannels];
    this->rampUpRate                      = new std::atomic<float>[numVoltageChannels];
    this->rampDownRate                    = new std::atomic<float>[numVoltageChannels];
    this->maxCurrent                      = new std::atomic<float>[numVoltageChannels];
    this->maxVoltage                      = new std::atomic<float>[numVoltageChannels];
    this->currentTripTime                 = new std::atomic<int>[numVoltageChannels];
    this->temperature                     = new std::atomic<int>[numVoltageChannels];
    this->maxTemperature                  = new std::atomic<int>[numVoltageChannels];
    this->outputSwitch                    = new std::atomic<bool>[numVoltageChannels];
    this->outputOn                        = new std::atomic<bool>[numVoltageChannels];
    this->outputInhibit                   = new std::atomic<bool>[numVoltageChannels];
    this->outputFailureMinSenseVoltage    = new std::atomic<bool>[numVoltageChannels];
    this->outputFailureMaxSenseVoltage    = new std::atomic<bool>[numVoltageChannels];
    this->outputFailureMaxTerminalVoltage = new std::atomic<bool>[numVoltageChannels];
    this->outputFailureMaxCurrent         = new std::atomic<bool>[numVoltageChannels];
    this->outputFailureMaxTemperature     = new std::atomic<bool>[numVoltageChannels];
    this->outputFailureMaxPower           = new std::atomic<bool>[numVoltageChannels];
    this->outputFailureTimeout            = new std::atomic<bool>[numVoltageChannels];
    this->outputCurrentLimited            = new std::atomic<bool>[numVoltageChannels];
    this->outputRampUp                    = new std::atomic<bool>[numVoltageChannels];
    this->outputRampDown                  = new std::atomic<bool>[numVoltageChannels];
    this->outputEnableKill                = new std::atomic<bool>[numVoltageChannels];
    this->outputEmergencyOff              = new std::atomic<bool>[numVoltageChannels];
    this->outputAdjusting                 = new std::atomic<bool>[numVoltageChannels];
    this->outputConstantVoltage           = new std::atomic<bool>[numVoltageChannels];
    this->outputCurrentBoundsExceeded     = new std::atomic<bool>[numVoltageChannels];
    this->outputFailureCurrentLimit       = new std::atomic<bool>[numVoltageChannels];
}

SlowData::~SlowData(int numVolChan, int numTempChan)
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
    delete[] this->outputOn;
    delete[] this->outputInhibit;
    delete[] this->outputFailureMinSenseVoltage;
    delete[] this->outputFailureMaxSenseVoltage;
    delete[] this->outputFailureMaxTerminalVoltage;
    delete[] this->outputFailureMaxCurrent;
    delete[] this->outputFailureMaxTemperature;
    delete[] this->outputFailureMaxPower;
    delete[] this->outputFailureTimeout;
    delete[] this->outputCurrentLimited;
    delete[] this->outputRampUp;
    delete[] this->outputRampDown;
    delete[] this->outputEnableKill;
    delete[] this->outputEmergencyOff;
    delete[] this->outputAdjusting;
    delete[] this->outputConstantVoltage;
    delete[] this->outputCurrentBoundsExceeded;
    delete[] this->outputFailureCurrentLimit;
}

SlowData::readVoltageData(const VoltageData& data)
{

}
