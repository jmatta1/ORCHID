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

static const boost::posix_time::ptime Epoch(boost::posix_time::time_from_string("1970-01-01 00:00:00.000"));

void MpodReader::readAll()
{
    this->voltageData.beginRead = (boost::posix_time::microsec_clock::universal_time() - Epoch).total_microseconds();
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
    this->voltageData.finishRead = (boost::posix_time::microsec_clock::universal_time() - Epoch).total_microseconds();
}

void MpodReader::readActive()
{
    this->voltageData.beginRead = (boost::posix_time::microsec_clock::universal_time() - Epoch).total_microseconds();
    this->walkTerminalVoltages();
    this->walkSenseVoltages();
    this->walkCurrents();
    this->walkTemperatures();
    this->walkChannelStatuses();
    this->getCrateStatus();
    this->voltageData.finishRead = (boost::posix_time::microsec_clock::universal_time() - Epoch).total_microseconds();
}

}
