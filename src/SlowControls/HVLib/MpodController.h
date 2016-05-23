/***************************************************************************//**
********************************************************************************
**
** @file MpodController.h
** @author James Till Matta
** @date 09 May, 2016
** @brief
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** @details Definition file for a class which has functions to explicitly
**   set voltages and the like
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHID_SRC_HVLIB_MPODCONTROLLER_H
#define ORCHID_SRC_HVLIB_MPODCONTROLLER_H

// includes for C system headers
// includes for C++ system headers
// includes from other libraries
// includes from ORCHID
#include"SnmpUtilControl.h"
#include"InputLib/Blocks/MpodChannelData.h"
#include"InputLib/Blocks/MpodModuleData.h"
namespace SlowControls
{
using InputParser::MpodChannelData;
using InputParser::MpodModuleData;

class MpodController
{
public:
    //construction and destruction
    MpodController(SnmpUtilControl* snmpCtrl, MpodChannelData* chData,
                   MpodModuleData* modData) : snmpController(snmpCtrl),
                   channelData(chData), moduleData(modData) {}
    ~MpodController(){}
    
    //global modification functions
    void setCrateStatus(bool online);
    bool setupAllChannels(); //returns false if the crate is offline
    
    //individual module and channel modification functions
    bool setChannelStatus(int module, int channel, bool online); //returns false if the crate is offline
    bool setChannelVoltage(int module, int channel, float voltage); //returns false if the crate is offline
    bool setRiseRate(int module, int channel, float riseRate); //returns false if the crate is offline
    bool setFallRate(int module, int channel, float fallRate); //returns false if the crate is offline
    
    
private:
    //programs the various maximums into the system, called every time the crate
    //is turned on
    bool doMaxSetup(); //returns false if the crate is offline
    
    SnmpUtilControl* snmpController;
    MpodChannelData* channelData;
    MpodModuleData* moduleData;
};
}
#endif //ORCHID_SRC_HVLIB_MPODCONTROLLER_H

