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

//TODO: Remove spoofing of MPOD connection from this class
std::string SnmpUtilControl::snmpGlobalGet(MpodGlobalGetParam command)
{
#ifdef SPOOF
    //TODO: Remove this spoof here
    switch(command)
    {
    case MpodGlobalGetParam::SysMainSwitch:
        return "INTEGER: 0";
        break;
    case MpodGlobalGetParam::SysStatus:
        return "BITS: 80 [.]0";
        break;
    }
#endif
    return this->runCommand(this->buildCommand("snmpget", "guru",
                                               CmdLookup::GLOBAL_GET_COMMANDS.at(command)));
}

std::string SnmpUtilControl::snmpChannelGet(MpodChannelGetParam command,
                                            int board, int channel)
{
#ifdef SPOOF
    //TODO: Remove this spoof here
    switch(command)
    {
    case MpodChannelGetParam::OutputStatus:
        if (channel < 4)
        {
            return "BITS: 80 01 80   [...]0 15 16 ";
        }
        else
        {
            return "BITS: 00 01   [..]15 ";
        }
        break;
    case MpodChannelGetParam::OutputSwitch:
        if (channel < 4)
        {
            return "INTEGER: 1";
        }
        else
        {
            return "INTEGER: 0";
        }
        break;
    case MpodChannelGetParam::SenseVoltage:
        if (channel < 4)
        {
            return "Opaque: Float: 1200.000244";
        }
        else
        {
            return "Opaque: Float: 0.078624";
        }
        break;
    case MpodChannelGetParam::TerminalVoltage:
        if (channel < 4)
        {
            return "Opaque: Float: 1199.999023";
        }
        else
        {
            return "Opaque: Float: 0.078624";
        }
        break;
    case MpodChannelGetParam::Temperature:
        return "INTEGER: 27";
        break;
    case MpodChannelGetParam::SetVoltage:
        if (channel < 4)
        {
            return "Opaque: Float: 1200.000000";
        }
        else
        {
            return "Opaque: Float: 0.000000";
        }
        break;
    case MpodChannelGetParam::Current:
        if (channel < 4)
        {
            return "Opaque: Float: 0.000250";
        }
        else
        {
            return "Opaque: Float: 0.000000";
        }
        break;
    case MpodChannelGetParam::RampUp:
        return "Opaque: Float: 30.000000";
        break;
    case MpodChannelGetParam::RampDown:
        return "Opaque: Float: 30.000000";
        break;
    case MpodChannelGetParam::MaxTerminalVoltage:
        return "Opaque: Float: 0.000000";
        break;
    case MpodChannelGetParam::MaxCurrent:
        return "Opaque: Float: 0.000000";
        break;
    case MpodChannelGetParam::SetCurrent:
        return "Opaque: Float: 0.003000";
        break;
    case MpodChannelGetParam::MaxCurrentTripTime:
        return "INTEGER: 500";
        break;
    }
#endif
    std::ostringstream parameterBuilder;
    parameterBuilder << CmdLookup::CHANNEL_GET_COMMANDS.at(command);
    parameterBuilder << this->convertToUniversalChannel(board, channel);
    return this->runCommand(this->buildCommand("snmpget", "guru",
                                               parameterBuilder.str()));
}


std::string SnmpUtilControl::snmpChannelWalk(MpodChannelGetParam command)
{
#ifdef SPOOF
    //TODO: Remove this spoof here
    std::string output;
    switch(command)
    {
    case MpodChannelGetParam::OutputStatus:
        output = "BITS: 80 01 80   [...]0 15 16 \nBITS: 80 01 80   [...]0 15 16 \nBITS: 80 01 80   [...]0 15 16 \nBITS: 80 01 80   [...]0 15 16 \n"
                 "BITS: 00 01   [..]15 \nBITS: 00 01   [..]15 \nBITS: 00 01   [..]15 \nBITS: 00 01   [..]15 \nBITS: 00 01   [..]15 \nBITS: 00 01   [..]15 \n"
                 "BITS: 00 01   [..]15 \nBITS: 00 01   [..]15 \nBITS: 00 01   [..]15 \nBITS: 00 01   [..]15 \nBITS: 00 01   [..]15 \nBITS: 00 01   [..]15 \n";
        return output;
        break;
    case MpodChannelGetParam::OutputSwitch:
        output = "INTEGER: 1\nINTEGER: 1\nINTEGER: 1\nINTEGER: 1\nINTEGER: 0\nINTEGER: 0\nINTEGER: 0\nINTEGER: 0\nINTEGER: 0\nINTEGER: 0\n"
                 "INTEGER: 0\nINTEGER: 0\nINTEGER: 0\nINTEGER: 0\nINTEGER: 0\nINTEGER: 0\n";
        return output;
        break;
    case MpodChannelGetParam::SenseVoltage:
        output = "Opaque: Float: 1200.000244\nOpaque: Float: 1200.000244\nOpaque: Float: 1200.000244\nOpaque: Float: 1200.000244\nOpaque: Float: -0.068010\n"
                 "Opaque: Float: -0.071796\nOpaque: Float: -0.090165\nOpaque: Float: -0.084424\nOpaque: Float: -0.073771\nOpaque: Float: -0.027393\n"
                 "Opaque: Float: 0.078624\nOpaque: Float: -0.057828\nOpaque: Float: 0.077518\nOpaque: Float: -0.037471\nOpaque: Float: 0.051563\n"
                 "Opaque: Float: -0.063784\n";
        return output;
        break;
    case MpodChannelGetParam::TerminalVoltage:
        output = "Opaque: Float: 1199.999023\nOpaque: Float: 1199.999023\nOpaque: Float: 1199.999023\nOpaque: Float: 1199.999023\nOpaque: Float: -0.068747\n"
                 "Opaque: Float: -0.081385\nOpaque: Float: -0.097542\nOpaque: Float: -0.081473\nOpaque: Float: -0.065654\nOpaque: Float: -0.027393\n"
                 "Opaque: Float: 0.078624\nOpaque: Float: -0.057828\nOpaque: Float: 0.077518\nOpaque: Float: -0.037471\nOpaque: Float: 0.051563\n"
                 "Opaque: Float: -0.063784\n";
        return output;
        break;
    case MpodChannelGetParam::Temperature:
        output = "INTEGER: 27\nINTEGER: 27\nINTEGER: 27\nINTEGER: 27\nINTEGER: 27\nINTEGER: 27\nINTEGER: 27\nINTEGER: 27\nINTEGER: 27\n"
                 "INTEGER: 27\nINTEGER: 27\nINTEGER: 27\nINTEGER: 27\nINTEGER: 27\nINTEGER: 27\nINTEGER: 27\n";
        return output;
        break;
    case MpodChannelGetParam::SetVoltage:
        output = "Opaque: Float: 1200.000000\nOpaque: Float: 1200.000000\nOpaque: Float: 1200.000000\nOpaque: Float: 1200.000000\nOpaque: Float: 0.000000\n"
                 "Opaque: Float: 0.000000\nOpaque: Float: 0.000000\nOpaque: Float: 0.000000\nOpaque: Float: 0.000000\nOpaque: Float: 0.000000\n"
                 "Opaque: Float: 0.000000\nOpaque: Float: 0.000000\nOpaque: Float: 0.000000\nOpaque: Float: 0.000000\nOpaque: Float: 0.000000\n"
                 "Opaque: Float: 0.000000\n";
        return output;
        break;
    case MpodChannelGetParam::Current:
        output = "Opaque: Float: 0.000250\nOpaque: Float: 0.000250\nOpaque: Float: 0.000250\nOpaque: Float: 0.000250\nOpaque: Float: 0.000000\n"
                 "Opaque: Float: 0.000000\nOpaque: Float: 0.000000\nOpaque: Float: 0.000000\nOpaque: Float: 0.000000\nOpaque: Float: 0.000000\n"
                 "Opaque: Float: 0.000000\nOpaque: Float: 0.000000\nOpaque: Float: 0.000000\nOpaque: Float: 0.000000\nOpaque: Float: 0.000000\n"
                 "Opaque: Float: 0.000000\n";
        return output;
        break;
    case MpodChannelGetParam::RampUp:
        output = "Opaque: Float: 30.000000\nOpaque: Float: 30.000000\nOpaque: Float: 30.000000\nOpaque: Float: 30.000000\nOpaque: Float: 30.000000\n"
                 "Opaque: Float: 30.000000\nOpaque: Float: 30.000000\nOpaque: Float: 30.000000\nOpaque: Float: 30.000000\nOpaque: Float: 30.000000\n"
                 "Opaque: Float: 30.000000\nOpaque: Float: 30.000000\nOpaque: Float: 30.000000\nOpaque: Float: 30.000000\nOpaque: Float: 30.000000\n"
                 "Opaque: Float: 30.000000\n";
        return output;
        break;
    case MpodChannelGetParam::RampDown:
        output = "Opaque: Float: 30.000000\nOpaque: Float: 30.000000\nOpaque: Float: 30.000000\nOpaque: Float: 30.000000\nOpaque: Float: 30.000000\n"
                 "Opaque: Float: 30.000000\nOpaque: Float: 30.000000\nOpaque: Float: 30.000000\nOpaque: Float: 30.000000\nOpaque: Float: 30.000000\n"
                 "Opaque: Float: 30.000000\nOpaque: Float: 30.000000\nOpaque: Float: 30.000000\nOpaque: Float: 30.000000\nOpaque: Float: 30.000000\n"
                 "Opaque: Float: 30.000000\n";
        return output;
        break;
    case MpodChannelGetParam::MaxTerminalVoltage:
        output = "Opaque: Float: 0.000000\nOpaque: Float: 0.000000\nOpaque: Float: 0.000000\nOpaque: Float: 0.000000\nOpaque: Float: 0.000000\n"
                 "Opaque: Float: 0.000000\nOpaque: Float: 0.000000\nOpaque: Float: 0.000000\nOpaque: Float: 0.000000\nOpaque: Float: 0.000000\n"
                 "Opaque: Float: 0.000000\nOpaque: Float: 0.000000\nOpaque: Float: 0.000000\nOpaque: Float: 0.000000\nOpaque: Float: 0.000000\n"
                 "Opaque: Float: 0.000000\n";
        return output;
        break;
    case MpodChannelGetParam::MaxCurrent:
        output = "Opaque: Float: 0.000000\nOpaque: Float: 0.000000\nOpaque: Float: 0.000000\nOpaque: Float: 0.000000\nOpaque: Float: 0.000000\n"
                 "Opaque: Float: 0.000000\nOpaque: Float: 0.000000\nOpaque: Float: 0.000000\nOpaque: Float: 0.000000\nOpaque: Float: 0.000000\n"
                 "Opaque: Float: 0.000000\nOpaque: Float: 0.000000\nOpaque: Float: 0.000000\nOpaque: Float: 0.000000\nOpaque: Float: 0.000000\n"
                 "Opaque: Float: 0.000000\n";
        return output;
        break;
    case MpodChannelGetParam::SetCurrent:
        output = "Opaque: Float: 0.003000\nOpaque: Float: 0.003000\nOpaque: Float: 0.003000\nOpaque: Float: 0.003000\nOpaque: Float: 0.003000\n"
                 "Opaque: Float: 0.003000\nOpaque: Float: 0.003000\nOpaque: Float: 0.003000\nOpaque: Float: 0.003000\nOpaque: Float: 0.003000\n"
                 "Opaque: Float: 0.003000\nOpaque: Float: 0.003000\nOpaque: Float: 0.003000\nOpaque: Float: 0.003000\nOpaque: Float: 0.003000\n"
                 "Opaque: Float: 0.003000\n";
        return output;
        break;
    case MpodChannelGetParam::MaxCurrentTripTime:
        output = "INTEGER: 500\nINTEGER: 500\nINTEGER: 500\nINTEGER: 500\nINTEGER: 500\nINTEGER: 500\nINTEGER: 500\nINTEGER: 500\nINTEGER: 500\n"
                 "INTEGER: 500\nINTEGER: 500\nINTEGER: 500\nINTEGER: 500\nINTEGER: 500\nINTEGER: 500\nINTEGER: 500\n";
        return output;
        break;
    }
#endif
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
    //open a pipe that takes the output of the snmp command to be run
    std::shared_ptr<FILE> pipe(popen(command.c_str(), "r"), pclose);
    if (!pipe)
    {
        BOOST_LOG_SEV(OrchidLog::get(), Critical) << "popen() failed in SnmpUtilControl!";
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
