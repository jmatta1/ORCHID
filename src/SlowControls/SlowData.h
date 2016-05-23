/***************************************************************************//**
********************************************************************************
**
** @file SlowData.h
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
** @details Holds the definition of the struct that stores the data garnished
** from the slow controls like voltage and temperature
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHID_SRC_SLOWCONTROLS_SLOWDATA_H
#define ORCHID_SRC_SLOWCONTROLS_SLOWDATA_H

// includes for C system headers
// includes for C++ system headers
#include<atomic>
using std::atomic;
// includes from other libraries
// includes from ORCHID
namespace SlowControls
{
struct SlowData
{
    SlowData(int numVolChan, int numTempChan);
    ~SlowData();
    void readVoltageData(const VoltageData& data);

    //all stored data in this class is std::atomic for thread safety

    //MPOD Information
    //Per Channel information
    std::atomic<float>* terminalVoltage;
    std::atomic<float>* senseVoltage;
    std::atomic<float>* setVoltage;
    std::atomic<float>* current;
    std::atomic<float>* rampUpRate;
    std::atomic<float>* rampDownRate;
    std::atomic<float>* maxCurrent;
    std::atomic<float>* maxVoltage;
    std::atomic<int>*   currentTripTime;
    std::atomic<int>*   temperature;
    std::atomic<int>*   maxTemperature;
    std::atomic<bool>*  outputSwitch;
    std::atomic<bool>*  outputOn;
    std::atomic<bool>*  outputInhibit;
    std::atomic<bool>*  outputFailureMinSenseVoltage;
    std::atomic<bool>*  outputFailureMaxSenseVoltage;
    std::atomic<bool>*  outputFailureMaxTerminalVoltage;
    std::atomic<bool>*  outputFailureMaxCurrent;
    std::atomic<bool>*  outputFailureMaxTemperature;
    std::atomic<bool>*  outputFailureMaxPower;
    std::atomic<bool>*  outputFailureTimeout;
    std::atomic<bool>*  outputCurrentLimited;
    std::atomic<bool>*  outputRampUp;
    std::atomic<bool>*  outputRampDown;
    std::atomic<bool>*  outputEnableKill;
    std::atomic<bool>*  outputEmergencyOff;
    std::atomic<bool>*  outputAdjusting;
    std::atomic<bool>*  outputConstantVoltage;
    std::atomic<bool>*  outputCurrentBoundsExceeded;
    std::atomic<bool>*  outputFailureCurrentLimit;
    //Crate Information
    std::atomic<bool>  crateMainOn;
    std::atomic<bool>  crateMainInhibit;
    std::atomic<bool>  crateLocalControlOnly;
    std::atomic<bool>  crateInputFailure;
    std::atomic<bool>  crateOutputFailure;
    std::atomic<bool>  crateFantrayFailure;
    std::atomic<bool>  crateSensorFailure;
    std::atomic<bool>  crateVmeSysFailure;
    std::atomic<bool>  cratePlugAndPlayIncompatible;
    std::atomic<bool>  crateBusReset;
    std::atomic<bool>  crateSupplyDerating;
    std::atomic<bool>  crateSupplyFailure;

    //Temperature Sensor Information

    //general information
    std::atomic<int> numVoltageChannels;
    std::atomic<int> numTemperatureChannels;
};

}
#endif //ORCHID_SRC_SLOWCONTROLS_SLOWDATA_H
