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
#include"Hardware/HVLib/VoltageData.h"

namespace InterThread
{

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

SlowData::~SlowData()
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

void SlowData::readVoltageData(const SlowControls::VoltageData& data)
{
    //set up the loop for transfering per channel information
    std::size_t i=0, j=0;
    std::size_t dataSize = data.terminalVoltage.size();
    //loop and transfer per channel information
    while((i < dataSize) && (j < numVoltageChannels))
    {
        if(data.outputSwitch[i])
        {
            this->terminalVoltage[j].store( data.terminalVoltage[i], std::memory_order_relaxed);
            this->senseVoltage[j].store(    data.senseVoltage[i], std::memory_order_relaxed);
            this->setVoltage[j].store(      data.setVoltage[i], std::memory_order_relaxed);
            this->current[j].store(         data.current[i], std::memory_order_relaxed);
            this->rampUpRate[j].store(      data.rampUpRate[i], std::memory_order_relaxed);
            this->rampDownRate[j].store(    data.rampDownRate[i], std::memory_order_relaxed);
            this->maxCurrent[j].store(      data.maxCurrent[i], std::memory_order_relaxed);
            this->maxVoltage[j].store(      data.maxVoltage[i], std::memory_order_relaxed);
            this->currentTripTime[j].store( data.currentTripTime[i], std::memory_order_relaxed);
            this->temperature[j].store(     data.temperature[i], std::memory_order_relaxed);
            this->maxTemperature[j].store(  data.maxTemperature[i], std::memory_order_relaxed);
            this->outputSwitch[j].store(    data.outputSwitch[i], std::memory_order_relaxed);

            this->outputOn[j].store(                        data.channelStatus[i].outputOn, std::memory_order_relaxed);
            this->outputInhibit[j].store(                   data.channelStatus[i].outputInhibit, std::memory_order_relaxed);
            this->outputFailureMinSenseVoltage[j].store(    data.channelStatus[i].outputFailureMinSenseVoltage, std::memory_order_relaxed);
            this->outputFailureMaxSenseVoltage[j].store(    data.channelStatus[i].outputFailureMaxSenseVoltage, std::memory_order_relaxed);
            this->outputFailureMaxTerminalVoltage[j].store( data.channelStatus[i].outputFailureMaxTerminalVoltage, std::memory_order_relaxed);
            this->outputFailureMaxCurrent[j].store(         data.channelStatus[i].outputFailureMaxCurrent, std::memory_order_relaxed);
            this->outputFailureMaxTemperature[j].store(     data.channelStatus[i].outputFailureMaxTemperature, std::memory_order_relaxed);
            this->outputFailureMaxPower[j].store(           data.channelStatus[i].outputFailureMaxPower, std::memory_order_relaxed);
            this->outputFailureTimeout[j].store(            data.channelStatus[i].outputFailureTimeout, std::memory_order_relaxed);
            this->outputCurrentLimited[j].store(            data.channelStatus[i].outputCurrentLimited, std::memory_order_relaxed);
            this->outputRampUp[j].store(                    data.channelStatus[i].outputRampUp, std::memory_order_relaxed);
            this->outputRampDown[j].store(                  data.channelStatus[i].outputRampDown, std::memory_order_relaxed);
            this->outputEnableKill[j].store(                data.channelStatus[i].outputEnableKill, std::memory_order_relaxed);
            this->outputEmergencyOff[j].store(              data.channelStatus[i].outputEmergencyOff, std::memory_order_relaxed);
            this->outputAdjusting[j].store(                 data.channelStatus[i].outputAdjusting, std::memory_order_relaxed);
            this->outputConstantVoltage[j].store(           data.channelStatus[i].outputConstantVoltage, std::memory_order_relaxed);
            this->outputCurrentBoundsExceeded[j].store(     data.channelStatus[i].outputCurrentBoundsExceeded, std::memory_order_relaxed);
            this->outputFailureCurrentLimit[j].store(       data.channelStatus[i].outputFailureCurrentLimit, std::memory_order_relaxed);
            ++j;
        }
        ++i;
    }
    //now transfer crate information
    this->crateMainOn.store(                    data.crateStatus.mainOn, std::memory_order_relaxed);
    this->crateMainInhibit.store(               data.crateStatus.mainInhibit, std::memory_order_relaxed);
    this->crateLocalControlOnly.store(          data.crateStatus.localControlOnly, std::memory_order_relaxed);
    this->crateInputFailure.store(              data.crateStatus.inputFailure, std::memory_order_relaxed);
    this->crateOutputFailure.store(             data.crateStatus.outputFailure, std::memory_order_relaxed);
    this->crateFantrayFailure.store(            data.crateStatus.fantrayFailure, std::memory_order_relaxed);
    this->crateSensorFailure.store(             data.crateStatus.sensorFailure, std::memory_order_relaxed);
    this->crateVmeSysFailure.store(             data.crateStatus.vmeSysFailure, std::memory_order_relaxed);
    this->cratePlugAndPlayIncompatible.store(   data.crateStatus.plugAndPlayIncompatible, std::memory_order_relaxed);
    this->crateBusReset.store(                  data.crateStatus.busReset, std::memory_order_relaxed);
    this->crateSupplyDerating.store(            data.crateStatus.supplyDerating, std::memory_order_relaxed);
    this->crateSupplyFailure.store(             data.crateStatus.supplyFailure, std::memory_order_relaxed);
}

void SlowData::genCrateInfoString(std::string& out)
{
    std::ostringstream output;
    if(this->crateMainOn)
    {
        output << "On ";
    }
    if(this->crateMainInhibit)
    {
        output << "Inhibit ";
    }
    if(this->crateLocalControlOnly)
    {
        output << "Lcl_Ctrl_Only ";
    }
    if(     this->crateInputFailure   || this->crateOutputFailure           || 
            this->crateFantrayFailure || this->crateSensorFailure           ||
            this->crateVmeSysFailure  || this->cratePlugAndPlayIncompatible ||
            this->crateSupplyDerating || this->crateSupplyFailure)
    {
        output << "Failure: ( ";
        if(this->crateInputFailure) output << "input ";
        if(this->crateOutputFailure) output << "output ";
        if(this->crateFantrayFailure) output << "fantray ";
        if(this->crateSensorFailure) output << "sensor ";
        if(this->crateVmeSysFailure) output << "VME_Sys ";
        if(this->cratePlugAndPlayIncompatible) output << "PnP_Incompat ";
        if(this->crateSupplyDerating) output << "PS_derating ";
        if(this->crateSupplyFailure) output << "PS_failure ";
        output << ") ";
        
    }
    if(this->crateBusReset)
    {
        output << "Bus_Reset ";
    }
    out = output.str();
}

void SlowData::genChannelInfoString(int channel, std::string& out)
{
    std::ostringstream output;
    if(outputOn[channel]) output << "On ";
    if(outputInhibit[channel]) output << "Inhibit ";

    if(outputFailureMinSenseVoltage[channel] || outputFailureMaxSenseVoltage[channel] ||
            outputFailureMaxTerminalVoltage[channel] || outputFailureMaxCurrent[channel] ||
            outputFailureMaxTemperature[channel] || outputFailureMaxPower[channel] ||
            outputFailureTimeout[channel] || outputEmergencyOff[channel] ||
            outputCurrentBoundsExceeded[channel] || outputFailureCurrentLimit[channel])
    {
        output << "Failure: ( ";
        if(outputFailureMinSenseVoltage[channel]) output << "MinSense ";
        if(outputFailureMaxSenseVoltage[channel]) output << "MaxSense ";
        if(outputFailureMaxTerminalVoltage[channel]) output << "MaxTerm ";
        if(outputFailureMaxCurrent[channel]) output << "MaxCurr ";
        if(outputFailureMaxTemperature[channel]) output << "MaxTemp ";
        if(outputFailureMaxPower[channel]) output << "MaxPower ";
        if(outputFailureTimeout[channel]) output << "FailTimout ";
        if(outputEmergencyOff[channel]) output << "EmerOff ";
        if(outputCurrentBoundsExceeded[channel]) output << "OutOfCurrBounds ";
        if(outputFailureCurrentLimit[channel]) output << "CurrLimFail ";
        output << ") ";
        
    }
    if(outputCurrentLimited[channel]) output << "OutCurLim ";
    if(outputRampUp[channel]) output << "RampUp ";
    if(outputRampDown[channel]) output << "RampDown ";
    if(!outputAdjusting[channel]) output << "No_FineAdj ";
    if(!outputConstantVoltage[channel]) output << "No_ConstVoltage ";
    out = output.str();
}

}
