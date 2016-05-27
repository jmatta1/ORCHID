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
#ifndef ORCHID_SRC_SLOWCONTROLS_HVLIB_MPODCONTROLLER_H
#define ORCHID_SRC_SLOWCONTROLS_HVLIB_MPODCONTROLLER_H

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
    bool turnCrateOn();
    bool turnCrateOff();
    bool activateAllChannels();
    bool deactivateAllChannels();
    
    
private:
    //programs the various maximums into the system, called every time the crate
    //is turned on
    bool setupChannels();
    
    SnmpUtilControl* snmpController;
    MpodChannelData* channelData;
    MpodModuleData* moduleData;
};
}
#endif //ORCHID_SRC_SLOWCONTROLS_HVLIB_MPODCONTROLLER_H

