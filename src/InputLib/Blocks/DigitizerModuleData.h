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
    std::vector<bool>           enableAutoFlush;
    std::vector<bool>           propogateTrigs;
    std::vector<bool>           recordWaveforms;
    std::vector<int>            analogProbe;
    std::vector<int>            dualTrace;
    std::vector<bool>           recExtrasWord;
    std::vector<int>            digVirtProbe1;
    std::vector<int>            digVirtProbe2;
    std::vector<int>            chanBuffPerAgg;
    std::vector<int>            triggerCountingMethod;
    std::vector<int>            memFullMode;
    std::vector<int>            pllRefClock;
    std::vector<int>            globalChanPairTrigMask;
    std::vector<int>            globalCoincidenceWindow;
    std::vector<int>            globalMajorityLevel;
    std::vector<bool>           externalTrigger;
    std::vector<int>            chanPairTrigOutMask;
    std::vector<int>            trigOutGenerationLogic;
    std::vector<int>            trigOutMajorityLevel;
    std::vector<int>            extTrigInTrigOut;
    std::vector<int>            memBuffAlmostFullLevel;
    std::vector<int>            runStartStopDelay;
    std::vector<bool>           useExtTrigger;
    std::vector<int>            interruptEventCount;
    std::vector<int>            aggregatesPerBlockTransfer;

    void addLinkType(LinkType input);
    void addLinkNumber(int input);
    void addDaisyChainNumber(int input);
    void addVmeBaseAddr(unsigned int input);
    void addEnableAutoFlush(bool input);
    void addPropogateTrigs(bool input);
    void addRecordWaveforms(bool input);
    void addAnalogProbe(int input);
    void addDualTrace(int input);
    void addRecExtrasWord(bool input);
    void addDigVirtProbe1(int input);
    void addDigVirtProbe2(int input);
    void addChanBuffPerAgg(int input);
    void addTriggerCountingMethod(int input);
    void addMemFullMode(int input);
    void addPllRefClock(int input);
    void addGlobalChanPairTrigMask(int input);
    void addGlobalCoincidenceWindow(int input);
    void addGlobalMajorityLevel(int input);
    void addExternalTrigger(bool input);
    void addChanPairTrigOutMask(int input);
    void addTrigOutGenerationLogic(int input);
    void addTrigOutMajorityLevel(int input);
    void addExtTrigInTrigOut(int input);
    void addMemBuffAlmostFullLevel(int input);
    void addRunStartStopDelay(int input);
    void addUseExtTrigger(bool input);
    void addInterruptEventCount(int input);
    void addAggregatesPerBlockTransfer(int input);
    
    bool validate();
    void printValidationErrors();
    
    void sort();
    
    // ostream operator to allow outputting of the block class
    friend std::ostream& operator<<(std::ostream& os, DigitizerModuleData const& mcd);
};

}

#endif //ORCHID_SRC_INPUTLIB_BLOCKS_DIGITIZERMODULEDATA_H
