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
    std::vector<int> moduleNumber; //1
    std::vector<int> channelNumber;//2
    std::vector<bool> channelEnable;//3
    std::vector<int> recordLength;//4
    std::vector<bool> largeRange;//5
    std::vector<int> aggregateEvents;//6
    std::vector<int> preTrigger;//7
    std::vector<int> cfdDelay;//8
    std::vector<int> cfdFraction;//9
    std::vector<int> shortGate;//10
    std::vector<int> longGate;//11
    std::vector<int> gateOffset;//12
    std::vector<int> trigThreshold;//13
    std::vector<int> fixedBaseline;//14
    std::vector<int> shapedTrigWidth;//15
    std::vector<int> trigHoldOff;//16
    std::vector<int> psdThreshold;//17
    std::vector<int> chargeSensitivity;//18
    std::vector<bool> chargePedestalOn;//19
    std::vector<int> dppTriggerCounting;//20
    std::vector<int> discMode;//21
    std::vector<int> pulsePolarity;//22
    std::vector<int> trigMode;//23
    std::vector<int> baselineMean;//24
    std::vector<bool> disableSelfTrigger;//25
    std::vector<bool> psdCutBelowThresh;//26
    std::vector<bool> psdCutAboveThresh;//27
    std::vector<bool> overRangeRejection;//28
    std::vector<bool> triggerHysteresis;//29
    std::vector<bool> useLocalShapedTrig;//30
    std::vector<int> localShapedTrigMode;//31
    std::vector<bool> useLocalTrigVal;//32
    std::vector<int> localTrigValMode;//33
    std::vector<bool> localTrigValAsVeto;//34
    std::vector<int> extrasWordOptions;//35
    std::vector<bool> smoothIntegration;//36
    std::vector<int> inputSmoothing;//37
    std::vector<int> dcOffset;//38
    std::vector<int> vetoDurationExtension;//39
    std::vector<int> triggerValidMask;//40
    
    bool validate();
    void printValidationErrors();
    
    void sort();
    
    // ostream operator to allow outputting of the block class
    friend std::ostream& operator<<(std::ostream& os, DigitizerChannelData const& mcd);
};

}

#endif //ORCHID_SRC_INPUTLIB_BLOCKS_DIGITIZERCHANNELDATA_H
