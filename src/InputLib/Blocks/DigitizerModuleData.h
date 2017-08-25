/***************************************************************************//**
********************************************************************************
**
** @file DigitizerModuleData.h
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
#ifndef ORCHID_SRC_INPUTLIB_BLOCKS_DIGITIZERMODULEDATA_H
#define ORCHID_SRC_INPUTLIB_BLOCKS_DIGITIZERMODULEDATA_H

// includes for C system headers
// includes for C++ system headers
#include<vector>
#include<iostream>
// includes from other libraries
// includes from ORCHID

namespace InputParser
{

enum class LinkType: char {USB, Optical, DirectOptical};

// stream operator to output the ProblemType enum in a pretty way
std::ostream& operator<<(std::ostream& os, LinkType const& value);

class DigitizerModuleData
{
public:
    std::vector<LinkType>       linkType;
    std::vector<int>            linkNumber;
    std::vector<int>            daisyChainNumber;
    std::vector<unsigned int>   vmeBaseAddr;
    std::vector<int>            trigOverlapHandling;
    std::vector<bool>           enableTestPattern;
    std::vector<int>            selfTrigPolarity;
    std::vector<int>            samplesPerEvent;
    std::vector<int>            trigCountMode;
    std::vector<int>            memFullMode;
    std::vector<int>            majorityCoincidenceWindow;
    std::vector<int>            majorityLevel;
    std::vector<int>            numPostTrigSamples;
    std::vector<int>            interruptEventCount;
    std::vector<int>            maxEventsPerBLT;

    void addLinkType(LinkType input);
    void addLinkNumber(int input);
    void addDaisyChainNumber(int input);
    void addVmeBaseAddr(unsigned int input);
    void addTrigOverlapHandling(int input);
    void addEnableTestPattern(bool input);
    void addSelfTrigPolarity(int input);
    void addSamplesPerEvent(int input);
    void addTrigCountMode(int input);
    void addMemFullMode(int input);
    void addMajorityCoincidenceWindow(int input);
    void addMajorityLevel(int input);
    void addNumPostTrigSamples(int input);
    void addInterruptEventCount(int input);
    void addMaxEventsPerBLT(int input);
    
    bool validate();
    void printValidationErrors();
    
    void sort();
    
    // ostream operator to allow outputting of the block class
    friend std::ostream& operator<<(std::ostream& os, DigitizerModuleData const& mcd);
};

}

#endif //ORCHID_SRC_INPUTLIB_BLOCKS_DIGITIZERMODULEDATA_H
