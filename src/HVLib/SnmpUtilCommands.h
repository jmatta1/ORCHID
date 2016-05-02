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
** @details Template definitions that do some magic to convert a strongly typed
**  enumeration into a strings
**
********************************************************************************
*******************************************************************************/

#ifndef ORCHID_SRC_HVLIB_SNMPUTILCOMMANDS_H
#define ORCHID_SRC_HVLIB_SNMPUTILCOMMANDS_H

// includes for C system headers
// includes for C++ system headers
#include<map>
#include<string>
// includes from other libraries
#include<boost/assign/list_of.hpp>
using boost::assign::map_list_of;
// includes from ORCHID

//this is a hackish solution to generate the appropriate strings for enums
//it is crap to maintain, but it will work for now
enum class: char MpodGlobalGetParam{SysMainSwitch, SysStatus};


enum class: char MpodChannelGetParam{OutputStatus, OutputSwitch, SenseVoltage, TerminalVoltage,
                                     Temperature, SetVoltage, Current, RampUp,
                                     RampDown, MaxTerminalVoltage, MaxCurrent, 
                                     MaxCurrentTripTime};

enum class: char MpodGlobalSetParam{SysMainSwitch};


enum class: char MpodChannelSetParam{OutputSwitch, SetVoltage, RampUp, RampDown,
                                     MaxTerminalVoltage, MaxCurrent, MaxCurrentTripTime};

namespace CmdLookup
{

static const std::map<MpodGlobalGetParam, std::string> GLOBAL_GET_COMMANDS =
    map_list_of (MpodGlobalGetParam:SysMainSwitch, "sysMainSwitch.0")
                (MpodGlobalGetParam:SysStatus    , "sysStatus.0");

static const std::map<MpodChannelGetParam, std::string> CHANNEL_GET_COMMANDS =
    map_list_of (MpodChannelGetParam:OutputStatus      , "outputStatus")
                (MpodChannelGetParam:OutputSwitch      , "outputSwitch")
                (MpodChannelGetParam:SenseVoltage      , "outputMeasurementSenseVoltage")
                (MpodChannelGetParam:TerminalVoltage   , "outputMeasurementTerminalVoltage")
                (MpodChannelGetParam:Temperature       , "outputMeasurementTemperature")
                (MpodChannelGetParam:SetVoltage        , "outputVoltage")
                (MpodChannelGetParam:Current           , "outputCurrent")
                (MpodChannelGetParam:RampUp            , "outputVoltageRiseRate")
                (MpodChannelGetParam:RampDown          , "outputVoltageFallRate")
                (MpodChannelGetParam:MaxTerminalVoltage, "outputSupervisionMaxTerminalVoltage")
                (MpodChannelGetParam:MaxCurrent        , "outputSupervisionMaxCurrent")
                (MpodChannelGetParam:MaxCurrentTripTime, "outputTripTimeMaxCurrent");

static const std::map<MpodGlobalSetParam, std::string> GLOBAL_SET_COMMANDS =
    map_list_of (MpodGlobalSetParam:SysMainSwitch, "sysMainSwitch.0");

static const std::map<MpodChannelSetParam, std::string> CHANNEL_SET_COMMANDS =
    map_list_of (MpodChannelSetParam:OutputSwitch      , "outputSwitch")
                (MpodChannelSetParam:SetVoltage        , "outputVoltage"))
                (MpodChannelGetParam:RampUp            , "outputVoltageRiseRate")
                (MpodChannelSetParam:RampDown          , "outputVoltageFallRate")
                (MpodChannelSetParam:MaxTerminalVoltage, "outputSupervisionMaxTerminalVoltage")
                (MpodChannelSetParam:MaxCurrent        , "outputSupervisionMaxCurrent")
                (MpodChannelSetParam:MaxCurrentTripTime, "outputTripTimeMaxCurrent");

}
#endif //ORCHID_SRC_HVLIB_SNMPUTILCOMMANDS_H
