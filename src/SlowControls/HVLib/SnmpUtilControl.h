/***************************************************************************//**
********************************************************************************
**
** @file SnmpUtilControl.h
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
** @details Definition file for the class that runs the MPOD control stuff
**
********************************************************************************
*******************************************************************************/

#ifndef ORCHID_SRC_SLOWCONTROLS_HVLIB_SNMPUTILCONTROL_H
#define ORCHID_SRC_SLOWCONTROLS_HVLIB_SNMPUTILCONTROL_H

//TODO: convert to asio based direct communication rather than using the snmp
// utils as intermediates, this will require some work and interfacing with the
// open snmp library directly

// includes for C system headers
// includes for C++ system headers
#include<string>
#include<sstream>
#include<boost/thread.hpp>
// includes from other libraries
// includes from ORCHID
#include"SnmpUtilCommands.h"
namespace SlowControls
{
class SnmpUtilControl
{
public:
    SnmpUtilControl(const std::string& ip, const std::string& mibLoc):
        ipAddress(ip), mibLocation(mibLoc) {}
    ~SnmpUtilControl(){}
    
    std::string snmpGlobalGet(MpodGlobalGetParam command);
    std::string snmpChannelGet(MpodChannelGetParam command, int board, int channel);
    std::string snmpChannelWalk(MpodChannelGetParam command);

    
    template<class Number>
    std::string snmpGlobalSet(MpodGlobalSetParam command, Number value);
    template<class Number>
    std::string snmpChannelSet(MpodChannelSetParam command, int board, int channel, Number value);

private:
    template<class Number>
    std::string buildSetChannelParameter(const std::string& paramName, const std::string& channel, const std::string& type, Number value) const;
    template<class Number>
    std::string buildSetGlobalParameter(const std::string& paramName, const std::string& type, Number value) const;
    std::string buildCommand(const std::string& command, const std::string& user, const std::string& parameter) const;


    std::string convertToUniversalChannel(int board, int channel) const;
    std::string runCommand(const std::string& command);

    //member variables
    //data for creating commands
    std::string ipAddress;
    std::string mibLocation;
    
    //threading protection
    // this mutex will be infrequently locked, when it is locked it will be for a
    // relativelyfor long period of time (milliseconds), and since we should
    // probably not have two snmp commands trying to talk to the mpod
    // simultaneously (and yet we need to have two threads be able to access
    // the mpod at a given time) we use an exclusive mutex to guard commands
    // sent to the digitizer
    boost::mutex commandMutex;
};

template<class Number>
std::string SnmpUtilControl::snmpGlobalSet(MpodGlobalSetParam command,
                                           Number value)
{
    if(CmdLookup::GLOBAL_SET_TYPES.at(command) == 'i')
    {
        return this->runCommand(this->buildCommand("snmpset",
                                                   CmdLookup::GLOBAL_SET_USERS.at(command),
                                                   this->buildSetGlobalParameter(
                                                       CmdLookup::GLOBAL_SET_COMMANDS.at(command),
                                                       "i", (int)value) ));
    }
    else
    {
        return this->runCommand(this->buildCommand("snmpset",
                                                   CmdLookup::GLOBAL_SET_USERS.at(command),
                                                   this->buildSetGlobalParameter(
                                                       CmdLookup::GLOBAL_SET_COMMANDS.at(command),
                                                       "F", (float)value) ));
    }
}

template<class Number>
std::string SnmpUtilControl::snmpChannelSet(MpodChannelSetParam command,
                                            int board, int channel,
                                            Number value)
{
    if(CmdLookup::CHANNEL_SET_TYPES.at(command) == 'i')
    {
        return this->runCommand(this->buildCommand("snmpset",
                                                   CmdLookup::CHANNEL_SET_USERS.at(command),
                                                   this->buildSetChannelParameter(
                                                       CmdLookup::CHANNEL_SET_COMMANDS.at(command),
                                                       this->convertToUniversalChannel(board, channel),
                                                       "i", (int)value) ));
    }
    else
    {
        return this->runCommand(this->buildCommand("snmpset",
                                                   CmdLookup::CHANNEL_SET_USERS.at(command),
                                                   this->buildSetChannelParameter(
                                                       CmdLookup::CHANNEL_SET_COMMANDS.at(command),
                                                       this->convertToUniversalChannel(board, channel),
                                                       "F", (float)value) ));
    }
    
}

template<class Number>
std::string SnmpUtilControl::buildSetChannelParameter(const std::string& paramName,
                                                      const std::string& channel,
                                                      const std::string& type,
                                                      Number value) const
{
    std::ostringstream parameterBuilder;
    parameterBuilder << paramName << channel << " " << type << " " << value;
    return parameterBuilder.str();
}

template<class Number>
std::string SnmpUtilControl::buildSetGlobalParameter(const std::string& paramName,
                                                     const std::string& type,
                                                     Number value) const
{
    std::ostringstream parameterBuilder;
    parameterBuilder << paramName << " " << type << " " << value;
    return parameterBuilder.str();
}

}
#endif //ORCHID_SRC_SLOWCONTROLS_HVLIB_SNMPUTILCONTROL_H
