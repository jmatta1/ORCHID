/***************************************************************************//**
********************************************************************************
**
** @file MpodReader.h
** @author James Till Matta
** @date 31 May, 2016
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

#include"MpodReader.h"
// includes for C system headers
// includes for C++ system headers
// includes from other libraries
// includes from ORCHID
#include"InputLib/Blocks/MpodChannelData.h"
namespace SlowControls
{

void MpodReader::readAll()
{
    this->walkTerminalVoltages();
    this->walkSenseVoltages();
    this->walkSetVoltages();
    this->walkCurrents();
    this->walkRampUpRates();
    this->walkRampDownRates();
    this->walkMaxCurrents();
    this->walkMaxVoltages();
    this->walkCurrentTripTimes();
    this->walkTemperatures();
    this->walkOutputSwitches();
    this->walkChannelStatuses();
    this->getCrateStatus();
}

void MpodReader::readActive()
{
    this->walkTerminalVoltages();
    this->walkSenseVoltages();
    this->walkCurrents();
    this->walkTemperatures();
    this->walkChannelStatuses();
    this->getCrateStatus();
}

}
