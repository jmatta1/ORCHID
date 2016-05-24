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
#include"HVLib/VoltageData.h"


SlowData::SlowData(int numVolChan, int numTempChan):
    numVoltageChannels(numVolChan), numTemperatureChannels(numTempChan)
{
    //allocate everything
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
    //now initialize everything
    //first initialize the per channel information
    for(int i = 0; i < numVolChan; ++i)
    {
        this->terminalVoltage[i].store(0.0);
        this->senseVoltage[i].store(0.0);
        this->setVoltage[i].store(0.0);
        this->current[i].store(0.0);
        this->rampUpRate[i].store(0.0);
        this->rampDownRate[i].store(0.0);
        this->maxCurrent[i].store(0.0);
        this->maxVoltage[i].store(0.0);
        this->currentTripTime[i].store(0);
        this->temperature[i].store(0);
        this->maxTemperature[i].store(0);
        this->outputSwitch[i].store(false);
        this->outputOn[i].store(false);
        this->outputInhibit[i].store(false);
        this->outputFailureMinSenseVoltage[i].store(false);
        this->outputFailureMaxSenseVoltage[i].store(false);
        this->outputFailureMaxTerminalVoltage[i].store(false);
        this->outputFailureMaxCurrent[i].store(false);
        this->outputFailureMaxTemperature[i].store(false);
        this->outputFailureMaxPower[i].store(false);
        this->outputFailureTimeout[i].store(false);
        this->outputCurrentLimited[i].store(false);
        this->outputRampUp[i].store(false);
        this->outputRampDown[i].store(false);
        this->outputEnableKill[i].store(false);
        this->outputEmergencyOff[i].store(false);
        this->outputAdjusting[i].store(false);
        this->outputConstantVoltage[i].store(false);
        this->outputCurrentBoundsExceeded[i].store(false);
        this->outputFailureCurrentLimit[i].store(false);
    }
    //now initialize the crate information
    this->crateMainOn.store(false);
    this->crateMainInhibit.store(false);
    this->crateLocalControlOnly.store(false);
    this->crateInputFailure.store(false);
    this->crateOutputFailure.store(false);
    this->crateFantrayFailure.store(false);
    this->crateSensorFailure.store(false);
    this->crateVmeSysFailure.store(false);
    this->cratePlugAndPlayIncompatible.store(false);
    this->crateBusReset.store(false);
    this->crateSupplyDerating.store(false);
    this->crateSupplyFailure.store(false);
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

void SlowData::readVoltageData(const VoltageData& data)
{
    //set up the loop for transfering per channel information
    std::size_t i=0, j=0;
    std::size_t dataSize = data.terminalVoltage.size();
    //loop and transfer per channel information
    while((i < dataSize) && (j < numVoltageChannels))
    {
        if(data.outputSwitch[i])
        {
            this->terminalVoltage[j].store( data.terminalVoltage[i]);
            this->senseVoltage[j].store(    data.senseVoltage[i]);
            this->setVoltage[j].store(      data.setVoltage[i]);
            this->current[j].store(         data.current[i]);
            this->rampUpRate[j].store(      data.rampUpRate[i]);
            this->rampDownRate[j].store(    data.rampDownRate[i]);
            this->maxCurrent[j].store(      data.maxCurrent[i]);
            this->maxVoltage[j].store(      data.maxVoltage[i]);
            this->currentTripTime[j].store( data.currentTripTime[i]);
            this->temperature[j].store(     data.temperature[i]);
            this->maxTemperature[j].store(  data.maxTemperature[i]);
            this->outputSwitch[j].store(    data.outputSwitch[i]);

            this->outputOn[j].store(                        data.channelStatus[i].outputOn);
            this->outputInhibit[j].store(                   data.channelStatus[i].outputInhibit);
            this->outputFailureMinSenseVoltage[j].store(    data.channelStatus[i].outputFailureMinSenseVoltage);
            this->outputFailureMaxSenseVoltage[j].store(    data.channelStatus[i].outputFailureMaxSenseVoltage);
            this->outputFailureMaxTerminalVoltage[j].store( data.channelStatus[i].outputFailureMaxTerminalVoltage);
            this->outputFailureMaxCurrent[j].store(         data.channelStatus[i].outputFailureMaxCurrent);
            this->outputFailureMaxTemperature[j].store(     data.channelStatus[i].outputFailureMaxTemperature);
            this->outputFailureMaxPower[j].store(           data.channelStatus[i].outputFailureMaxPower);
            this->outputFailureTimeout[j].store(            data.channelStatus[i].outputFailureTimeout);
            this->outputCurrentLimited[j].store(            data.channelStatus[i].outputCurrentLimited);
            this->outputRampUp[j].store(                    data.channelStatus[i].outputRampUp);
            this->outputRampDown[j].store(                  data.channelStatus[i].outputRampDown);
            this->outputEnableKill[j].store(                data.channelStatus[i].outputEnableKill);
            this->outputEmergencyOff[j].store(              data.channelStatus[i].outputEmergencyOff);
            this->outputAdjusting[j].store(                 data.channelStatus[i].outputAdjusting);
            this->outputConstantVoltage[j].store(           data.channelStatus[i].outputConstantVoltage);
            this->outputCurrentBoundsExceeded[j].store(     data.channelStatus[i].outputCurrentBoundsExceeded);
            this->outputFailureCurrentLimit[j].store(       data.channelStatus[i].outputFailureCurrentLimit);
            ++j;
        }
        ++i;
    }
    //now transfer crate information
    this->crateMainOn.store(mainOn);
    this->crateMainInhibit.store(mainInhibit);
    this->crateLocalControlOnly.store(localControlOnly);
    this->crateInputFailure.store(inputFailure);
    this->crateOutputFailure.store(outputFailure);
    this->crateFantrayFailure.store(fantrayFailure);
    this->crateSensorFailure.store(sensorFailure);
    this->crateVmeSysFailure.store(vmeSysFailure);
    this->cratePlugAndPlayIncompatible.store(plugAndPlayIncompatible);
    this->crateBusReset.store(busReset);
    this->crateSupplyDerating.store(supplyDerating);
    this->crateSupplyFailure.store(supplyFailure);
}
