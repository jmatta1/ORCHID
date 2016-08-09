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
    std::vector<int>            moduleNumber;//1
    std::vector<int>            channelNumber;//2
    std::vector<bool>           channelEnable;//3
    std::vector<int>            recordLength;//4
    std::vector<bool>           largeRange;//5
    std::vector<int>            aggregateEvents;//6
    std::vector<int>            preTrigger;//7
    std::vector<int>            cfdDelay;//8
    std::vector<int>            cfdFraction;//9
    std::vector<int>            shortGate;//10
    std::vector<int>            longGate;//11
    std::vector<int>            gateOffset;//12
    std::vector<int>            trigThreshold;//13
    std::vector<int>            fixedBaseline;//14
    std::vector<int>            shapedTrigWidth;//15
    std::vector<int>            trigHoldOff;//16
    std::vector<int>            psdThreshold;//17
    std::vector<int>            chargeSensitivity;//18
    std::vector<bool>           chargePedestalOn;//19
    std::vector<int>            dppTriggerCounting;//20
    std::vector<int>            discMode;//21
    std::vector<int>            pulsePolarity;//22
    std::vector<int>            trigMode;//23
    std::vector<int>            baselineMean;//24
    std::vector<bool>           disableSelfTrigger;//25
    std::vector<bool>           psdCutBelowThresh;//26
    std::vector<bool>           psdCutAboveThresh;//27
    std::vector<bool>           overRangeRejection;//28
    std::vector<bool>           triggerHysteresis;//29
    std::vector<bool>           useLocalShapedTrig;//30
    std::vector<int>            localShapedTrigMode;//31
    std::vector<bool>           useLocalTrigVal;//32
    std::vector<int>            localTrigValMode;//33
    std::vector<bool>           localTrigValAsVeto;//34
    std::vector<int>            extrasWordOptions;//35
    std::vector<bool>           smoothIntegration;//36
    std::vector<int>            inputSmoothing;//37
    std::vector<int>            dcOffset;//38
    std::vector<int>            vetoDurationExtension;//39
    std::vector<unsigned int>   triggerValidMask;//40
    
    void addModuleNumber(int input);//1
    void addChannelNumber(int input);//2
    void addChannelEnable(bool input);//3
    void addRecordLength(int input);//4
    void addLargeRange(bool input);//5
    void addAggregateEvents(int input);//6
    void addPreTrigger(int input);//7
    void addCfdDelay(int input);//8
    void addCfdFraction(int input);//9
    void addShortGate(int input);//10
    void addLongGate(int input);//11
    void addGateOffset(int input);//12
    void addTrigThreshold(int input);//13
    void addFixedBaseline(int input);//14
    void addShapedTrigWidth(int input);//15
    void addTrigHoldOff(int input);//16
    void addPsdThreshold(int input);//17
    void addChargeSensitivity(int input);//18
    void addChargePedestalOn(bool input);//19
    void addDppTriggerCounting(int input);//20
    void addDiscMode(int input);//21
    void addPulsePolarity(int input);//22
    void addTrigMode(int input);//23
    void addBaselineMean(int input);//24
    void addDisableSelfTrigger(bool input);//25
    void addPsdCutBelowThresh(bool input);//26
    void addPsdCutAboveThresh(bool input);//27
    void addOverRangeRejection(bool input);//28
    void addTriggerHysteresis(bool input);//29
    void addUseLocalShapedTrig(bool input);//30
    void addLocalShapedTrigMode(int input);//31
    void addUseLocalTrigVal(bool input);//32
    void addLocalTrigValMode(int input);//33
    void addLocalTrigValAsVeto(bool input);//34
    void addExtrasWordOptions(int input);//35
    void addSmoothIntegration(bool input);//36
    void addInputSmoothing(int input);//37
    void addDcOffset(int input);//38
    void addVetoDurationExtension(int input);//39
    void addTriggerValidMask(unsigned int input);//40
    
/*
int moduleNumber; //1
int channelNumber;//2
bool channelEnable;//3
int recordLength;//4
bool largeRange;//5
int aggregateEvents;//6
int preTrigger;//7
int cfdDelay;//8
int cfdFraction;//9
int shortGate;//10
int longGate;//11
int gateOffset;//12
int trigThreshold;//13
int fixedBaseline;//14
int shapedTrigWidth;//15
int trigHoldOff;//16
int psdThreshold;//17
int chargeSensitivity;//18
bool chargePedestalOn;//19
int dppTriggerCounting;//20
int discMode;//21
int pulsePolarity;//22
int trigMode;//23
int baselineMean;//24
bool disableSelfTrigger;//25
bool psdCutBelowThresh;//26
bool psdCutAboveThresh;//27
bool overRangeRejection;//28
bool triggerHysteresis;//29
bool useLocalShapedTrig;//30
int localShapedTrigMode;//31
bool useLocalTrigVal;//32
int localTrigValMode;//33
bool localTrigValAsVeto;//34
int extrasWordOptions;//35
bool smoothIntegration;//36
int inputSmoothing;//37
int dcOffset;//38
int vetoDurationExtension;//39
int triggerValidMask;//40
*/
    
    bool validate();
    void printValidationErrors();
    
    void sort();
    
    // ostream operator to allow outputting of the block class
    friend std::ostream& operator<<(std::ostream& os, DigitizerChannelData const& mcd);
};

}

#endif //ORCHID_SRC_INPUTLIB_BLOCKS_DIGITIZERCHANNELDATA_H
