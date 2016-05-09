/***************************************************************************//**
********************************************************************************
**
** @file MpodModuleData.h
** @author James Till Matta
** @date 27 Apr, 2016
** @brief
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** @details Implementation file that holds the class with MPOD per module info
**
********************************************************************************
*******************************************************************************/

#include"InputLib/Blocks/MpodModuleData.h"
// includes for C system headers
// includes for C++ system headers
#include<iomanip>
// includes from other libraries
// includes from ORCHID
#include"Utility/SortPermutation.h"

namespace InputParser
{


void MpodModuleData::addBoard(int input)
{
    this->board.push_back(input);
}

void MpodModuleData::addNumChannels(int input)
{
    this->numChannels.push_back(input);
}

void MpodModuleData::addOnline(bool input)
{
    this->online.push_back(input);
}

void MpodModuleData::addMaxRampUpSpeed(float input)
{
    this->maxRampUpSpeed.push_back(input);
}

void MpodModuleData::addMaxRampDownSpeed(float input)
{
    this->maxRampDownSpeed.push_back(input);
}

void MpodModuleData::addMaxSetVoltage(float input)
{
    this->maxSetVoltage.push_back(input);
}

void MpodModuleData::addMaxSetCurrent(float input)
{
    this->maxSetCurrent.push_back(input);
}

void MpodModuleData::addMaxCurrentTripTime(float input)
{
    this->maxCurrentTripTime.push_back(input);
}

bool MpodModuleData::validate()
{
    bool output = ( (board.size() >= 1) &&
                    (numChannels.size() >= 1) &&
                    (online.size() >= 1) &&
                    (maxRampUpSpeed.size() >= 1) &&
                    (maxRampDownSpeed.size() >= 1) &&
                    (maxSetVoltage.size() >= 1) &&
                    (maxSetCurrent.size() >= 1) &&
                    (maxCurrentTripTime.size() >= 1));

    output = (output &&
              (board.size() == online.size()) &&
              (board.size() == numChannels.size()) &&
              (board.size() == maxRampUpSpeed.size()) &&
              (board.size() == maxRampDownSpeed.size()) &&
              (board.size() == maxSetVoltage.size()) &&
              (board.size() == maxSetCurrent.size()) &&
              (board.size() == maxCurrentTripTime.size()));

    for(int i=0; i<maxSetVoltage.size(); ++i)
    {
        if(maxSetVoltage[i] > 3000.0)
        {
            output = false;
        }
    }
    
    for(int i=0; i<maxSetCurrent.size(); ++i)
    {
        if(maxSetCurrent[i] > 2000.0)
        {
            output = false;
        }
    }
    
    for(int i=0; i<maxRampUpSpeed.size(); ++i)
    {
        if(maxRampUpSpeed[i] > 150.0)
        {
            output = false;
        }
    }
    
    for(int i=0; i<maxRampDownSpeed.size(); ++i)
    {
        if(maxRampDownSpeed[i] > 150.0)
        {
            output = false;
        }
    }
    
    for(int i=0; i<maxCurrentTripTime.size(); ++i)
    {
        if(maxCurrentTripTime[i] > 500)
        {
            output = false;
        }
    }
    
    return output;             
}

void MpodModuleData::printValidationErrors()
{
    if(!( (board.size() >= 1) &&
          (numChannels.size() >= 1) &&
          (online.size() >= 1) &&
          (maxRampUpSpeed.size() >= 1) &&
          (maxRampDownSpeed.size() >= 1) &&
          (maxSetVoltage.size() >= 1) &&
          (maxSetCurrent.size() >= 1) &&
          (maxCurrentTripTime.size() >= 1) ) )
    {
        std::cout << "There must be 1 or more entries for each parameter" <<std::endl;
    }
    
    if(!(board.size() == online.size()) &&
        (board.size() == numChannels.size()) &&
        (board.size() == maxRampUpSpeed.size()) &&
        (board.size() == maxRampDownSpeed.size()) &&
        (board.size() == maxSetVoltage.size()) &&
        (board.size() == maxSetCurrent.size()) &&
        (board.size() == maxCurrentTripTime.size()))
    {
        std::cout << "There must be an equal number of entries for each parameter" <<std::endl;
    }
    
    for(int i=0; i<maxSetVoltage.size(); ++i)
    {
        if(maxSetVoltage[i] > 3000.0)
        {
            std::cout << "The voltage on module " << i << " exceeds the 3000V limit" <<std::endl;
        }
    }
    
    for(int i=0; i<maxSetCurrent.size(); ++i)
    {
        if(maxSetCurrent[i] > 2000.0)
        {
            std::cout << "The maximum current on module " << i << " exceeds the 2mA limit" <<std::endl;
        }
    }
    
    for(int i=0; i<maxRampUpSpeed.size(); ++i)
    {
        if(maxRampUpSpeed[i] > 150.0)
        {
            std::cout << "The maximum ramp up speed on module " << i << " exceeds the 150V/s limit" <<std::endl;
        }
    }
    
    for(int i=0; i<maxRampDownSpeed.size(); ++i)
    {
        if(maxRampDownSpeed[i] > 150.0)
        {
            std::cout << "The maximum ramp down speed on module " << i << " exceeds the 150V/s limit" <<std::endl;
        }
    }
    
    for(int i=0; i<maxCurrentTripTime.size(); ++i)
    {
        if(maxCurrentTripTime[i] > 500)
        {
            std::cout << "The maximum current trip time on module " << i << " exceeds the 500ms limit" <<std::endl;
        }
    }
}

void MpodModuleData::sort()
{
    OneKeyIntCompare comp;
    std::vector<std::size_t> permutation = oneKeySortPermutation(board, comp);
    board              = applyPermutation(board, permutation);
    numChannels        = applyPermutation(numChannels, permutation);
    online             = applyPermutation(online, permutation);
    maxRampUpSpeed     = applyPermutation(maxRampUpSpeed, permutation);
    maxRampDownSpeed   = applyPermutation(maxRampDownSpeed, permutation);
    maxSetVoltage      = applyPermutation(maxSetVoltage, permutation);
    maxSetCurrent      = applyPermutation(maxSetCurrent, permutation);
    maxCurrentTripTime = applyPermutation(maxCurrentTripTime, permutation);
}

std::ostream& operator<<(std::ostream& os, MpodModuleData const& mmd)
{
    using std::setw;
    using std::setfill;
    using std::setprecision;
    using std::fixed;
    os << "Module, Channels, On, Max Rise(V/s), Max Fall(V/s), Max Set V(V), Max Set I(uA), Max Trip Time(ms)\n";
    for(int i=0; i<mmd.board.size(); ++i)
    {
        os << setw(6)  << setfill(' ')                             << mmd.board[i]              << ", ";
        os << setw(8)  << setfill(' ')                             << mmd.numChannels[i]        << ", ";
        os << setw(2)  << setfill(' ')                             << (mmd.online[i] ? "T":"F") << ", ";
        os << setw(13) << setfill(' ') << fixed << setprecision(1) << mmd.maxRampUpSpeed[i]     << ", ";
        os << setw(13) << setfill(' ') << fixed << setprecision(1) << mmd.maxRampDownSpeed[i]   << ", ";
        os << setw(12) << setfill(' ') << fixed << setprecision(1) << mmd.maxSetVoltage[i]      << ", ";
        os << setw(13) << setfill(' ') << fixed << setprecision(1) << mmd.maxSetCurrent[i]      << ", ";
        os << setw(17) << setfill(' ') << fixed << setprecision(1) << mmd.maxCurrentTripTime[i] << "\n";
    }
    return os;
}

}
