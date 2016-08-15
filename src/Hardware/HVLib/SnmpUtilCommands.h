/***************************************************************************//**
********************************************************************************
**
** @file SnmpUtilCommands.h
** @author James Till Matta
** @date 02 May, 2016
** @brief
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** @details Lookup table to convert a command enum into a command string
**
********************************************************************************
*******************************************************************************/

#ifndef ORCHID_SRC_HARDWARE_HVLIB_SNMPUTILCOMMANDS_H
#define ORCHID_SRC_HARDWARE_HVLIB_SNMPUTILCOMMANDS_H

// includes for C system headers
// includes for C++ system headers
#include<map>
#include<string>
// includes from other libraries
#include<boost/assign/list_of.hpp>
using boost::assign::map_list_of;
// includes from ORCHID

namespace SlowControls
{
//this is a hackish solution to generate the appropriate strings for enums
//it is crap to maintain, but it will work for now
enum class MpodGlobalGetParam: char{SysMainSwitch, SysStatus};


enum class MpodChannelGetParam: char{OutputStatus, OutputSwitch, SenseVoltage, TerminalVoltage,
                                     Temperature, SetVoltage, Current, RampUp,
                                     RampDown, MaxTerminalVoltage, MaxCurrent,
                                     SetCurrent, MaxCurrentTripTime};

enum class MpodGlobalSetParam: char{SysMainSwitch};


enum class MpodChannelSetParam: char{OutputSwitch, SetVoltage, RampUp, RampDown,
                                     MaxTerminalVoltage, MaxCurrent, MaxCurrentTripTime};

namespace CmdLookup
{

static const std::map<MpodGlobalGetParam, std::string> GLOBAL_GET_COMMANDS =
    map_list_of (MpodGlobalGetParam::SysMainSwitch, "sysMainSwitch.0")
                (MpodGlobalGetParam::SysStatus    , "sysStatus.0");

static const std::map<MpodChannelGetParam, std::string> CHANNEL_GET_COMMANDS =
    map_list_of (MpodChannelGetParam::OutputStatus      , "outputStatus")
                (MpodChannelGetParam::OutputSwitch      , "outputSwitch")
                (MpodChannelGetParam::SenseVoltage      , "outputMeasurementSenseVoltage")
                (MpodChannelGetParam::TerminalVoltage   , "outputMeasurementTerminalVoltage")
                (MpodChannelGetParam::Current           , "outputMeasurementCurrent")
                (MpodChannelGetParam::Temperature       , "outputMeasurementTemperature")
                (MpodChannelGetParam::SetVoltage        , "outputVoltage")
                (MpodChannelGetParam::SetCurrent        , "outputCurrent")
                (MpodChannelGetParam::RampUp            , "outputVoltageRiseRate")
                (MpodChannelGetParam::RampDown          , "outputVoltageFallRate")
                (MpodChannelGetParam::MaxTerminalVoltage, "outputSupervisionMaxTerminalVoltage")
                (MpodChannelGetParam::MaxCurrent        , "outputSupervisionMaxCurrent")
                (MpodChannelGetParam::MaxCurrentTripTime, "outputTripTimeMaxCurrent");

static const std::map<MpodGlobalSetParam, std::string> GLOBAL_SET_COMMANDS =
    map_list_of (MpodGlobalSetParam::SysMainSwitch, "sysMainSwitch.0");

static const std::map<MpodChannelSetParam, std::string> CHANNEL_SET_COMMANDS =
    map_list_of (MpodChannelSetParam::OutputSwitch      , "outputSwitch")
                (MpodChannelSetParam::SetVoltage        , "outputVoltage")
                (MpodChannelSetParam::RampUp            , "outputVoltageRiseRate")
                (MpodChannelSetParam::RampDown          , "outputVoltageFallRate")
                (MpodChannelSetParam::MaxTerminalVoltage, "outputSupervisionMaxTerminalVoltage")
                (MpodChannelSetParam::MaxCurrent        , "outputSupervisionMaxCurrent")
                (MpodChannelSetParam::MaxCurrentTripTime, "outputTripTimeMaxCurrent");

static const std::map<MpodGlobalSetParam, std::string> GLOBAL_SET_USERS =
    map_list_of (MpodGlobalSetParam::SysMainSwitch, "guru");

static const std::map<MpodChannelSetParam, std::string> CHANNEL_SET_USERS =
    map_list_of (MpodChannelSetParam::OutputSwitch      , "guru")
                (MpodChannelSetParam::SetVoltage        , "guru")
                (MpodChannelSetParam::RampUp            , "guru")
                (MpodChannelSetParam::RampDown          , "guru")
                (MpodChannelSetParam::MaxTerminalVoltage, "guru")
                (MpodChannelSetParam::MaxCurrent        , "guru")
                (MpodChannelSetParam::MaxCurrentTripTime, "guru");

static const std::map<MpodGlobalSetParam, char> GLOBAL_SET_TYPES =
    map_list_of (MpodGlobalSetParam::SysMainSwitch, 'i');

static const std::map<MpodChannelSetParam, char> CHANNEL_SET_TYPES =
    map_list_of (MpodChannelSetParam::OutputSwitch      , 'i')
                (MpodChannelSetParam::SetVoltage        , 'F')
                (MpodChannelSetParam::RampUp            , 'F')
                (MpodChannelSetParam::RampDown          , 'F')
                (MpodChannelSetParam::MaxTerminalVoltage, 'F')
                (MpodChannelSetParam::MaxCurrent        , 'F')
                (MpodChannelSetParam::MaxCurrentTripTime, 'i');

}
}
#endif //ORCHID_SRC_HARDWARE_HVLIB_SNMPUTILCOMMANDS_H
