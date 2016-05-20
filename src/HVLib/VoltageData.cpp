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
// includes from other libraries
#include <boost/spirit/include/qi.hpp>
// includes from ORCHID

namespace SlowControls
{
CrateStatus::CrateStatus():         mainOn(false),          mainInhibit(false),
    localControlOnly(false),        inputFailure(false),    outputFailure(false),
    fantrayFailure(false),          sensorFailure(false),   vmeSysFailure(false),
    plugAndPlayIncompatible(false), busReset(false),        supplyDerating(false),
    supplyFailure(false) {}

//TODO: Parse channel status into nice string format for UI
std::string CrateStatus::getStatusString()
{
    return std::string("");
}

void CrateStatus::loadFromValue(unsigned int value)
{
    mainOn                  = ((value & CrateMasks::MaskMainOn)                  != 0);
    mainInhibit             = ((value & CrateMasks::MaskMainInhibit)             != 0);
    localControlOnly        = ((value & CrateMasks::MaskLocalControlOnly)        != 0);
    inputFailure            = ((value & CrateMasks::MaskInputFailure)            != 0);
    outputFailure           = ((value & CrateMasks::MaskOutputFailure)           != 0);
    fantrayFailure          = ((value & CrateMasks::MaskFantrayFailue)           != 0);
    sensorFailure           = ((value & CrateMasks::MaskSensorFailure)           != 0);
    vmeSysFailure           = ((value & CrateMasks::MaskVmeSysFailure)           != 0);
    plugAndPlayIncompatible = ((value & CrateMasks::MaskPlugAndPlayIncompatible) != 0);
    busReset                = ((value & CrateMasks::MaskBusReset)                != 0);
    supplyDerating          = ((value & CrateMasks::MaskSupplyDerating)          != 0);
    supplyFailure           = ((value & CrateMasks::MaskSupplyFailure)           != 0);
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

//TODO: Parse channel status into nice string format for UI
std::string ChannelStatus::getStatusString()
{
    return std::string("");
}


void ChannelStatus::loadFromValue(unsigned int value)
{
    outputOn                        = ((value & ChannelMasks::MaskOutputOn)                        != 0);
    outputInhibit                   = ((value & ChannelMasks::MaskOutputInhibit)                   != 0);
    outputFailureMinSenseVoltage    = ((value & ChannelMasks::MaskOutputFailureMinSenseVoltage)    != 0);
    outputFailureMaxSenseVoltage    = ((value & ChannelMasks::MaskOutputFailureMaxSenseVoltage)    != 0);
    outputFailureMaxTerminalVoltage = ((value & ChannelMasks::MaskOutputFailureMaxTerminalVoltage) != 0);
    outputFailureMaxCurrent         = ((value & ChannelMasks::MaskOutputFailureMaxCurrent)         != 0);
    outputFailureMaxTemperature     = ((value & ChannelMasks::MaskOutputFailureMaxTemperature)     != 0);
    outputFailureMaxPower           = ((value & ChannelMasks::MaskOutputFailureMaxPower)           != 0);
    outputFailureTimeout            = ((value & ChannelMasks::MaskOutputFailureTimeout)            != 0);
    outputCurrentLimited            = ((value & ChannelMasks::MaskOutputCurrentLimited)            != 0);
    outputRampUp                    = ((value & ChannelMasks::MaskOutputRampUp)                    != 0);
    outputRampDown                  = ((value & ChannelMasks::MaskOutputRampDown)                  != 0);
    outputEnableKill                = ((value & ChannelMasks::MaskOutputEnableKill)                != 0);
    outputEmergencyOff              = ((value & ChannelMasks::MaskOutputEmergencyOff)              != 0);
    outputAdjusting                 = ((value & ChannelMasks::MaskOutputAdjusting)                 != 0);
    outputConstantVoltage           = ((value & ChannelMasks::MaskOutputConstantVoltage)           != 0);
    outputCurrentBoundsExceeded     = ((value & ChannelMasks::MaskOutputCurrentBoundsExceeded)     != 0);
    outputFailureCurrentLimit       = ((value & ChannelMasks::MaskOutputFailureCurrentLimit)       != 0);
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
    std::string input(line + '\n');
    std::string intermediate;
    //parse the interesting part of the input line
    parse(input.begin(), input.end(), lexeme["BITS: "] >> string_ >> lexeme["["] >> +char_('.') >> lexeme["]"] >> +eol, intermediate);
    //remove the spaces
    intermediate.erase(std::remove_if(intermediate.begin(), intermediate.end(), [](char x){return std::isspace(x);}), intermediate.end());
    //since mpod does not return a trailing byte if its value is zero (ie no bits set)
    //calculate how many bits we need to push to result up to have a '4*nibbleCount-bit' integer
    int push = (4 * (nibbleCount -  intermediate.size()));
    //parse into an integer from hex format
    unsigned int final;
    parse(intermediate.begin(), intermediate.end(), hex, final);
    final <<= push;
    return final;    
}

}
