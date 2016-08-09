/***************************************************************************//**
********************************************************************************
**
** @file DigitizerChannelData.cpp
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
** @details definition file that holds the class with digitizer per channel cfg
**
********************************************************************************
*******************************************************************************/

#include"DigitizerChannelData.h"
// includes for C system headers
// includes for C++ system headers
#include<iomanip>
// includes from other libraries
// includes from ORCHID
#include"Utility/SortPermutation.h"

namespace InputParser
{

void DigitizerChannelData::addModuleNumber(int input)
{
    this->moduleNumber.push_back(input);
}

void DigitizerChannelData::addChannelNumber(int input)
{
    this->channelNumber.push_back(input);
}

void DigitizerChannelData::addChannelEnable(bool input)
{
    this->channelEnable.push_back(input);
}

void DigitizerChannelData::addRecordLength(int input)
{
    this->recordLength.push_back(input);
}

void DigitizerChannelData::addLargeRange(bool input)
{
    this->largeRange.push_back(input);
}

void DigitizerChannelData::addAggregateEvents(int input)
{
    this->aggregateEvents.push_back(input);
}

void DigitizerChannelData::addPreTrigger(int input)
{
    this->preTrigger.push_back(input);
}

void DigitizerChannelData::addCfdDelay(int input)
{
    this->cfdDelay.push_back(input);
}

void DigitizerChannelData::addCfdFraction(int input)
{
    this->cfdFraction.push_back(input);
}

void DigitizerChannelData::addShortGate(int input)
{
    this->shortGate.push_back(input);
}

void DigitizerChannelData::addLongGate(int input)
{
    this->longGate.push_back(input);
}

void DigitizerChannelData::addGateOffset(int input)
{
    this->gateOffset.push_back(input);
}

void DigitizerChannelData::addTrigThreshold(int input)
{
    this->trigThreshold.push_back(input);
}

void DigitizerChannelData::addFixedBaseline(int input)
{
    this->fixedBaseline.push_back(input);
}

void DigitizerChannelData::addShapedTrigWidth(int input)
{
    this->shapedTrigWidth.push_back(input);
}

void DigitizerChannelData::addTrigHoldOff(int input)
{
    this->trigHoldOff.push_back(input);
}

void DigitizerChannelData::addPsdThreshold(int input)
{
    this->psdThreshold.push_back(input);
}

void DigitizerChannelData::addChargeSensitivity(int input)
{
    this->chargeSensitivity.push_back(input);
}

void DigitizerChannelData::addChargePedestalOn(bool input)
{
    this->chargePedestalOn.push_back(input);
}

void DigitizerChannelData::addDppTriggerCounting(int input)
{
    this->dppTriggerCounting.push_back(input);
}

void DigitizerChannelData::addDiscMode(int input)
{
    this->discMode.push_back(input);
}

void DigitizerChannelData::addPulsePolarity(int input)
{
    this->pulsePolarity.push_back(input);
}

void DigitizerChannelData::addTrigMode(int input)
{
    this->trigMode.push_back(input);
}

void DigitizerChannelData::addBaselineMean(int input)
{
    this->baselineMean.push_back(input);
}

void DigitizerChannelData::addDisableSelfTrigger(bool input)
{
    this->disableSelfTrigger.push_back(input);
}

void DigitizerChannelData::addPsdCutBelowThresh(bool input)
{
    this->psdCutBelowThresh.push_back(input);
}

void DigitizerChannelData::addPsdCutAboveThresh(bool input)
{
    this->psdCutAboveThresh.push_back(input);
}

void DigitizerChannelData::addOverRangeRejection(bool input)
{
    this->overRangeRejection.push_back(input);
}

void DigitizerChannelData::addTriggerHysteresis(bool input)
{
    this->triggerHysteresis.push_back(input);
}

void DigitizerChannelData::addUseLocalShapedTrig(bool input)
{
    this->useLocalShapedTrig.push_back(input);
}

void DigitizerChannelData::addLocalShapedTrigMode(int input)
{
    this->localShapedTrigMode.push_back(input);
}

void DigitizerChannelData::addUseLocalTrigVal(bool input)
{
    this->useLocalTrigVal.push_back(input);
}

void DigitizerChannelData::addLocalTrigValMode(int input)
{
    this->localTrigValMode.push_back(input);
}

void DigitizerChannelData::addLocalTrigValAsVeto(bool input)
{
    this->localTrigValAsVeto.push_back(input);
}

void DigitizerChannelData::addExtrasWordOptions(int input)
{
    this->extrasWordOptions.push_back(input);
}

void DigitizerChannelData::addSmoothIntegration(bool input)
{
    this->smoothIntegration.push_back(input);
}

void DigitizerChannelData::addInputSmoothing(int input)
{
    this->inputSmoothing.push_back(input);
}

void DigitizerChannelData::addDcOffset(int input)
{
    this->dcOffset.push_back(input);
}

void DigitizerChannelData::addVetoDurationExtension(int input)
{
    this->vetoDurationExtension.push_back(input);
}

void DigitizerChannelData::addTriggerValidMask(unsigned int input)
{
    this->triggerValidMask.push_back(input);
}

bool DigitizerChannelData::validate()
{
    bool output = true;
    if(!(   (moduleNumber.size() >= 1) &&
            (channelNumber.size() >= 1) &&
            (channelEnable.size() >= 1) &&
            (recordLength.size() >= 1) &&
            (largeRange.size() >= 1) &&
            (aggregateEvents.size() >= 1) &&
            (preTrigger.size() >= 1) &&
            (cfdDelay.size() >= 1) &&
            (cfdFraction.size() >= 1) &&
            (shortGate.size() >= 1) &&
            (longGate.size() >= 1) &&
            (gateOffset.size() >= 1) &&
            (trigThreshold.size() >= 1) &&
            (fixedBaseline.size() >= 1) &&
            (shapedTrigWidth.size() >= 1) &&
            (trigHoldOff.size() >= 1) &&
            (psdThreshold.size() >= 1) &&
            (chargeSensitivity.size() >= 1) &&
            (chargePedestalOn.size() >= 1) &&
            (dppTriggerCounting.size() >= 1) &&
            (discMode.size() >= 1) &&
            (pulsePolarity.size() >= 1) &&
            (trigMode.size() >= 1) &&
            (baselineMean.size() >= 1) &&
            (disableSelfTrigger.size() >= 1) &&
            (psdCutBelowThresh.size() >= 1) &&
            (psdCutAboveThresh.size() >= 1) &&
            (overRangeRejection.size() >= 1) &&
            (triggerHysteresis.size() >= 1) &&
            (useLocalShapedTrig.size() >= 1) &&
            (localShapedTrigMode.size() >= 1) &&
            (useLocalTrigVal.size() >= 1) &&
            (localTrigValMode.size() >= 1) &&
            (localTrigValAsVeto.size() >= 1) &&
            (extrasWordOptions.size() >= 1) &&
            (smoothIntegration.size() >= 1) &&
            (inputSmoothing.size() >= 1) &&
            (dcOffset.size() >= 1) &&
            (vetoDurationExtension.size() >= 1) &&
            (triggerValidMask.size() >= 1)  ))
    {
        output = false;
    }
    int rows = moduleNumber.size();
    if(!(   (rows == channelNumber.size()) &&
            (rows == channelEnable.size()) &&
            (rows == recordLength.size()) &&
            (rows == largeRange.size()) &&
            (rows == aggregateEvents.size()) &&
            (rows == preTrigger.size()) &&
            (rows == cfdDelay.size()) &&
            (rows == cfdFraction.size()) &&
            (rows == shortGate.size()) &&
            (rows == longGate.size()) &&
            (rows == gateOffset.size()) &&
            (rows == trigThreshold.size()) &&
            (rows == fixedBaseline.size()) &&
            (rows == shapedTrigWidth.size()) &&
            (rows == trigHoldOff.size()) &&
            (rows == psdThreshold.size()) &&
            (rows == chargeSensitivity.size()) &&
            (rows == chargePedestalOn.size()) &&
            (rows == dppTriggerCounting.size()) &&
            (rows == discMode.size()) &&
            (rows == pulsePolarity.size()) &&
            (rows == trigMode.size()) &&
            (rows == baselineMean.size()) &&
            (rows == disableSelfTrigger.size()) &&
            (rows == psdCutBelowThresh.size()) &&
            (rows == psdCutAboveThresh.size()) &&
            (rows == overRangeRejection.size()) &&
            (rows == triggerHysteresis.size()) &&
            (rows == useLocalShapedTrig.size()) &&
            (rows == localShapedTrigMode.size()) &&
            (rows == useLocalTrigVal.size()) &&
            (rows == localTrigValMode.size()) &&
            (rows == localTrigValAsVeto.size()) &&
            (rows == extrasWordOptions.size()) &&
            (rows == smoothIntegration.size()) &&
            (rows == inputSmoothing.size()) &&
            (rows == dcOffset.size()) &&
            (rows == vetoDurationExtension.size()) &&
            (rows == triggerValidMask.size())     ))
    {
        output = false;
    }
    //TODO: Add substantially more validation tests here
    return output;
}

void DigitizerChannelData::printValidationErrors()
{
    if(!(   (moduleNumber.size() >= 1) &&
            (channelNumber.size() >= 1) &&
            (channelEnable.size() >= 1) &&
            (recordLength.size() >= 1) &&
            (largeRange.size() >= 1) &&
            (aggregateEvents.size() >= 1) &&
            (preTrigger.size() >= 1) &&
            (cfdDelay.size() >= 1) &&
            (cfdFraction.size() >= 1) &&
            (shortGate.size() >= 1) &&
            (longGate.size() >= 1) &&
            (gateOffset.size() >= 1) &&
            (trigThreshold.size() >= 1) &&
            (fixedBaseline.size() >= 1) &&
            (shapedTrigWidth.size() >= 1) &&
            (trigHoldOff.size() >= 1) &&
            (psdThreshold.size() >= 1) &&
            (chargeSensitivity.size() >= 1) &&
            (chargePedestalOn.size() >= 1) &&
            (dppTriggerCounting.size() >= 1) &&
            (discMode.size() >= 1) &&
            (pulsePolarity.size() >= 1) &&
            (trigMode.size() >= 1) &&
            (baselineMean.size() >= 1) &&
            (disableSelfTrigger.size() >= 1) &&
            (psdCutBelowThresh.size() >= 1) &&
            (psdCutAboveThresh.size() >= 1) &&
            (overRangeRejection.size() >= 1) &&
            (triggerHysteresis.size() >= 1) &&
            (useLocalShapedTrig.size() >= 1) &&
            (localShapedTrigMode.size() >= 1) &&
            (useLocalTrigVal.size() >= 1) &&
            (localTrigValMode.size() >= 1) &&
            (localTrigValAsVeto.size() >= 1) &&
            (extrasWordOptions.size() >= 1) &&
            (smoothIntegration.size() >= 1) &&
            (inputSmoothing.size() >= 1) &&
            (dcOffset.size() >= 1) &&
            (vetoDurationExtension.size() >= 1) &&
            (triggerValidMask.size() >= 1)  ))
    {
        std::cout << "There must be 1 or more entries for each parameter" <<std::endl;
    }
    int rows = moduleNumber.size();
    if(!(   (rows == channelNumber.size()) &&
            (rows == channelEnable.size()) &&
            (rows == recordLength.size()) &&
            (rows == largeRange.size()) &&
            (rows == aggregateEvents.size()) &&
            (rows == preTrigger.size()) &&
            (rows == cfdDelay.size()) &&
            (rows == cfdFraction.size()) &&
            (rows == shortGate.size()) &&
            (rows == longGate.size()) &&
            (rows == gateOffset.size()) &&
            (rows == trigThreshold.size()) &&
            (rows == fixedBaseline.size()) &&
            (rows == shapedTrigWidth.size()) &&
            (rows == trigHoldOff.size()) &&
            (rows == psdThreshold.size()) &&
            (rows == chargeSensitivity.size()) &&
            (rows == chargePedestalOn.size()) &&
            (rows == dppTriggerCounting.size()) &&
            (rows == discMode.size()) &&
            (rows == pulsePolarity.size()) &&
            (rows == trigMode.size()) &&
            (rows == baselineMean.size()) &&
            (rows == disableSelfTrigger.size()) &&
            (rows == psdCutBelowThresh.size()) &&
            (rows == psdCutAboveThresh.size()) &&
            (rows == overRangeRejection.size()) &&
            (rows == triggerHysteresis.size()) &&
            (rows == useLocalShapedTrig.size()) &&
            (rows == localShapedTrigMode.size()) &&
            (rows == useLocalTrigVal.size()) &&
            (rows == localTrigValMode.size()) &&
            (rows == localTrigValAsVeto.size()) &&
            (rows == extrasWordOptions.size()) &&
            (rows == smoothIntegration.size()) &&
            (rows == inputSmoothing.size()) &&
            (rows == dcOffset.size()) &&
            (rows == vetoDurationExtension.size()) &&
            (rows == triggerValidMask.size())     ))
    {
        std::cout << "There must the same number of rows for every column" <<std::endl;
    }
    //TODO: Add substantially more validation tests here
}

void DigitizerChannelData::sort()
{
    Utility::TwoKeyIntCompare comp;
    std::vector<std::size_t> permutation = Utility::twoKeySortPermutation(moduleNumber, channelNumber, comp);

    moduleNumber = Utility::applyPermutation(moduleNumber, permutation);
    channelNumber = Utility::applyPermutation(channelNumber, permutation);
    channelEnable = Utility::applyPermutation(channelEnable, permutation);
    recordLength = Utility::applyPermutation(recordLength, permutation);
    largeRange = Utility::applyPermutation(largeRange, permutation);
    aggregateEvents = Utility::applyPermutation(aggregateEvents, permutation);
    preTrigger = Utility::applyPermutation(preTrigger, permutation);
    cfdDelay = Utility::applyPermutation(cfdDelay, permutation);
    cfdFraction = Utility::applyPermutation(cfdFraction, permutation);
    shortGate = Utility::applyPermutation(shortGate, permutation);
    longGate = Utility::applyPermutation(longGate, permutation);
    gateOffset = Utility::applyPermutation(gateOffset, permutation);
    trigThreshold = Utility::applyPermutation(trigThreshold, permutation);
    fixedBaseline = Utility::applyPermutation(fixedBaseline, permutation);
    shapedTrigWidth = Utility::applyPermutation(shapedTrigWidth, permutation);
    trigHoldOff = Utility::applyPermutation(trigHoldOff, permutation);
    psdThreshold = Utility::applyPermutation(psdThreshold, permutation);
    chargeSensitivity = Utility::applyPermutation(chargeSensitivity, permutation);
    chargePedestalOn = Utility::applyPermutation(chargePedestalOn, permutation);
    dppTriggerCounting = Utility::applyPermutation(dppTriggerCounting, permutation);
    discMode = Utility::applyPermutation(discMode, permutation);
    pulsePolarity = Utility::applyPermutation(pulsePolarity, permutation);
    trigMode = Utility::applyPermutation(trigMode, permutation);
    baselineMean = Utility::applyPermutation(baselineMean, permutation);
    disableSelfTrigger = Utility::applyPermutation(disableSelfTrigger, permutation);
    psdCutBelowThresh = Utility::applyPermutation(psdCutBelowThresh, permutation);
    psdCutAboveThresh = Utility::applyPermutation(psdCutAboveThresh, permutation);
    overRangeRejection = Utility::applyPermutation(overRangeRejection, permutation);
    triggerHysteresis = Utility::applyPermutation(triggerHysteresis, permutation);
    useLocalShapedTrig = Utility::applyPermutation(useLocalShapedTrig, permutation);
    localShapedTrigMode = Utility::applyPermutation(localShapedTrigMode, permutation);
    useLocalTrigVal = Utility::applyPermutation(useLocalTrigVal, permutation);
    localTrigValMode = Utility::applyPermutation(localTrigValMode, permutation);
    localTrigValAsVeto = Utility::applyPermutation(localTrigValAsVeto, permutation);
    extrasWordOptions = Utility::applyPermutation(extrasWordOptions, permutation);
    smoothIntegration = Utility::applyPermutation(smoothIntegration, permutation);
    inputSmoothing = Utility::applyPermutation(inputSmoothing, permutation);
    dcOffset = Utility::applyPermutation(dcOffset, permutation);
    vetoDurationExtension = Utility::applyPermutation(vetoDurationExtension, permutation);
    triggerValidMask = Utility::applyPermutation(triggerValidMask, permutation);
}


std::ostream& operator<<(std::ostream& os, DigitizerChannelData const& dcd)
{
    using std::setw;
    using std::setfill;
    using std::hex;
    using std::dec;
    using std::setprecision;
    using std::fixed;
    int rows = dcd.moduleNumber.size();
    //output the first 9 columns
    os << "_______________________________________________________________________________________________\n";
    os << "Mod#, Chan#, Enabled, Rec Len, Big Range, Aggregate Evnts, PreTrigger, CFD Delay, CFD Fraction,\n";
    for(int i=0; i<rows; ++i)
    {
        os << setw(4)  << setfill(' ') << dcd.moduleNumber[i]              << ", ";
        os << setw(5)  << setfill(' ') << dcd.channelNumber[i]             << ", ";
        os << setw(7)  << setfill(' ') << (dcd.channelEnable[i] ? "T":"F") << ", ";
        os << setw(7)  << setfill(' ') << dcd.recordLength[i]              << ", ";
        os << setw(9)  << setfill(' ') << (dcd.largeRange[i] ? "T":"F")    << ", ";
        os << setw(15) << setfill(' ') << dcd.aggregateEvents[i]           << ", ";
        os << setw(10) << setfill(' ') << dcd.preTrigger[i]                << ", ";
        os << setw(9)  << setfill(' ') << dcd.cfdDelay[i]                  << ", ";
        os << setw(12) << setfill(' ') << dcd.cfdFraction[i]               << ",\n";
    }
        //output the next 7 columns
    os << "__________________________________________________________________________________________________\n";
    os << "Mod#, Chan#, Sh Gate, Lg Gate, Gt Offset, Trig Thresh, Fixed Bsl, Shaped Trig Width, Trig Holdoff,\n";
    for(int i=0; i<rows; ++i)
    {
        os << setw(4)  << setfill(' ') << dcd.moduleNumber[i]    << ", ";
        os << setw(5)  << setfill(' ') << dcd.channelNumber[i]   << ", ";
        os << setw(7)  << setfill(' ') << dcd.shortGate[i]       << ", ";
        os << setw(7)  << setfill(' ') << dcd.longGate[i]        << ", ";
        os << setw(9)  << setfill(' ') << dcd.gateOffset[i]      << ", ";
        os << setw(12) << setfill(' ') << dcd.trigThreshold[i]   << ", ";
        os << setw(9)  << setfill(' ') << dcd.fixedBaseline[i]   << ", ";
        os << setw(17) << setfill(' ') << dcd.shapedTrigWidth[i] << ", ";
        os << setw(12) << setfill(' ') << dcd.trigHoldOff[i]     << ",\n";
    }
    //output the next 7 columns
    os << "_______________________________________________________________________________________________\n";
    os << "Mod#, Chan#, PSD Thresh, Q Sens, Q Pedestal, DPP Trig Counting, Disc Mode, Polarity, Trig Mode,\n";
    for(int i=0; i<rows; ++i)
    {
        os << setw(4)  << setfill(' ') << dcd.moduleNumber[i]                 << ", ";
        os << setw(5)  << setfill(' ') << dcd.channelNumber[i]                << ", ";
        os << setw(7)  << setfill(' ') << dcd.psdThreshold[i]                 << ", ";
        os << setw(7)  << setfill(' ') << dcd.chargeSensitivity[i]            << ", ";
        os << setw(9)  << setfill(' ') << (dcd.chargePedestalOn[i] ? "T":"F") << ", ";
        os << setw(15) << setfill(' ') << dcd.dppTriggerCounting[i]           << ", ";
        os << setw(10) << setfill(' ') << dcd.discMode[i]                     << ", ";
        os << setw(9)  << setfill(' ') << dcd.pulsePolarity[i]                << ", ";
        os << setw(12) << setfill(' ') << dcd.trigMode[i]                     << ",\n";
    }

    //output the next 6 columns
    os << "_______________________________________________________________________________________________________\n";
    os << "Mod#, Chan#, Bsl Mean, No Self Trig, PSD Cut Blw Thresh, PSD Cut Abv Thresh, Rej Over Range, Trig Hyst,\n";
    for(int i=0; i<rows; ++i)
    {
        os << setw(4)  << setfill(' ') << dcd.moduleNumber[i]                   << ", ";
        os << setw(5)  << setfill(' ') << dcd.channelNumber[i]                  << ", ";
        os << setw(8)  << setfill(' ') << dcd.baselineMean[i]                   << ", ";
        os << setw(12) << setfill(' ') << (dcd.disableSelfTrigger[i] ? "T":"F") << ", ";
        os << setw(18) << setfill(' ') << (dcd.psdCutBelowThresh[i]  ? "T":"F") << ", ";
        os << setw(18) << setfill(' ') << (dcd.psdCutAboveThresh[i]  ? "T":"F") << ", ";
        os << setw(14) << setfill(' ') << (dcd.overRangeRejection[i] ? "T":"F") << ", ";
        os << setw(9)  << setfill(' ') << (dcd.triggerHysteresis[i]  ? "T":"F") << ",\n";
    }
    //output the next 5 columns
    os << "__________________________________________________________________________________________________________________\n";
    os << "Mod#, Chan#, Use Lcl Shaped Trig, Lcl Shaped Trig Mode, Use Lcl Trig Val, Lcl Trig Val Mode, Lcl Trig Val as Veto,\n";
    for(int i=0; i<rows; ++i)
    {
        os << setw(4)  << setfill(' ') << dcd.moduleNumber[i]                    << ", ";
        os << setw(5)  << setfill(' ') << dcd.channelNumber[i]                   << ", ";
        os << setw(19) << setfill(' ') << (dcd.useLocalShapedTrig[i] ? "T":"F")  << ", ";
        os << setw(20) << setfill(' ') << dcd.localShapedTrigMode[i]             << ", ";
        os << setw(16) << setfill(' ') << (dcd.useLocalTrigVal[i]  ? "T":"F")    << ", ";
        os << setw(17) << setfill(' ') << dcd.localTrigValMode[i]                << ", ";
        os << setw(20)  << setfill(' ') << (dcd.localTrigValAsVeto[i]  ? "T":"F") << ",\n";
    }

    //output the next 6 columns
    os << "____________________________________________________________________________________________________\n";
    os << "Mod#, Chan#, Extra Wd Opt, Smooth Integ, Inp Smooth, DC Offset, Veto Dur Exten, Trig Validation Mask\n";
    for(int i=0; i<rows; ++i)
    {
        os << setw(4)  << setfill(' ') << dcd.moduleNumber[i]                   << ", ";
        os << setw(5)  << setfill(' ') << dcd.channelNumber[i]                  << ", ";
        os << setw(12) << setfill(' ') << dcd.extrasWordOptions[i]              << ", ";
        os << setw(12) << setfill(' ') << (dcd.smoothIntegration[i] ? "T":"F")  << ", ";
        os << setw(10) << setfill(' ') << dcd.inputSmoothing[i]                 << ", ";
        os << setw(9)  << setfill(' ') << dcd.dcOffset[i]                       << ", ";
        os << setw(14) << setfill(' ') << dcd.vetoDurationExtension[i]          << ", ";
        os << "          0x" << setw(8) << setfill('0') << hex << dcd.triggerValidMask[i] << dec << "\n";
    }
    return os;
}

}
