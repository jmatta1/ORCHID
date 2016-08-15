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
namespace SlowControls
{

bool MpodController::turnCrateOn()
{
    //TODO interpret return string
    this->snmpController->snmpGlobalSet(MpodGlobalSetParam::SysMainSwitch, 1);
    //insert this pause so we don't hit the crate 10 microseconds after it acks
    //with setup commands
    boost::this_thread::sleep_for(boost::chrono::seconds(3));
    return this->setupChannels();
}

bool MpodController::turnCrateOff()
{
    //TODO interpret return string
    this->snmpController->snmpGlobalSet(MpodGlobalSetParam::SysMainSwitch, 0);
    return true;
}

bool MpodController::activateAllChannels()
{
    int numBoards = this->moduleData->board.size();
    int channelIndexOffset = 0;
    for(int i=0; i < numBoards; ++i)
    {
        int numChannels = this->moduleData->numChannels[i];
        int maxChan = channelIndexOffset + numChannels;
        if(this->moduleData->online[i])
        {
            int board = this->moduleData->board[i];
            for(int j=channelIndexOffset; j < maxChan; ++j)
            {
                if(!(this->channelData->online[j])) continue;
                
                int channel = this->channelData->channel[j];
                //use the SNMP controller to turn on each channel
                //TODO interpret return string
                this->snmpController->snmpChannelSet(MpodChannelSetParam::OutputSwitch,
                                                     board, channel, 1);
            }
        }
        channelIndexOffset += numChannels;
    }
    return true;
}

bool MpodController::deactivateAllChannels()
{
    int numBoards = this->moduleData->board.size();
    int channelIndexOffset = 0;
    for(int i=0; i < numBoards; ++i)
    {
        int numChannels = this->moduleData->numChannels[i];
        int maxChan = channelIndexOffset + numChannels;
        if(this->moduleData->online[i])
        {
            int board = this->moduleData->board[i];
            for(int j=channelIndexOffset; j < maxChan; ++j)
            {
                if(!(this->channelData->online[j])) continue;
                
                int channel = this->channelData->channel[j];
                //use the SNMP controller to turn on each channel
                //TODO interpret return string
                this->snmpController->snmpChannelSet(MpodChannelSetParam::OutputSwitch,
                                                     board, channel, 0);
            }
        }
        channelIndexOffset += numChannels;
    }
    return true;
}

bool MpodController::setupChannels()
{
    int numBoards = this->moduleData->board.size();
    int channelIndexOffset = 0;
    for(int i=0; i < numBoards; ++i)
    {
        int numChannels = this->moduleData->numChannels[i];
        int maxChan = channelIndexOffset + numChannels;
        //BOOST_LOG_SEV(OrchidLog::get(), Information) << "Setting up board: " << i << " With "<< numChannels <<" channels";
        //BOOST_LOG_SEV(OrchidLog::get(), Information) << "Channels Offset is:" << channelIndexOffset;
        if(this->moduleData->online[i])
        {
            int board = this->moduleData->board[i];
            int boardMaxSetMaxCurrentTripTime   = this->moduleData->maxCurrentTripTime[i];
            float boardMaxSetMaxCurrent         = (this->moduleData->maxSetCurrent[i]/1000000.0);
            float boardMaxSetVoltage            = this->moduleData->maxSetVoltage[i];
            float boardMaxSetRampDownSpeed      = this->moduleData->maxRampDownSpeed[i];
            float boardMaxSetRampUpSpeed        = this->moduleData->maxRampUpSpeed[i];
            for(int j=channelIndexOffset; j < maxChan; ++j)
            {
                if(!(this->channelData->online[j])) continue;
                
                int setCurrentTripTime = this->channelData->currentTripTime[j];
                float setMaxCurrent = (this->channelData->maxCurrent[j]/1000000.0);
                float setVoltage = this->channelData->voltage[j];
                float setRampDownSpeed = this->channelData->rampDownRate[j];
                float setRampUpSpeed = this->channelData->rampUpRate[j];
                if(setCurrentTripTime   > boardMaxSetMaxCurrentTripTime)
                {
                    setCurrentTripTime  = boardMaxSetMaxCurrentTripTime;
                }
                if(setMaxCurrent        > boardMaxSetMaxCurrent)
                {
                    setMaxCurrent       = boardMaxSetMaxCurrent;
                }
                if(setVoltage           > boardMaxSetVoltage)
                {
                    setVoltage          = boardMaxSetVoltage;
                }
                if(setRampDownSpeed     > boardMaxSetRampDownSpeed)
                {
                    setRampDownSpeed    = boardMaxSetRampDownSpeed;
                }
                if(setRampUpSpeed       > boardMaxSetRampUpSpeed)
                {
                    setRampUpSpeed      = boardMaxSetRampUpSpeed;
                }
                int channel = this->channelData->channel[j];
                //now set each parameter using the SNMP controller
                //for now we disregard the return string
                //TODO: See if an operation suceeded by reading the return string
                this->snmpController->snmpChannelSet(MpodChannelSetParam::MaxCurrent,           board, channel, setMaxCurrent);
                this->snmpController->snmpChannelSet(MpodChannelSetParam::MaxCurrentTripTime,   board, channel, setCurrentTripTime);
                this->snmpController->snmpChannelSet(MpodChannelSetParam::MaxTerminalVoltage,   board, channel, boardMaxSetVoltage);
                this->snmpController->snmpChannelSet(MpodChannelSetParam::RampDown,             board, channel, setRampDownSpeed);
                this->snmpController->snmpChannelSet(MpodChannelSetParam::RampUp,               board, channel, setRampUpSpeed);
                this->snmpController->snmpChannelSet(MpodChannelSetParam::SetVoltage,           board, channel, setVoltage);
            }
        }
        channelIndexOffset += numChannels;
    }
    return true;
}

}
