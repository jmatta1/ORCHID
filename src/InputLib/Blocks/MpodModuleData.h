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
** @details definition file that holds the class with MPOD per module info
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHID_SRC_INPUTLIB_BLOCKS_MPODMODULEDATA_H
#define ORCHID_SRC_INPUTLIB_BLOCKS_MPODMODULEDATA_H

// includes for C system headers
// includes for C++ system headers
#include<vector>
#include<iostream>
// includes from other libraries
// includes from ORCHID

namespace InputParser
{


class MpodModuleData
{
public:
    // a vector for each column
    std::vector<int>   board;
    std::vector<int>   numChannels;
    std::vector<bool>  online;
    std::vector<float> maxRampUpSpeed;
    std::vector<float> maxRampDownSpeed;
    std::vector<float> maxSetVoltage;
    std::vector<float> maxSetCurrent;
    std::vector<int>   maxCurrentTripTime;
    
    // now an adder for each vector
    void addBoard(int input);
    void addNumChannels(int input);
    void addOnline(bool input);
    void addMaxRampUpSpeed(float input);
    void addMaxRampDownSpeed(float input);
    void addMaxSetVoltage(float input);
    void addMaxSetCurrent(float input);
    void addMaxCurrentTripTime(float input);
    
    bool validate();
    void printValidationErrors();

    // ostream operator to allow outputting of the block class
    friend std::ostream& operator<<(std::ostream& os, MpodModuleData const& mcd);

};

}
#endif  // ORCHID_SRC_INPUTLIB_BLOCKS_MPODMODULEDATA_H
