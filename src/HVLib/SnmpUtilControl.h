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
    
    std::string snmpGlobalGet(MpodGlobalGetParam command);
    std::string snmpChannelGet(MpodChannelGetParam command, int board, int channel);
    std::string snmpChannelWalk(MpodChannelGetParam command);

    std::string snmpGlobalSet(MpodGlobalSetParam command, float value);
    std::string snmpGlobalSet(MpodGlobalSetParam command, double value);
    std::string snmpGlobalSet(MpodGlobalSetParam command, int value);
    std::string snmpChannelSet(MpodChannelSetParam command, int board, int channel, float value);
    std::string snmpChannelSet(MpodChannelSetParam command, int board, int channel, double value);
    std::string snmpChannelSet(MpodChannelSetParam command, int board, int channel, int value);

private:
    std::string buildCommand(const std::string& command, const std::string& user, const std::string& parameter);
    std::string runCommand(const std::string& command);
    std::string convertToUniversalChannel(int board, int channel);

    std::string ipAddress;
    std::string mibLocation;
};
#endif //ORCHID_SRC_HVLIB_SNMPUTILCONTROL_H
