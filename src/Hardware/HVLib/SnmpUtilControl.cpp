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
#include<iomanip>
#include<cstdio>
#include<memory>
#include<stdexcept>
#include<iostream>
// includes from other libraries
// includes from ORCHID
#include"Utility/OrchidLogger.h"

namespace SlowControls
{

std::string SnmpUtilControl::snmpGlobalGet(MpodGlobalGetParam command)
{
    return this->runCommand(this->buildCommand("snmpget", "guru",
                                               CmdLookup::GLOBAL_GET_COMMANDS.at(command)));
}

std::string SnmpUtilControl::snmpChannelGet(MpodChannelGetParam command,
                                            int board, int channel)
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

std::string SnmpUtilControl::buildCommand(const std::string& command,
                                          const std::string& user,
                                          const std::string& parameter) const
{
    std::ostringstream cmdBuilder;
    cmdBuilder << command << " -OTeUv -v 2c -M " << this->mibLocation;
    cmdBuilder << " -m +WIENER-CRATE-MIB  -c "<< user << " " << this->ipAddress;
    //cmdBuilder << " " << parameter;
    cmdBuilder << " " << parameter << " 2>&1";
    return cmdBuilder.str();
}


std::string SnmpUtilControl::runCommand(const std::string& command)
{
    //prep the buffer that will hold direct pipe commands
    char buffer[128];
    //prep the string that will be returned as a result
    std::string result = "";
    //lock the command running mutex
    boost::lock_guard<boost::mutex> lock(this->commandMutex);
    BOOST_LOG_SEV(OrchidLog::get(), Information) << "command is: "<<command;
    //open a pipe that takes the output of the snmp command to be run
    std::shared_ptr<FILE> pipe(popen(command.c_str(), "r"), pclose);
    //FILE* pipe = popen(command.c_str(), "r");
    if (!pipe)
    {
        BOOST_LOG_SEV(OrchidLog::get(), Critical) << "popen() failed in SnmpUtilControl!";
        BOOST_LOG_SEV(OrchidLog::get(), Critical) << "Got the popen() error: " << strerror(errno);
        throw std::runtime_error("popen() failed in SnmpUtilControl!");
    }
    while (!feof(pipe.get()))
    {
        if(fgets(buffer, 128, pipe.get()) != NULL)
        {
            result.append(buffer);
        }
    }
    return result;
    //lock guard is destructed when this function returns, therefore the mutex
    // is unlocked when the function exits
}

std::string SnmpUtilControl::convertToUniversalChannel(int board,
                                                       int channel) const
{
    //TODO, support for multiple chained crates?
    if( (board < 0) || (board > 9) ) throw std::runtime_error("Invalid board number (not in the range [0, 9]");
    std::ostringstream channelNamer;
    channelNamer << ".u";
    if((board!=0))
    {
        channelNamer << board << std::setw(2) << std::setfill('0') << channel;
    }
    else
    {
        channelNamer << channel;;
    }
    return channelNamer.str();
}
}
