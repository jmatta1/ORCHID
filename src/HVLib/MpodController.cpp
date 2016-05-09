/***************************************************************************//**
********************************************************************************
**
** @file MpodController.cpp
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
** @details Implementation file for a class which has functions to explicitly
**   set voltages and the like
**
********************************************************************************
*******************************************************************************/

#include"MpodController.h"
// includes for C system headers
// includes for C++ system headers
// includes from other libraries
// includes from ORCHID

void MpodController::setCrateStatus(bool online)
{
    
}

bool MpodController::setupAllChannels()
{
    
}

//individual module and channel modification functions
bool MpodController::setChannelStatus(int module, int channel, bool online)
{
    
}

bool MpodController::setChannelVoltage(int module, int channel, float voltage)
{
    
}

bool MpodController::setRiseRate(int module, int channel, float riseRate)
{
    
}

bool MpodController::setFallRate(int module, int channel, float fallRate)
{
    
}

bool MpodController::doMaxSetup()
{
    int numChannels = channelData->board.size();
    int boardIndex = 0;
    for(int i = 0; i < numChannels; ++i)
    {
        //first make certain that the given module is listed in the module data
        //file
    }
}
