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


CrateStatus::CrateStatus():         mainOn(false),          mainInhibit(false),
    localControlOnly(false),        inputFailure(false),    outputFailure(false),
    fantrayFailure(false),          sensorFailure(false),   vmeSysFailure(false),
    plugAndPlayIncompatible(false), busReset(false),        supplyDerating(false),
    supplyFailure(false) {}

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

void VoltageData::loadTerminalVoltages(const std::string& input)
{
    int i = 0;
    std::stringstream inStream(input + '\n');
    std::string line;
    std::string intermediate;
    using namespace boost::spirit::qi;
    
    while(std::getline(inStream, line, '\n') && (i < numChannels))
    {
        line.append("\n");
        //parse the interesting part of the input line
        parse(line.begin(), line.end(), lexeme["BITS: "] >> string_ >> eol, intermediate);
        //remove the spaces
        intermediate.erase(std::remove_if(intermediate.begin(), intermediate.end(),[](char x){return std::isspace(x);}), intermediate.end());
        float final;
        //parse into a float
        parse(intermediate.begin(), intermediate.end(), float_, final);
        //now put the values into the vector
        terminalVoltage[i] = final;
        ++i;
    }
}

void VoltageData::loadSenseVoltages(const std::string& input)
{
    int i = 0;
    std::stringstream inStream(input + '\n');
    std::string line;
    std::string intermediate;
    using namespace boost::spirit::qi;
    
    while(std::getline(inStream, line, '\n') && (i < numChannels))
    {
        line.append("\n");
        //parse the interesting part of the input line
        parse(line.begin(), line.end(), lexeme["BITS: "] >> string_ >> eol, intermediate);
        //remove the spaces
        intermediate.erase(std::remove_if(intermediate.begin(), intermediate.end(),[](char x){return std::isspace(x);}), intermediate.end());
        float final;
        //parse into a float
        parse(intermediate.begin(), intermediate.end(), float_, final);
        //now put the values into the vector
        senseVoltage[i] = final;
        ++i;
    }
}

void VoltageData::loadSetVoltages(const std::string& input)
{
    int i = 0;
    std::stringstream inStream(input + '\n');
    std::string line;
    std::string intermediate;
    using namespace boost::spirit::qi;
    
    while(std::getline(inStream, line, '\n') && (i < numChannels))
    {
        line.append("\n");
        //parse the interesting part of the input line
        parse(line.begin(), line.end(), lexeme["BITS: "] >> string_ >> eol, intermediate);
        //remove the spaces
        intermediate.erase(std::remove_if(intermediate.begin(), intermediate.end(),[](char x){return std::isspace(x);}), intermediate.end());
        float final;
        //parse into a float
        parse(intermediate.begin(), intermediate.end(), float_, final);
        //now put the values into the vector
        setVoltage[i] = final;
        ++i;
    }
}

void VoltageData::loadTemperatures(const std::string& input)
{
    int i = 0;
    std::stringstream inStream(input + '\n');
    std::string line;
    std::string intermediate;
    using namespace boost::spirit::qi;
    
    while(std::getline(inStream, line, '\n') && (i < numChannels))
    {
        line.append("\n");
        //parse the interesting part of the input line
        parse(line.begin(), line.end(), lexeme["BITS: "] >> string_ >> eol, intermediate);
        //remove the spaces
        intermediate.erase(std::remove_if(intermediate.begin(), intermediate.end(),[](char x){return std::isspace(x);}), intermediate.end());
        int final;
        //parse into an integer
        parse(intermediate.begin(), intermediate.end(), int_, final);
        //now put the values into the vector
        temperature[i] = final;
        ++i;
    }
}

void VoltageData::loadCurrents(const std::string& input)
{
    int i = 0;
    std::stringstream inStream(input + '\n');
    std::string line;
    std::string intermediate;
    using namespace boost::spirit::qi;
    
    while(std::getline(inStream, line, '\n') && (i < numChannels))
    {
        line.append("\n");
        //parse the interesting part of the input line
        parse(line.begin(), line.end(), lexeme["BITS: "] >> string_ >> eol, intermediate);
        //remove the spaces
        intermediate.erase(std::remove_if(intermediate.begin(), intermediate.end(),[](char x){return std::isspace(x);}), intermediate.end());
        float final;
        //parse into a float
        parse(intermediate.begin(), intermediate.end(), float_, final);
        //now put the values into the vector
        current[i] = final;
        ++i;
    }
}

void VoltageData::loadOutputSwitchs(const std::string& input)
{
    int i = 0;
    std::stringstream inStream(input + '\n');
    std::string line;
    std::string intermediate;
    using namespace boost::spirit::qi;
    
    while(std::getline(inStream, line, '\n') && (i < numChannels))
    {
        line.append("\n");
        //parse the interesting part of the input line
        parse(line.begin(), line.end(), lexeme["BITS: "] >> string_ >> eol, intermediate);
        //remove the spaces
        intermediate.erase(std::remove_if(intermediate.begin(), intermediate.end(),[](char x){return std::isspace(x);}), intermediate.end());
        int final;
        //parse into an integer
        parse(intermediate.begin(), intermediate.end(), int_, final);
        //now put the values into the vector
        outputSwitch[i] = (final != 0);
        ++i;
    }
}

void VoltageData::loadRampUpRates(const std::string& input)
{
    int i = 0;
    std::stringstream inStream(input + '\n');
    std::string line;
    std::string intermediate;
    using namespace boost::spirit::qi;
    
    while(std::getline(inStream, line, '\n') && (i < numChannels))
    {
        line.append("\n");
        //parse the interesting part of the input line
        parse(line.begin(), line.end(), lexeme["BITS: "] >> string_ >> eol, intermediate);
        //remove the spaces
        intermediate.erase(std::remove_if(intermediate.begin(), intermediate.end(),[](char x){return std::isspace(x);}), intermediate.end());
        float final;
        //parse into a float
        parse(intermediate.begin(), intermediate.end(), float_, final);
        //now put the values into the vector
        rampUpRate[i] = final;
        ++i;
    }
}

void VoltageData::loadRampDownRates(const std::string& input)
{
    int i = 0;
    std::stringstream inStream(input + '\n');
    std::string line;
    std::string intermediate;
    using namespace boost::spirit::qi;
    
    while(std::getline(inStream, line, '\n') && (i < numChannels))
    {
        line.append("\n");
        //parse the interesting part of the input line
        parse(line.begin(), line.end(), lexeme["BITS: "] >> string_ >> eol, intermediate);
        //remove the spaces
        intermediate.erase(std::remove_if(intermediate.begin(), intermediate.end(),[](char x){return std::isspace(x);}), intermediate.end());
        float final;
        //parse into a float
        parse(intermediate.begin(), intermediate.end(), float_, final);
        //now put the values into the vector
        rampDownRate[i] = final;
        ++i;
    }
}

void VoltageData::loadCurrentTripTimes(const std::string& input)
{
    int i = 0;
    std::stringstream inStream(input + '\n');
    std::string line;
    std::string intermediate;
    using namespace boost::spirit::qi;
    
    while(std::getline(inStream, line, '\n') && (i < numChannels))
    {
        line.append("\n");
        //parse the interesting part of the input line
        parse(line.begin(), line.end(), lexeme["BITS: "] >> string_ >> eol, intermediate);
        //remove the spaces
        intermediate.erase(std::remove_if(intermediate.begin(), intermediate.end(),[](char x){return std::isspace(x);}), intermediate.end());
        int final;
        //parse into an integer
        parse(intermediate.begin(), intermediate.end(), int_, final);
        //now put the values into the vector
        currentTripTime[i] = final;
        ++i;
    }
}

void VoltageData::loadMaxTemperatures(const std::string& input)
{
    int i = 0;
    std::stringstream inStream(input + '\n');
    std::string line;
    std::string intermediate;
    using namespace boost::spirit::qi;
    
    while(std::getline(inStream, line, '\n') && (i < numChannels))
    {
        line.append("\n");
        //parse the interesting part of the input line
        parse(line.begin(), line.end(), lexeme["BITS: "] >> string_ >> eol, intermediate);
        //remove the spaces
        intermediate.erase(std::remove_if(intermediate.begin(), intermediate.end(),[](char x){return std::isspace(x);}), intermediate.end());
        int final;
        //parse into an integer
        parse(intermediate.begin(), intermediate.end(), int_, final);
        //now put the values into the vector
        maxTemperature[i] = final;
        ++i;
    }
}

void VoltageData::loadMaxCurrents(const std::string& input)
{
    int i = 0;
    std::stringstream inStream(input + '\n');
    std::string line;
    std::string intermediate;
    using namespace boost::spirit::qi;
    
    while(std::getline(inStream, line, '\n') && (i < numChannels))
    {
        line.append("\n");
        //parse the interesting part of the input line
        parse(line.begin(), line.end(), lexeme["BITS: "] >> string_ >> eol, intermediate);
        //remove the spaces
        intermediate.erase(std::remove_if(intermediate.begin(), intermediate.end(),[](char x){return std::isspace(x);}), intermediate.end());
        float final;
        //parse into a float
        parse(intermediate.begin(), intermediate.end(), float_, final);
        //now put the values into the vector
        maxCurrent[i] = final;
        ++i;
    }
}

void VoltageData::loadMaxVoltages(const std::string& input)
{
    int i = 0;
    std::stringstream inStream(input + '\n');
    std::string line;
    std::string intermediate;
    using namespace boost::spirit::qi;
    
    while(std::getline(inStream, line, '\n') && (i < numChannels))
    {
        line.append("\n");
        //parse the interesting part of the input line
        parse(line.begin(), line.end(), lexeme["BITS: "] >> string_ >> eol, intermediate);
        //remove the spaces
        intermediate.erase(std::remove_if(intermediate.begin(), intermediate.end(),[](char x){return std::isspace(x);}), intermediate.end());
        float final;
        //parse into a float
        parse(intermediate.begin(), intermediate.end(), float_, final);
        //now put the values into the vector
        maxVoltage[i] = final;
        ++i;
    }
}

void VoltageData::loadChannelStatuses(const std::string& input)
{
    int i = 0;
    std::stringstream inStream(input + '\n');
    std::string line;
    std::string intermediate;
    using namespace boost::spirit::qi;
    
    while(std::getline(inStream, line, '\n') && (i < numChannels))
    {
        line.append("\n");
        //parse the interesting part of the input line
        parse(line.begin(), line.end(), lexeme["BITS: "] >> string_ >> eol, intermediate);
        //remove the spaces
        intermediate.erase(std::remove_if(intermediate.begin(), intermediate.end(),[](char x){return std::isspace(x);}), intermediate.end());
        unsigned int final;
        //parse into an integer from hex format
        parse(intermediate.begin(), intermediate.end(), hex, final);
        //now put the values into the vector
        channelStatus[i].loadFromValue(final);
        ++i;
    }
}

void VoltageData::loadCrateStatus(const std::string& input)
{
    using namespace boost::spirit::qi;
    std::string line(input + '\n');
    std::string intermediate;
    //parse the interesting part of the input line
    parse(line.begin(), line.end(), lexeme["BITS: "] >> string_ >> eol, intermediate);
    //remove the spaces
    intermediate.erase(std::remove_if(intermediate.begin(), intermediate.end(), [](char x){return std::isspace(x);}), intermediate.end());
    unsigned int final;
    //parse into an integer from hex format
    parse(intermediate.begin(), intermediate.end(), hex, final);
    //now put the values into the vector
    crateStatus.loadFromValue(final);
}


