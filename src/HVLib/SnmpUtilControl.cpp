/***************************************************************************//**
********************************************************************************
**
** @file SnmpUtilControl.cpp
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
** @details Implementation file for the class that runs the MPOD control stuff
**
********************************************************************************
*******************************************************************************/

#include"SnmpUtilControl.h"
// includes for C system headers
// includes for C++ system headers
#include<sstream>
#include<string>
#include<cstdio>
#include<memory>
#include<stdexcept>
#include<iostream>
// includes from other libraries
// includes from ORCHID

std::string SnmpUtilControl::snmpGlobalGet(MpodGlobalGetParam command)
{
    return this->runCommand(this->buildCommand("snmpget", "guru",
                                               CmdLookup::GLOBAL_GET_COMMANDS.at(command)));
}

std::string SnmpUtilControl::snmpChannelGet(MpodChannelGetParam command, int board, int channel)
{
    std::ostringstream parameterBuilder;
    parameterBuilder << CmdLookup::CHANNEL_GET_COMMANDS.at(command);
    parameterBuilder << this->convertToUniversalChannel(board, channel);
    return this->runCommand(this->buildCommand("snmpget", "guru",
                                               parameterBuilder.str()));
}

std::string SnmpUtilControl::snmpChannelWalk(MpodChannelGetParam command)
{
    return this->runCommand(this->buildCommand("snmpwalk", "guru",
                                               CmdLookup::CHANNEL_GET_COMMANDS.at(command)));
}

template<class Number>
std::string SnmpUtilControl::snmpGlobalSet(MpodGlobalSetParam command, Number value)
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
std::string SnmpUtilControl::snmpChannelSet(MpodChannelSetParam command, int board, int channel, Number value)
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
                                                      Number value)
{
    std::ostringstream parameterBuilder;
    parameterBuilder << paramName << channel << " " << type << " " << value;
    return parameterBuilder.str();
}

template<class Number>
std::string SnmpUtilControl::buildSetGlobalParameter(const std::string& paramName,
                                                     const std::string& type,
                                                     Number value)
{
    std::ostringstream parameterBuilder;
    parameterBuilder << paramName << " " << type << " " << value;
    return parameterBuilder.str();
}

std::string SnmpUtilControl::buildCommand(const std::string& command, const std::string& user, const std::string& parameter)
{
    std::ostringstream cmdBuilder;
    cmdBuilder << command << " -OeUv -v 2c -M " << this->mibLocation;
    cmdBuilder << " -m +WIENER-CRATE-MIB  -c "<< user << " " << this->ipAddress;
    cmdBuilder << " " << parameter;
    return cmdBuilder.str();
}


std::string SnmpUtilControl::runCommand(const std::string& command)
{
    char buffer[128];
    std::string result = "";
    std::shared_ptr<FILE> pipe(popen(command.c_str(), "r"), pclose);
    if (!pipe) throw std::runtime_error("popen() failed in SnmpUtilControl!");
    while (!feof(pipe.get()))
    {
        if(fgets(buffer, 128, pipe.get()) != NULL)
        {
            result.append(buffer);
        }
    }
    return result;
}

std::string SnmpUtilControl::convertToUniversalChannel(int board, int channel)
{
    if( (board < 1) || (board > 10) ) throw std::runtime_error("Invalid board number (not in the range [1, 10]");
    std::ostringstream channelNamer;
    channelNamer << ".u";
    if((board>=2))
    {
        channelNamer << (board-1);
    }
    channelNamer << (channel-1);
    return channelNamer.str();
}

