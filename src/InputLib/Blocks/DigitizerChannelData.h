/***************************************************************************//**
********************************************************************************
**
** @file DigitizerChannelData.h
** @author James Till Matta
** @date 01 Jul, 2016
** @brief
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** @details definition file that holds the class with digitizer per module config
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHID_SRC_INPUTLIB_BLOCKS_DIGITIZERCHANNELDATA_H
#define ORCHID_SRC_INPUTLIB_BLOCKS_DIGITIZERCHANNELDATA_H

// includes for C system headers
// includes for C++ system headers
#include<vector>
#include<iostream>
// includes from other libraries
// includes from ORCHID

namespace InputParser
{

class DigitizerChannelData
{
public:
    std::vector<int>    moduleNumber;//1
    std::vector<int>    channelNumber;//2
    std::vector<bool>   channelEnable;//3
    std::vector<int>   participateInTrigger;//4
    std::vector<bool>   largeRange;//5
    std::vector<int>    pulseWidth;//6
    std::vector<int>    trigThreshold;//7
    std::vector<int>    coupleTrigLogic;//8
    std::vector<int>    couplePulseType;//9
    std::vector<int>    dcOffset;//10
    
    void addModuleNumber(int input);//1
    void addChannelNumber(int input);//2
    void addChannelEnable(bool input);//3
    void addParticipateInTrigger(bool input);//4
    void addLargeRange(bool input);//5
    void addPulseWidth(int input);//6
    void addTrigThreshold(int input);//7
    void addCoupleTrigLogic(int input);//8
    void addCouplePulseType(int input);//9
    void addDcOffset(int input);//10
    
    bool validate();
    void printValidationErrors();
    
    void sort();
    
    // ostream operator to allow outputting of the block class
    friend std::ostream& operator<<(std::ostream& os, DigitizerChannelData const& mcd);
};

}

#endif //ORCHID_SRC_INPUTLIB_BLOCKS_DIGITIZERCHANNELDATA_H
