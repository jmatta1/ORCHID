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

#ifndef ORCHID_SRC_HVLIB_SNMPUTILCONTROL_H
#define ORCHID_SRC_HVLIB_SNMPUTILCONTROL_H

// includes for C system headers
// includes for C++ system headers
#include<string>
// includes from other libraries
// includes from ORCHID
#include"SnmpUtilCommands.h"

class SnmpUtilControl
{
public:
    SnmpUtilControl(const std::string& ip, const std::string& mibLoc):
        ipAddress(ip), mibLocation(mibLoc) {}
    ~SnmpUtilControl(){}
    
    std::string snmpGlobalGet(MpodGlobalGetParam command) const;
    std::string snmpChannelGet(MpodChannelGetParam command, int board, int channel) const;
    std::string snmpChannelWalk(MpodChannelGetParam command) const;

    
    template<class Number>
    std::string snmpGlobalSet(MpodGlobalSetParam command, Number value) const;
    template<class Number>
    std::string snmpChannelSet(MpodChannelSetParam command, int board, int channel, Number value) const;

private:
    template<class Number>
    std::string buildSetChannelParameter(const std::string& paramName, const std::string& channel, const std::string& type, Number value) const;
    template<class Number>
    std::string buildSetGlobalParameter(const std::string& paramName, const std::string& type, Number value) const;
    std::string buildCommand(const std::string& command, const std::string& user, const std::string& parameter) const;


    std::string convertToUniversalChannel(int board, int channel) const;
    std::string runCommand(const std::string& command) const;

    //member variables with data for creating commands
    std::string ipAddress;
    std::string mibLocation;
};
#endif //ORCHID_SRC_HVLIB_SNMPUTILCONTROL_H
