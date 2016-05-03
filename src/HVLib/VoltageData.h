/***************************************************************************//**
********************************************************************************
**
** @file VoltageData.h
** @author James Till Matta
** @date 03 May, 2016
** @brief
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** @details Definition file for ChannelStatus and VoltageData classes
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHID_SRC_HVLIB_VOLTAGEDATA_H
#define ORCHID_SRC_HVLIB_VOLTAGEDATA_H

// includes for C system headers
// includes for C++ system headers
#include<string>
// includes from other libraries
// includes from ORCHID

class CrateStatus
{
public:
    CrateStatus();
    bool mainOn                  :1;//bit0      (MSB)
    bool mainInhibit             :1;//bit1
    bool localControlOnly        :1;//bit2
    bool inputFailure            :1;//bit3
    bool outputFailure           :1;//bit4
    bool fantrayFailure          :1;//bit5
    bool sensorFailure           :1;//bit6
    bool vmeSysfail              :1;//bit7
    bool plugAndPlayIncompatible :1;//bit8
    bool busReset                :1;//bit9
    bool supplyDerating          :1;//bit10
    bool supplyFailure           :1;//bit11
    bool supplyDerating2         :1;//bit12
    bool supplyFailure2          :1;//bit13     (LSB)
    
    std::string getStatusString();
    void loadFromValue(int value);
};

class ChannelStatus
{
public:
    ChannelStatus();
    bool outputOn                        :1;//bit0      (MSB)
    bool outputInhibit                   :1;//bit1
    bool outputFailureMinSenseVoltage    :1;//bit2
    bool outputFailureMaxSenseVoltage    :1;//bit3
    bool outputFailureMaxTerminalVoltage :1;//bit4
    bool outputFailureMaxCurrent         :1;//bit5
    bool outputFailureMaxTemperature     :1;//bit6
    bool outputFailureMaxPower           :1;//bit7
    bool reserved1                       :1;//bit8
    bool outputFailureTimeout            :1;//bit9
    bool outputCurrentLimited            :1;//bit10
    bool outputRampUp                    :1;//bit11
    bool outputRampDown                  :1;//bit12
    bool outputEnableKill                :1;//bit13
    bool outputEmergencyOff              :1;//bit14
    bool outputAdjusting                 :1;//bit15
    bool outputConstantVoltage           :1;//bit16
    bool reserved2                       :1;//bit17
    bool outputCurrentBoundsExceeded     :1;//bit18
    bool outputFailureCurrentLimit       :1;//bit19     (LSB)

    std::string getStatusString();
    void loadFromValue(int value);
};

class VoltageData
{
public:
    VoltageData(int channels);
    ~VoltageData();
    
    std::vector<float>         terminalVoltage;
    std::vector<float>         senseVoltage;
    std::vector<float>         setVoltage;
    std::vector<int>           temperature;
    std::vector<float>         current;
    std::vector<bool>          outputSwitch;
    std::vector<float>         rampUpRate;
    std::vector<float>         rampDownRate;
    std::vector<int>           currentTripTime;
    std::vector<int>           maxTemperature;
    std::vector<float>         maxCurrent;
    std::vector<float>         maxVoltage;
    std::vector<ChannelStatus> channelStatus;
    CrateStatus                crateStatus;
    
    void loadTerminalVoltages(const std::string& input);
    void loadSenseVoltages   (const std::string& input);
    void loadSetVoltages     (const std::string& input);
    void loadTemperatures    (const std::string& input);
    void loadCurrents        (const std::string& input);
    void loadOutputSwitchs   (const std::string& input);
    void loadRampUpRates     (const std::string& input);
    void loadRampDownRates   (const std::string& input);
    void loadCurrentTripTimes(const std::string& input);
    void loadMaxTemperatures (const std::string& input);
    void loadMaxCurrents     (const std::string& input);
    void loadMaxVoltages     (const std::string& input);
    void loadChannelStatuses (const std::string& input);
    void loadCrateStatus     (const std::string& input);

private:
    int numChannels;
    //parser for handling strings in the various load systems
    InputParser::Utility::StrWithSpaces<std::string::iterator> string_;

};

#endif //ORCHID_SRC_HVLIB_VOLTAGEDATA_H
