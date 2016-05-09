/***************************************************************************//**
********************************************************************************
**
** @file MpodChannelData.h
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
** @details definition file that holds the class with MPOD per channel info
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHID_SRC_INPUTLIB_BLOCKS_MPODCHANNELDATA_H
#define ORCHID_SRC_INPUTLIB_BLOCKS_MPODCHANNELDATA_H

// includes for C system headers
// includes for C++ system headers
#include<vector>
#include<iostream>
// includes from other libraries
// includes from ORCHID

namespace InputParser
{


class MpodChannelData
{
public:
    // a vector for each column
    std::vector<int>   board;
    std::vector<int>   channel;
    std::vector<bool>  online;
    std::vector<float> rampUpRate;
    std::vector<float> rampDownRate;
    std::vector<float> voltage;
    std::vector<float> maxCurrent;
    std::vector<int>   currentTripTime;
    
    // now an adder for each vector
    void addBoard(int input);
    void addChannel(int input);
    void addOnline(bool input);
    void addRampUpRate(float input);
    void addRampDownRate(float input);
    void addVoltage(float input);
    void addMaxCurrent(float input);
    void addCurrentTripTime(int input);
    
    bool validate();
    void printValidationErrors();

    void sort();

    // ostream operator to allow outputting of the block class
    friend std::ostream& operator<<(std::ostream& os, MpodChannelData const& mcd);

};

}
#endif  // ORCHID_SRC_INPUTLIB_BLOCKS_MPODCHANNELDATA_H

