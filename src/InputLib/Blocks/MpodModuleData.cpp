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

void MpodModuleData::addRampSpeed(float input)
{
    this->rampSpeed.push_back(input);
}

void MpodModuleData::addMaxVoltage(float input)
{
    this->maxVoltage.push_back(input);
}

void MpodModuleData::addMaxCurrent(float input)
{
    this->maxCurrent.push_back(input);
}

bool MpodModuleData::validate()
{
    bool output = ( (board.size() >= 1) &&
                    (numChannels.size() >= 1) &&
                    (online.size() >= 1) &&
                    (rampSpeed.size() >= 1) &&
                    (maxVoltage.size() >= 1) &&
                    (maxCurrent.size() >= 1) );

    output = (output &&
              (board.size() == online.size()) &&
              (board.size() == numChannels.size()) &&
              (board.size() == rampSpeed.size()) &&
              (board.size() == maxVoltage.size()) &&
              (board.size() == maxCurrent.size()));

    for(int i=0; i<maxVoltage.size(); ++i)
    {
        if(maxVoltage[i] > 3000.0)
        {
            output = false;
        }
    }
    
    for(int i=0; i<maxCurrent.size(); ++i)
    {
        if(maxCurrent[i] > 2000.0)
        {
            output = false;
        }
    }
    
    return output;             
}

void MpodModuleData::printValidationErrors()
{
    if(!((board.size() >= 1) &&
         (numChannels.size() >= 1) &&
         (online.size() >= 1) &&
         (rampSpeed.size() >= 1) &&
         (maxVoltage.size() >= 1) &&
         (maxCurrent.size() >= 1)) )
    {
        std::cout << "There must be 1 or more entries for each parameter" <<std::endl;
    }
    
    if(!((board.size() == online.size()) &&
         (board.size() == numChannels.size()) &&
         (board.size() == rampSpeed.size()) &&
         (board.size() == maxVoltage.size()) &&
         (board.size() == maxCurrent.size())))
    {
        std::cout << "There must be an equal number of entries for each parameter" <<std::endl;
    }
    
    for(int i=0; i<maxVoltage.size(); ++i)
    {
        if(maxVoltage[i] > 3000.0)
        {
            std::cout << "The voltage on module " << i << " exceeds the 3000V limit" <<std::endl;
        }
    }
    
    for(int i=0; i<maxCurrent.size(); ++i)
    {
        if(maxCurrent[i] > 2000.0)
        {
            std::cout << "The maximum current on module " << i << " exceeds the 2mA limit" <<std::endl;
        }
    }
}

std::ostream& operator<<(std::ostream& os, MpodModuleData const& mmd)
{
    using std::setw;
    using std::setfill;
    using std::setprecision;
    using std::fixed;
    os << "Module #, # Channels, Active, Ramp speed (V/s), Max set voltage (V), Max set current (uA)\n";
    for(int i=0; i<mmd.board.size(); ++i)
    {
        os << setw(8)  << setfill(' ')                             << mmd.board[i]              << ", ";
        os << setw(10) << setfill(' ')                             << mmd.numChannels[i]        << ", ";
        os << setw(6)  << setfill(' ')                             << (mmd.online[i] ? "T":"F") << ", ";
        os << setw(16) << setfill(' ') << fixed << setprecision(1) << mmd.rampSpeed[i]          << ", ";
        os << setw(19) << setfill(' ') << fixed << setprecision(1) << mmd.maxVoltage[i]         << ", ";
        os << setw(20) << setfill(' ') << fixed << setprecision(1) << mmd.maxCurrent[i]         << "\n";
    }
    return os;
}

}
