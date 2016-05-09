/***************************************************************************//**
********************************************************************************
**
** @file MpodChannelData.cpp
** @author James Till Matta
** @date 25 Apr, 2016
** @brief
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** @details implementation file that holds the class with MPOD per channel info
**
********************************************************************************
*******************************************************************************/

#include"InputLib/Blocks/MpodChannelData.h"
// includes for C system headers
// includes for C++ system headers
#include<iomanip>
// includes from other libraries
// includes from ORCHID
#include"Utility/SortPermutation.h"

namespace InputParser
{


void MpodChannelData::addBoard(int input)
{
    this->board.push_back(input);
}

void MpodChannelData::addChannel(int input)
{
    this->channel.push_back(input);
}

void MpodChannelData::addOnline(bool input)
{
    this->online.push_back(input);
}

void MpodChannelData::addRampUpRate(float input)
{
    this->rampUpRate.push_back(input);
}

void MpodChannelData::addRampDownRate(float input)
{
    this->rampDownRate.push_back(input);
}

void MpodChannelData::addVoltage(float input)
{
    this->voltage.push_back(input);
}

void MpodChannelData::addMaxCurrent(float input)
{
    this->maxCurrent.push_back(input);
}

void MpodChannelData::addCurrentTripTime(int input)
{
    this->currentTripTime.push_back(input);
}

bool MpodChannelData::validate()
{
    bool output = ( (board.size() >= 1) &&
                    (channel.size() >= 1) &&
                    (online.size() >= 1) &&
                    (rampUpRate.size() >= 1) &&
                    (rampDownRate.size() >= 1) &&
                    (voltage.size() >= 1) &&
                    (maxCurrent.size() >= 1) &&
                    (currentTripTime.size() >= 1) );

    output = (output &&
              (board.size() == channel.size()) &&
              (board.size() == online.size()) &&
              (board.size() == rampUpRate.size()) &&
              (board.size() == rampDownRate.size()) &&
              (board.size() == voltage.size()) &&
              (board.size() == maxCurrent.size()) &&
              (board.size() == currentTripTime.size()) );

    for(int i=0; i<voltage.size(); ++i)
    {
        if(voltage[i] > 3000.0)
        {
            output = false;
        }
    }
    
    for(int i=0; i<maxCurrent.size(); ++i)
    {
        if(maxCurrent[i] > 3000.0)
        {
            output = false;
        }
    }
    
    return output;             
}

void MpodChannelData::printValidationErrors()
{
    if(!((board.size() >= 1) &&
         (channel.size() >= 1) &&
         (online.size() >= 1) &&
         (rampUpRate.size() >= 1) &&
         (rampDownRate.size() >= 1) &&
         (voltage.size() >= 1) &&
         (maxCurrent.size() >= 1) &&
         (currentTripTime.size() >= 1)) )
    {
        std::cout << "There must be 1 or more entries for each parameter" <<std::endl;
    }
    
    if( !( (board.size() == channel.size()) &&
           (board.size() == online.size()) &&
           (board.size() == rampUpRate.size()) &&
           (board.size() == rampDownRate.size()) &&
           (board.size() == voltage.size()) &&
           (board.size() == maxCurrent.size()) &&
           (board.size() == currentTripTime.size()) ) )
    {
        std::cout << "There must be an equal number of entries for each parameter" <<std::endl;
    }
    
    for(int i=0; i<voltage.size(); ++i)
    {
        if(voltage[i] > 3000.0)
        {
            std::cout << "The voltage on channel " << i << " exceeds the 3000V limit" <<std::endl;
        }
    }
    
    for(int i=0; i<maxCurrent.size(); ++i)
    {
        if(maxCurrent[i] > 3000.0)
        {
            std::cout << "The maximum current on channel " << i << " exceeds the 3mA limit" <<std::endl;
        }
    }
}


void MpodChannelData::sort()
{
    TwoKeyIntCompare comp;
    std::vector<std::size_t> permutation = twoKeySortPermutation(board, channel, comp);
    board = applyPermutation(board, permutation);
    channel = applyPermutation(channel, permutation);
    online = applyPermutation(online, permutation);
    rampUpRate = applyPermutation(rampUpRate, permutation);
    rampDownRate = applyPermutation(rampDownRate, permutation);
    voltage = applyPermutation(voltage, permutation);
    maxCurrent = applyPermutation(maxCurrent, permutation);
    currentTripTime = applyPermutation(currentTripTime, permutation);
}

std::ostream& operator<<(std::ostream& os, MpodChannelData const& mcd)
{
    using std::setw;
    using std::setfill;
    using std::setprecision;
    using std::fixed;
    os << "Board, Chan, On, Rise Rate(V/s), Fall Rate(V/s), Voltage(V), Max I(uA), Trip Time(ms)\n";
    for(int i=0; i<mcd.channel.size(); ++i)
    {
        os << setw(5)  << setfill(' ')                             << mcd.board[i]              << ", ";
        os << setw(4)  << setfill(' ')                             << mcd.channel[i]            << ", ";
        os << setw(2)  << setfill(' ')                             << (mcd.online[i] ? "T":"F") << ", ";
        os << setw(14) << setfill(' ')                             << mcd.rampUpRate[i]         << ", ";
        os << setw(14) << setfill(' ')                             << mcd.rampDownRate[i]       << ", ";
        os << setw(10) << setfill(' ') << fixed << setprecision(1) << mcd.voltage[i]            << ", ";
        os << setw(9)  << setfill(' ') << fixed << setprecision(1) << mcd.maxCurrent[i]         << ", ";
        os << setw(13) << setfill(' ')                             << mcd.currentTripTime[i]    << "\n";
    }
    return os;
}

}
