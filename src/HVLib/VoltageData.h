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
#include<vector>
// includes from other libraries
// includes from ORCHID
#include "InputLib/Parsers/Utility.h"

namespace CrateMasks 
{
static const unsigned int MaskMainOn=0x8000,        MaskMainInhibit=0x4000,    MaskLocalControlOnly=0x2000;
static const unsigned int MaskInputFailure=0x1000,  MaskOutputFailure=0x0800,  MaskFantrayFailue=0x4000;
static const unsigned int MaskSensorFailure=0x0200, MaskVmeSysFailure=0x0100,  MaskPlugAndPlayIncompatible=0x0080;
static const unsigned int MaskBusReset=0x0040,      MaskSupplyDerating=0x0020, MaskSupplyFailure=0x0010;
}

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
    bool vmeSysFailure           :1;//bit7
    bool plugAndPlayIncompatible :1;//bit8
    bool busReset                :1;//bit9
    bool supplyDerating          :1;//bit10
    bool supplyFailure           :1;//bit11     (LSB)
    
    std::string getStatusString();
    void loadFromValue(unsigned int value);
};

namespace ChannelMasks 
{
static const unsigned int MaskOutputOn=0x800000,                         MaskOutputInhibit=0x400000;
static const unsigned int MaskOutputFailureMinSenseVoltage=0x200000,     MaskOutputFailureMaxSenseVoltage=0x100000;
static const unsigned int MaskOutputFailureMaxTerminalVoltage=0x080000,  MaskOutputFailureMaxCurrent=0x040000;
static const unsigned int MaskOutputFailureMaxTemperature=0x020000,      MaskOutputFailureMaxPower=0x010000;
static const unsigned int MaskOutputFailureTimeout=0x008000,             MaskOutputCurrentLimited=0x004000;
static const unsigned int MaskOutputRampUp=0x002000,                     MaskOutputRampDown=0x001000;
static const unsigned int MaskOutputEnableKill=0x000800,                 MaskOutputEmergencyOff=0x000400;
static const unsigned int MaskOutputAdjusting=0x000400,                  MaskOutputConstantVoltage=0x000200;
static const unsigned int MaskOutputCurrentBoundsExceeded=0x000100,      MaskOutputFailureCurrentLimit=0x000010;
}

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
    void loadFromValue(unsigned int value);
};

class VoltageData
{
public:
    VoltageData(int channels);
    ~VoltageData();
    
    std::vector<float>         terminalVoltage;
    std::vector<float>         senseVoltage;
    std::vector<float>         setVoltage;
    std::vector<float>         current;
    std::vector<float>         rampUpRate;
    std::vector<float>         rampDownRate;
    std::vector<float>         maxCurrent;
    std::vector<float>         maxVoltage;
    std::vector<int>           currentTripTime;
    std::vector<int>           temperature;
    std::vector<int>           maxTemperature;
    std::vector<bool>          outputSwitch;
    std::vector<ChannelStatus> channelStatus;
    CrateStatus                crateStatus;
    
    //measured voltages
    void loadTerminalVoltages(const std::string& input) {   parseMultiLineFloat(input, this->terminalVoltage, 1.0);}
    void loadSenseVoltages   (const std::string& input) {   parseMultiLineFloat(input, this->senseVoltage,    1.0);}
    //set voltages and max voltages
    void loadSetVoltages     (const std::string& input) {   parseMultiLineFloat(input, this->setVoltage,      1.0);}
    void loadMaxVoltages     (const std::string& input) {   parseMultiLineFloat(input, this->maxVoltage,      1.0);}
    //ramp rates
    void loadRampUpRates     (const std::string& input) {   parseMultiLineFloat(input, this->rampUpRate,      1.0);}
    void loadRampDownRates   (const std::string& input) {   parseMultiLineFloat(input, this->rampDownRate,    1.0);}
    //currents are multiplied by 1,000,000 since they are read in A and we want them in uA
    void loadCurrents        (const std::string& input) {   parseMultiLineFloat(input, this->current,         1000000.0);}
    void loadMaxCurrents     (const std::string& input) {   parseMultiLineFloat(input, this->maxCurrent,      1000000.0);}
    //current temperature and max temperature
    void loadTemperatures    (const std::string& input) {   parseMultiLineInt(input, this->temperature,       1.0);}
    void loadMaxTemperatures (const std::string& input) {   parseMultiLineInt(input, this->maxTemperature,    1.0);}
    //time that we can be over current without tripping
    void loadCurrentTripTimes(const std::string& input) {   parseMultiLineInt(input, this->currentTripTime,   1.0);}
    //switches controlling terminal output
    void loadOutputSwitches   (const std::string& input);
    //bit fields with various status flags for each channel
    void loadChannelStatuses (const std::string& input);
    //bit fields with various status flags for the crate
    void loadCrateStatus     (const std::string& input);

private:
    void parseMultiLineInt(const std::string& input, std::vector<int>& container, int mult);
    void parseMultiLineFloat(const std::string& input, std::vector<float>& container, float mult);
    
    int parseIntegerLine(const std::string& line);
    float parseFloatLine(const std::string& line);
    unsigned int parseBitsLine(const std::string& line, int nibbleCount);

    int numChannels;
    //parser for handling strings in the various load systems
    InputParser::Utility::StrWithSpaces<std::string::iterator> string_;

};

#endif //ORCHID_SRC_HVLIB_VOLTAGEDATA_H
