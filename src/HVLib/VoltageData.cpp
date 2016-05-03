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
#include<algorithm>
#include<cctype>
// includes from other libraries
#include <boost/spirit/include/qi.hpp>
// includes from ORCHID
#include "InputLib/Parsers/Utility.h"


CrateStatus::CrateStatus():         mainOn(false),          mainInhibit(false),
    localControlOnly(false),        inputFailure(false),    outputFailure(false),
    fantrayFailure(false),          sensorFailure(false),   vmeSysfail(false),
    plugAndPlayIncompatible(false), busReset(false),        supplyDerating(false),
    supplyFailure(false),           supplyDerating2(false), supplyFailure2(false){}

std::string CrateStatus::getStatusString()
{
    return std::string("");
}

void CrateStatus::loadFromValue(int value);
{

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
    return std::string("");
}

void ChannelStatus::loadFromValue(int value);
{

}

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

VoltageData::VoltageData(int channels):                terminalVoltage(channels),
    senseVoltage(channels), setVoltage(channels),      temperature(channels),
    current(channels),      outputSwitch(channels),    rampUpRate(channels),
    rampDownRate(channels), currentTripTime(channels), maxTemperature(channels),
    maxCurrent(channels),   maxVoltage(channels),      channelStatus(maxVoltage),
    numChannels(channels) {}


void VoltageData::loadTerminalVoltages(const std::string& input)
{

}

void VoltageData::loadSenseVoltages(const std::string& input)
{

}

void VoltageData::loadSetVoltages(const std::string& input)
{

}

void VoltageData::loadTemperatures(const std::string& input)
{

}

void VoltageData::loadCurrents(const std::string& input)
{

}

void VoltageData::loadOutputSwitchs(const std::string& input)
{

}

void VoltageData::loadRampUpRates(const std::string& input)
{

}

void VoltageData::loadRampDownRates(const std::string& input)
{

}

void VoltageData::loadCurrentTripTimes(const std::string& input)
{

}

void VoltageData::loadMaxTemperatures(const std::string& input)
{

}

void VoltageData::loadMaxCurrents(const std::string& input)
{

}

void VoltageData::loadMaxVoltages(const std::string& input)
{

}

void VoltageData::loadChannelStatuses(const std::string& input)
{
    int i = 0;
    std::stringstream inStream(input);
    std::string line;
    std::string intermediate;
    
    while(std::getline(inStream, line, '\n') && (i < numChannels))
    {
        line.append('\n');
        //parse the interesting part of the input line
        boost::spirit::qi::parse(test.begin(), test.end(), lexeme["BITS: "] >> string_ >> eol, intermediate);
        //remove the spaces
        intermediate.erase(std::remove_if(intermediate.begin(), intermediate.end(), std::isspace), intermediate.end());
        float final;
        //parse into an integer from hex format
        boost::spirit::qi::parse(intermediate.begin(), intermediate.end(), hex, final);
        //now put the values into the vector
        channelStatus[i].loadFromValue(final);
        ++i;
    }
}

void VoltageData::loadCrateStatus(const std::string& input)
{

}


