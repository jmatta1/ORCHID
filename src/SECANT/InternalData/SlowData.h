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
#include<string>
// includes from other libraries
// includes from ORCHID
#include"Hardware/HVLib/VoltageData.h"

namespace InterThread
{
struct SlowData
{
    SlowData(int numVolChan, int numTempChan);
    ~SlowData();
    //function to read the mpod slow control info from the voltage data struct
    void readVoltageData(const SlowControls::VoltageData& data);
    //function to read the temperature slow control data from the temp struct
    //void readTemperatureData(const SlowControls::TemperatureData& data);
    
    //function to generate a descriptive string from the crate information
    void genCrateInfoString(std::string& out);
    
    //function to generate a descriptive string from the channel information
    void genChannelInfoString(int channel, std::string& out);
    
    
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

    //general information these do not need to be atomic as they are written
    //once at object creation and never again
    int numVoltageChannels;
    int numTemperatureChannels;
};

}
#endif //ORCHID_SRC_SLOWCONTROLS_SLOWDATA_H
