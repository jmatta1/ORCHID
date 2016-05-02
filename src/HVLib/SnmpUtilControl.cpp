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
    return this->runCommand(this->buildCommand("snmpget",
                                               CmdLookup::GLOBAL_GET_COMMANDS.at(command)));
}

std::string SnmpUtilControl::snmpChannelGet(MpodChannelGetParam command, int board, int channel)
{
    std::ostringstream parameterBuilder;
    parameterBuilder << CmdLookup::CHANNEL_GET_COMMANDS.at(command);
    parameterBuilder << this->convertToUniversalChannel(board, channel);
    return this->runCommand(this->buildCommand("snmpget",
                                               parameterBuilder.str()));
}

std::string SnmpUtilControl::snmpChannelWalk(MpodChannelGetParam command)
{
    return this->runCommand(this->buildCommand("snmpwalk",
                                               CmdLookup::CHANNEL_GET_COMMANDS.at(command)));
}

std::string SnmpUtilControl::snmpGlobalSet(MpodGlobalSetParam command, float value)
{
    std::ostringstream parameterBuilder;
    parameterBuilder << CmdLookup::GLOBAL_SET_COMMANDS.at(command);
    parameterBuilder << " F " << value;
    return this->runCommand(this->buildCommand("snmpset",
                                               parameterBuilder.str()));
}

std::string SnmpUtilControl::snmpGlobalSet(MpodGlobalSetParam command, double value)
{
    std::ostringstream parameterBuilder;
    parameterBuilder << CmdLookup::GLOBAL_SET_COMMANDS.at(command);
    parameterBuilder << " F " << value;
    return this->runCommand(this->buildCommand("snmpset",
                                               parameterBuilder.str()));
}

std::string SnmpUtilControl::snmpGlobalSet(MpodGlobalSetParam command, int value)
{
    std::ostringstream parameterBuilder;
    parameterBuilder << CmdLookup::GLOBAL_SET_COMMANDS.at(command);
    parameterBuilder << " i " << value;
    return this->runCommand(this->buildCommand("snmpset",
                                               parameterBuilder.str()));
}

std::string SnmpUtilControl::snmpChannelSet(MpodChannelSetParam command, int board, int channel, float value)
{
    std::ostringstream parameterBuilder;
    parameterBuilder << CmdLookup::CHANNEL_SET_COMMANDS.at(command);
    parameterBuilder << this->convertToUniversalChannel(board, channel);
    parameterBuilder << " F " << value;
    return this->runCommand(this->buildCommand("snmpset",
                                               parameterBuilder.str()));
}

std::string SnmpUtilControl::snmpChannelSet(MpodChannelSetParam command, int board, int channel, double value)
{
    std::ostringstream parameterBuilder;
    parameterBuilder << CmdLookup::CHANNEL_SET_COMMANDS.at(command);
    parameterBuilder << this->convertToUniversalChannel(board, channel);
    parameterBuilder << " F " << value;
    return this->runCommand(this->buildCommand("snmpset",
                                               parameterBuilder.str()));
}

std::string SnmpUtilControl::snmpChannelSet(MpodChannelSetParam command, int board, int channel, int value)
{
    std::ostringstream parameterBuilder;
    parameterBuilder << CmdLookup::CHANNEL_SET_COMMANDS.at(command);
    parameterBuilder << this->convertToUniversalChannel(board, channel);
    parameterBuilder << " i " << value;
    return this->runCommand(this->buildCommand("snmpset",
                                               parameterBuilder.str()));
}

std::string SnmpUtilControl::buildCommand(const std::string& command, const std::string& parameter)
{
    std::ostringstream cmdBuilder;
    cmdBuilder << command << " -OeqUv -v 2c -M " << this->mibLocation;
    cmdBuilder << " -m +WIENER-CRATE-MIB  -c guru " << this->ipAddress;
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

