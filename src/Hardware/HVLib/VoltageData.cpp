/***************************************************************************//**
********************************************************************************
**
** @file VoltageData.cpp
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
** @details Implementation file for ChannelStatus and VoltageData classes
**
********************************************************************************
*******************************************************************************/

#include"VoltageData.h"
// includes for C system headers
// includes for C++ system headers
#include<sstream>
#include<string>
#include<algorithm>
#include<cctype>
#include<iomanip>
// includes from other libraries
#include <boost/spirit/include/qi.hpp>
// includes from ORCHID
#include"Utility/OrchidLogger.h"

namespace SlowControls
{
CrateStatus::CrateStatus():         mainOn(false),          mainInhibit(false),
    localControlOnly(false),        inputFailure(false),    outputFailure(false),
    fantrayFailure(false),          sensorFailure(false),   vmeSysFailure(false),
    plugAndPlayIncompatible(false), busReset(false),        supplyDerating(false),
    supplyFailure(false) {}

std::string CrateStatus::getStatusString()
{
    std::ostringstream output;
    if(mainOn)
    {
        output << "On ";
    }
    if(mainInhibit)
    {
        output << "Inhibit ";
    }
    if(localControlOnly)
    {
        output << "Lcl_Ctrl_Only ";
    }
    if(inputFailure || outputFailure || fantrayFailure || sensorFailure || vmeSysFailure || plugAndPlayIncompatible || supplyDerating || supplyFailure)
    {
        output << "Failure: ( ";
        if(inputFailure) output << "input ";
        if(outputFailure) output << "output ";
        if(fantrayFailure) output << "fantray ";
        if(sensorFailure) output << "sensor ";
        if(vmeSysFailure) output << "VME_Sys ";
        if(plugAndPlayIncompatible) output << "PnP_Incompatibility ";
        if(supplyDerating) output << "PS_derating ";
        if(supplyFailure) output << "PS_failure ";
        output << ") ";
        
    }
    if(busReset)
    {
        output << "Bus_Reset ";
    }
    return output.str();
}

void CrateStatus::loadFromValue(unsigned int value)
{
    this->mainOn                  = ((value & CrateMasks::MaskMainOn)                  != 0);
    this->mainInhibit             = ((value & CrateMasks::MaskMainInhibit)             != 0);
    this->localControlOnly        = ((value & CrateMasks::MaskLocalControlOnly)        != 0);
    this->inputFailure            = ((value & CrateMasks::MaskInputFailure)            != 0);
    this->outputFailure           = ((value & CrateMasks::MaskOutputFailure)           != 0);
    this->fantrayFailure          = ((value & CrateMasks::MaskFantrayFailue)           != 0);
    this->sensorFailure           = ((value & CrateMasks::MaskSensorFailure)           != 0);
    this->vmeSysFailure           = ((value & CrateMasks::MaskVmeSysFailure)           != 0);
    this->plugAndPlayIncompatible = ((value & CrateMasks::MaskPlugAndPlayIncompatible) != 0);
    this->busReset                = ((value & CrateMasks::MaskBusReset)                != 0);
    this->supplyDerating          = ((value & CrateMasks::MaskSupplyDerating)          != 0);
    this->supplyFailure           = ((value & CrateMasks::MaskSupplyFailure)           != 0);
}

CrateStatus& CrateStatus::operator=(const CrateStatus& rhs)
{
    this->mainOn                  = rhs.mainOn;
    this->mainInhibit             = rhs.mainInhibit;
    this->localControlOnly        = rhs.localControlOnly;
    this->inputFailure            = rhs.inputFailure;
    this->outputFailure           = rhs.outputFailure;
    this->fantrayFailure          = rhs.fantrayFailure;
    this->sensorFailure           = rhs.sensorFailure;
    this->vmeSysFailure           = rhs.vmeSysFailure;
    this->plugAndPlayIncompatible = rhs.plugAndPlayIncompatible;
    this->busReset                = rhs.busReset;
    this->supplyDerating          = rhs.supplyDerating;
    this->supplyFailure           = rhs.supplyFailure;
    return *this;
}

unsigned int CrateStatus::giveIntRepresentation()
{
    unsigned int output = 0x00000000;
    output |= ((this->mainOn)                   ?CrateMasks::MaskMainOn                 :0x00000000);
    output |= ((this->mainInhibit)              ?CrateMasks::MaskMainInhibit            :0x00000000);
    output |= ((this->localControlOnly)         ?CrateMasks::MaskLocalControlOnly       :0x00000000);
    output |= ((this->inputFailure)             ?CrateMasks::MaskInputFailure           :0x00000000);
    output |= ((this->outputFailure)            ?CrateMasks::MaskOutputFailure          :0x00000000);
    output |= ((this->fantrayFailure)           ?CrateMasks::MaskFantrayFailue          :0x00000000);
    output |= ((this->sensorFailure)            ?CrateMasks::MaskSensorFailure          :0x00000000);
    output |= ((this->vmeSysFailure)            ?CrateMasks::MaskVmeSysFailure          :0x00000000);
    output |= ((this->plugAndPlayIncompatible)  ?CrateMasks::MaskPlugAndPlayIncompatible:0x00000000);
    output |= ((this->busReset)                 ?CrateMasks::MaskBusReset               :0x00000000);
    output |= ((this->supplyDerating)           ?CrateMasks::MaskSupplyDerating         :0x00000000);
    output |= ((this->supplyFailure)            ?CrateMasks::MaskSupplyFailure          :0x00000000);
    return output;
}


ChannelStatus::ChannelStatus():          outputOn(false),
    outputInhibit(false),                outputFailureMinSenseVoltage(false),
    outputFailureMaxSenseVoltage(false), outputFailureMaxTerminalVoltage(false),
    outputFailureMaxCurrent(false),      outputFailureMaxTemperature(false),
    outputFailureMaxPower(false),        reserved1(false),
    outputFailureTimeout(false),         outputCurrentLimited(false),
    outputRampUp(false),                 outputRampDown(false),
    outputEnableKill(false),             outputEmergencyOff(false),
    outputAdjusting(true),               outputConstantVoltage(false),
    reserved2(false),                    outputCurrentBoundsExceeded(false),
    outputFailureCurrentLimit(false) {}

std::string ChannelStatus::getStatusString()
{
    std::ostringstream output;
    if(outputOn) output << "On ";
    if(outputInhibit) output << "Inhibit ";
    if(outputFailureMinSenseVoltage || outputFailureMaxSenseVoltage ||
            outputFailureMaxTerminalVoltage || outputFailureMaxCurrent ||
            outputFailureMaxTemperature || outputFailureMaxPower ||
            outputFailureTimeout || outputEmergencyOff ||
            outputCurrentBoundsExceeded || outputFailureCurrentLimit)
    {
        output << "Failure: ( ";
        if(outputFailureMinSenseVoltage) output << "MinSense ";
        if(outputFailureMaxSenseVoltage) output << "MaxSense ";
        if(outputFailureMaxTerminalVoltage) output << "MaxTerm ";
        if(outputFailureMaxCurrent) output << "MaxCurr ";
        if(outputFailureMaxTemperature) output << "MaxTemp ";
        if(outputFailureMaxPower) output << "MaxPower ";
        if(outputFailureTimeout) output << "FailTimout ";
        if(outputEmergencyOff) output << "EmerOff ";
        if(outputCurrentBoundsExceeded) output << "OutOfCurrBounds ";
        if(outputFailureCurrentLimit) output << "CurrLimFail ";
        output << ") ";
        
    }
    if(outputCurrentLimited) output << "OutCurLim ";
    if(outputRampUp) output << "RampUp ";
    if(outputRampDown) output << "RampDown ";
    if(outputAdjusting) output << "FineAdj ";
    if(outputConstantVoltage) output << "ConstVoltage ";
    return output.str();
}


void ChannelStatus::loadFromValue(unsigned int value)
{
    this->outputOn                        = ((value & ChannelMasks::MaskOutputOn)                        != 0);
    this->outputInhibit                   = ((value & ChannelMasks::MaskOutputInhibit)                   != 0);
    this->outputFailureMinSenseVoltage    = ((value & ChannelMasks::MaskOutputFailureMinSenseVoltage)    != 0);
    this->outputFailureMaxSenseVoltage    = ((value & ChannelMasks::MaskOutputFailureMaxSenseVoltage)    != 0);
    this->outputFailureMaxTerminalVoltage = ((value & ChannelMasks::MaskOutputFailureMaxTerminalVoltage) != 0);
    this->outputFailureMaxCurrent         = ((value & ChannelMasks::MaskOutputFailureMaxCurrent)         != 0);
    this->outputFailureMaxTemperature     = ((value & ChannelMasks::MaskOutputFailureMaxTemperature)     != 0);
    this->outputFailureMaxPower           = ((value & ChannelMasks::MaskOutputFailureMaxPower)           != 0);
    this->outputFailureTimeout            = ((value & ChannelMasks::MaskOutputFailureTimeout)            != 0);
    this->outputCurrentLimited            = ((value & ChannelMasks::MaskOutputCurrentLimited)            != 0);
    this->outputRampUp                    = ((value & ChannelMasks::MaskOutputRampUp)                    != 0);
    this->outputRampDown                  = ((value & ChannelMasks::MaskOutputRampDown)                  != 0);
    this->outputEnableKill                = ((value & ChannelMasks::MaskOutputEnableKill)                != 0);
    this->outputEmergencyOff              = ((value & ChannelMasks::MaskOutputEmergencyOff)              != 0);
    this->outputAdjusting                 = ((value & ChannelMasks::MaskOutputAdjusting)                 != 0);
    this->outputConstantVoltage           = ((value & ChannelMasks::MaskOutputConstantVoltage)           != 0);
    this->outputCurrentBoundsExceeded     = ((value & ChannelMasks::MaskOutputCurrentBoundsExceeded)     != 0);
    this->outputFailureCurrentLimit       = ((value & ChannelMasks::MaskOutputFailureCurrentLimit)       != 0);
}

ChannelStatus& ChannelStatus::operator=(const ChannelStatus& rhs)
{
    this->outputOn                        = rhs.outputOn;
    this->outputInhibit                   = rhs.outputInhibit;
    this->outputFailureMinSenseVoltage    = rhs.outputFailureMinSenseVoltage;
    this->outputFailureMaxSenseVoltage    = rhs.outputFailureMaxSenseVoltage;
    this->outputFailureMaxTerminalVoltage = rhs.outputFailureMaxTerminalVoltage;
    this->outputFailureMaxCurrent         = rhs.outputFailureMaxCurrent;
    this->outputFailureMaxTemperature     = rhs.outputFailureMaxTemperature;
    this->outputFailureMaxPower           = rhs.outputFailureMaxPower;
    this->outputFailureTimeout            = rhs.outputFailureTimeout;
    this->outputCurrentLimited            = rhs.outputCurrentLimited;
    this->outputRampUp                    = rhs.outputRampUp;
    this->outputRampDown                  = rhs.outputRampDown;
    this->outputEnableKill                = rhs.outputEnableKill;
    this->outputEmergencyOff              = rhs.outputEmergencyOff;
    this->outputAdjusting                 = rhs.outputAdjusting;
    this->outputConstantVoltage           = rhs.outputConstantVoltage;
    this->outputCurrentBoundsExceeded     = rhs.outputCurrentBoundsExceeded;
    this->outputFailureCurrentLimit       = rhs.outputFailureCurrentLimit;
    return *this;
}

unsigned int ChannelStatus::giveIntRepresentation()
{
    unsigned int output = 0x00000000;
    output |= ((this->outputOn)                         ?ChannelMasks::MaskOutputOn                         :0x00000000);
    output |= ((this->outputInhibit)                    ?ChannelMasks::MaskOutputInhibit                    :0x00000000);
    output |= ((this->outputFailureMinSenseVoltage)     ?ChannelMasks::MaskOutputFailureMinSenseVoltage     :0x00000000);
    output |= ((this->outputFailureMaxSenseVoltage)     ?ChannelMasks::MaskOutputFailureMaxSenseVoltage     :0x00000000);
    output |= ((this->outputFailureMaxTerminalVoltage)  ?ChannelMasks::MaskOutputFailureMaxTerminalVoltage  :0x00000000);
    output |= ((this->outputFailureMaxCurrent)          ?ChannelMasks::MaskOutputFailureMaxCurrent          :0x00000000);
    output |= ((this->outputFailureMaxTemperature)      ?ChannelMasks::MaskOutputFailureMaxTemperature      :0x00000000);
    output |= ((this->outputFailureMaxPower)            ?ChannelMasks::MaskOutputFailureMaxPower            :0x00000000);
    output |= ((this->outputFailureTimeout)             ?ChannelMasks::MaskOutputFailureTimeout             :0x00000000);
    output |= ((this->outputCurrentLimited)             ?ChannelMasks::MaskOutputCurrentLimited             :0x00000000);
    output |= ((this->outputRampUp)                     ?ChannelMasks::MaskOutputRampUp                     :0x00000000);
    output |= ((this->outputRampDown)                   ?ChannelMasks::MaskOutputRampDown                   :0x00000000);
    output |= ((this->outputEnableKill)                 ?ChannelMasks::MaskOutputEnableKill                 :0x00000000);
    output |= ((this->outputEmergencyOff)               ?ChannelMasks::MaskOutputEmergencyOff               :0x00000000);
    output |= ((this->outputAdjusting)                  ?ChannelMasks::MaskOutputAdjusting                  :0x00000000);
    output |= ((this->outputConstantVoltage)            ?ChannelMasks::MaskOutputConstantVoltage            :0x00000000);
    output |= ((this->outputCurrentBoundsExceeded)      ?ChannelMasks::MaskOutputCurrentBoundsExceeded      :0x00000000);
    output |= ((this->outputFailureCurrentLimit)        ?ChannelMasks::MaskOutputFailureCurrentLimit        :0x00000000);
    return output;
}

VoltageData::VoltageData(int channels):                terminalVoltage(channels),
    senseVoltage(channels), setVoltage(channels),      temperature(channels),
    current(channels),      outputSwitch(channels),    rampUpRate(channels),
    rampDownRate(channels), currentTripTime(channels), maxTemperature(channels),
    maxCurrent(channels),   maxVoltage(channels),      channelStatus(channels),
    numChannels(channels) {}

void VoltageData::loadOutputSwitches(const std::string& input)
{
    int i = 0;
    std::stringstream inStream(input + '\n');
    std::string line;
    
    while(std::getline(inStream, line, '\n') && (i < numChannels))
    {
        //get the integer representation of the bits line
        int final = parseIntegerLine(input);
        //send the integer representation into the struct for parsing
        outputSwitch[i] = ((final==0)?false:true);
        ++i;
    }
}

void VoltageData::loadChannelStatuses(const std::string& input)
{
    int i = 0;
    std::stringstream inStream(input + '\n');
    std::string line;
    
    while(std::getline(inStream, line, '\n') && (i < numChannels))
    {
        //get the integer representation of the bits line
        unsigned int final = parseBitsLine(input, 6);
        //send the integer representation into the struct for parsing
        channelStatus[i].loadFromValue(final);
        ++i;
    }
}

void VoltageData::loadCrateStatus(const std::string& input)
{
    //get the integer representation of the bits line
    unsigned int final = parseBitsLine(input, 4);
    //send the integer representation into the struct for parsing
    crateStatus.loadFromValue(final);
}

void VoltageData::parseMultiLineInt(const std::string& input, std::vector<int>& container, int mult)
{
    int i = 0;
    std::stringstream inStream(input + '\n');
    std::string line;
    
    while(std::getline(inStream, line, '\n') && (i < numChannels))
    {
        //extract the int from the line
        int final = parseIntegerLine(line);
        //now put the values into the vector
        container[i] = mult * final;
        ++i;
    }
}

void VoltageData::parseMultiLineFloat(const std::string& input, std::vector<float>& container, float mult)
{
    int i = 0;
    std::stringstream inStream(input + '\n');
    std::string line;
    
    while(std::getline(inStream, line, '\n') && (i < numChannels))
    {
        //extract the float from the line
        float final = parseFloatLine(line);
        //now put the values into the vector
        container[i] = mult * final;
        ++i;
    }
}

int VoltageData::parseIntegerLine(const std::string& line)
{
    using namespace boost::spirit::qi;
    std::string input(line + '\n');
    int output;
    //parse the interesting part of the input line
    parse(line.begin(), line.end(), lexeme["INTEGER: "] >> int_ >> +eol, output);
    return output;
}

float VoltageData::parseFloatLine(const std::string& line)
{
    using namespace boost::spirit::qi;
    std::string input(line + '\n');
    float output;
    //parse the interesting part of the input line
    parse(input.begin(), input.end(), lexeme["Opaque: Float: "] >> float_ >> +eol, output);
    return output;
}

unsigned int VoltageData::parseBitsLine(const std::string& line, int nibbleCount)
{
    using namespace boost::spirit::qi;
    BOOST_LOG_SEV(OrchidLog::get(), Information) << "Preparing to parse bits line, nibbles is: " << nibbleCount;
    BOOST_LOG_SEV(OrchidLog::get(), Information) << "Line: " << line;
    std::string input(line + '\n');
    std::string intermediate;
    //parse the interesting part of the input line
    parse(input.begin(), input.end(), lexeme["BITS: "] >> string_ >> lexeme["["] >> +char_('.') >> lexeme["]"] >> +eol, intermediate);
    BOOST_LOG_SEV(OrchidLog::get(), Information) << "Intermediate: " << intermediate;
    //remove the spaces
    intermediate.erase(std::remove_if(intermediate.begin(), intermediate.end(), [](char x){return std::isspace(x);}), intermediate.end());
    BOOST_LOG_SEV(OrchidLog::get(), Information) << "Intermediate: " << intermediate;
    //since mpod does not return a trailing byte if its value is zero (ie no bits set)
    //calculate how many bits we need to push to result up to have a '4*nibbleCount-bit' integer
    int push = (4 * (nibbleCount -  intermediate.find_first_of('[')));
    //parse into an integer from hex format
    unsigned int final;
    parse(intermediate.begin(), intermediate.end(), hex, final);
    BOOST_LOG_SEV(OrchidLog::get(), Information) << "Value To Convert To Bits: " << final;
    final <<= push;
    return final;    
}

}
