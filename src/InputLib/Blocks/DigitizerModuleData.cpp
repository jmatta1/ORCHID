/***************************************************************************//**
********************************************************************************
**
** @file DigitizerModuleData.cpp
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
** @details implementation file that holds the class with digitizer per module config
**
********************************************************************************
*******************************************************************************/

#include"DigitizerModuleData.h"
// includes for C system headers
// includes for C++ system headers
#include<iomanip>
// includes from other libraries
// includes from ORCHID
#include"Utility/SortPermutation.h"

namespace InputParser
{

//stream operator to output the ProblemType in a pretty way
std::ostream& operator<<(std::ostream& os, LinkType const& value)
{
    switch(value)
    {
    case LinkType::USB:
        return os << "USB";
    case LinkType::Optical:
        return os << "Optical";
    }
}

void DigitizerModuleData::addLinkType(LinkType input)
{
    this->linkType.push_back(input);
}

void DigitizerModuleData::addLinkNumber(int input)
{
    this->linkNumber.push_back(input);
}

void DigitizerModuleData::addDaisyChainNumber(int input)
{
    this->daisyChainNumber.push_back(input);
}

void DigitizerModuleData::addVmeBaseAddr(unsigned int input)
{
    this->vmeBaseAddr.push_back(input);
}

void DigitizerModuleData::addEnableAutoFlush(bool input)
{
    this->enableAutoFlush.push_back(input);
}

void DigitizerModuleData::addPropogateTrigs(bool input)
{
    this->propogateTrigs.push_back(input);
}

void DigitizerModuleData::addRecordWaveforms(bool input)
{
    this->recordWaveforms.push_back(input);
}

void DigitizerModuleData::addAnalogProbe(int input)
{
    this->analogProbe.push_back(input);
}

void DigitizerModuleData::addDualTrace(int input)
{
    this->dualTrace.push_back(input);
}

void DigitizerModuleData::addRecExtrasWord(bool input)
{
    this->recExtrasWord.push_back(input);
}

void DigitizerModuleData::addDigVirtProbe1(int input)
{
    this->digVirtProbe1.push_back(input);
}

void DigitizerModuleData::addDigVirtProbe2(int input)
{
    this->digVirtProbe2.push_back(input);
}

void DigitizerModuleData::addChanBuffPerAgg(int input)
{
    this->chanBuffPerAgg.push_back(input);
}

void DigitizerModuleData::addTriggerCountingMethod(int input)
{
    this->triggerCountingMethod.push_back(input);
}

void DigitizerModuleData::addMemFullMode(int input)
{
    this->memFullMode.push_back(input);
}

void DigitizerModuleData::addPllRefClock(int input)
{
    this->pllRefClock.push_back(input);
}

void DigitizerModuleData::addGlobalChanPairTrigMask(int input)
{
    this->globalChanPairTrigMask.push_back(input);
}

void DigitizerModuleData::addGlobalCoincidenceWindow(int input)
{
    this->globalCoincidenceWindow.push_back(input);
}

void DigitizerModuleData::addGlobalMajorityLevel(int input)
{
    this->globalMajorityLevel.push_back(input);
}

void DigitizerModuleData::addExternalTrigger(bool input)
{
    this->externalTrigger.push_back(input);
}

void DigitizerModuleData::addChanPairTrigOutMask(int input)
{
    this->chanPairTrigOutMask.push_back(input);
}

void DigitizerModuleData::addTrigOutGenerationLogic(int input)
{
    this->trigOutGenerationLogic.push_back(input);
}

void DigitizerModuleData::addTrigOutMajorityLevel(int input)
{
    this->trigOutMajorityLevel.push_back(input);
}

void DigitizerModuleData::addExtTrigInTrigOut(int input)
{
    this->extTrigInTrigOut.push_back(input);
}

void DigitizerModuleData::addMemBuffAlmostFullLevel(int input)
{
    this->memBuffAlmostFullLevel.push_back(input);
}

void DigitizerModuleData::addRunStartStopDelay(int input)
{
    this->runStartStopDelay.push_back(input);
}

void DigitizerModuleData::addUseExtTrigger(bool input)
{
    this->useExtTrigger.push_back(input);
}

void DigitizerModuleData::addInterruptEventCount(int input)
{
    this->interruptEventCount.push_back(input);
}

void DigitizerModuleData::addAggregatesPerBlockTransfer(int input)
{
    this->aggregatesPerBlockTransfer.push_back(input);
}

bool DigitizerModuleData::validate()
{
    bool output = true;
    if(!(   (linkType.size() >= 1) &&
            (linkNumber.size() >= 1) &&
            (daisyChainNumber.size() >= 1) &&
            (vmeBaseAddr.size() >= 1) &&
            (enableAutoFlush.size() >= 1) &&
            (propogateTrigs.size() >= 1) &&
            (recordWaveforms.size() >= 1) &&
            (analogProbe.size() >= 1) &&
            (dualTrace.size() >= 1) &&
            (recExtrasWord.size() >= 1) &&
            (digVirtProbe1.size() >= 1) &&
            (digVirtProbe2.size() >= 1) &&
            (chanBuffPerAgg.size() >= 1) &&
            (triggerCountingMethod.size() >= 1) &&
            (memFullMode.size() >= 1) &&
            (pllRefClock.size() >= 1) &&
            (globalChanPairTrigMask.size() >= 1) &&
            (globalCoincidenceWindow.size() >= 1) &&
            (globalMajorityLevel.size() >= 1) &&
            (externalTrigger.size() >= 1) &&
            (chanPairTrigOutMask.size() >= 1) &&
            (trigOutGenerationLogic.size() >= 1) &&
            (trigOutMajorityLevel.size() >= 1) &&
            (extTrigInTrigOut.size() >= 1) &&
            (memBuffAlmostFullLevel.size() >= 1) &&
            (runStartStopDelay.size() >= 1) &&
            (useExtTrigger.size() >= 1) &&
            (interruptEventCount.size() >= 1) &&
            (aggregatesPerBlockTransfer.size() >= 1)))
    {
        output = false;
    }
    int rows = linkType.size();
    if(!(   (rows == linkNumber.size()) &&
            (rows == daisyChainNumber.size()) &&
            (rows == vmeBaseAddr.size()) &&
            (rows == enableAutoFlush.size()) &&
            (rows == propogateTrigs.size()) &&
            (rows == recordWaveforms.size()) &&
            (rows == analogProbe.size()) &&
            (rows == dualTrace.size()) &&
            (rows == recExtrasWord.size()) &&
            (rows == digVirtProbe1.size()) &&
            (rows == digVirtProbe2.size()) &&
            (rows == chanBuffPerAgg.size()) &&
            (rows == triggerCountingMethod.size()) &&
            (rows == memFullMode.size()) &&
            (rows == pllRefClock.size()) &&
            (rows == globalChanPairTrigMask.size()) &&
            (rows == globalCoincidenceWindow.size()) &&
            (rows == globalMajorityLevel.size()) &&
            (rows == externalTrigger.size()) &&
            (rows == chanPairTrigOutMask.size()) &&
            (rows == trigOutGenerationLogic.size()) &&
            (rows == trigOutMajorityLevel.size()) &&
            (rows == extTrigInTrigOut.size()) &&
            (rows == memBuffAlmostFullLevel.size()) &&
            (rows == runStartStopDelay.size()) &&
            (rows == useExtTrigger.size()) &&
            (rows == interruptEventCount.size()) &&
            (rows == aggregatesPerBlockTransfer.size())))
    {
        output = false;
    }
    //TODO: Add substantially more validation tests here
    return output;
}

void DigitizerModuleData::printValidationErrors()
{
    if(!(   (linkType.size() >= 1) &&
            (linkNumber.size() >= 1) &&
            (daisyChainNumber.size() >= 1) &&
            (vmeBaseAddr.size() >= 1) &&
            (enableAutoFlush.size() >= 1) &&
            (propogateTrigs.size() >= 1) &&
            (recordWaveforms.size() >= 1) &&
            (analogProbe.size() >= 1) &&
            (dualTrace.size() >= 1) &&
            (recExtrasWord.size() >= 1) &&
            (digVirtProbe1.size() >= 1) &&
            (digVirtProbe2.size() >= 1) &&
            (chanBuffPerAgg.size() >= 1) &&
            (triggerCountingMethod.size() >= 1) &&
            (memFullMode.size() >= 1) &&
            (pllRefClock.size() >= 1) &&
            (globalChanPairTrigMask.size() >= 1) &&
            (globalCoincidenceWindow.size() >= 1) &&
            (globalMajorityLevel.size() >= 1) &&
            (externalTrigger.size() >= 1) &&
            (chanPairTrigOutMask.size() >= 1) &&
            (trigOutGenerationLogic.size() >= 1) &&
            (trigOutMajorityLevel.size() >= 1) &&
            (extTrigInTrigOut.size() >= 1) &&
            (memBuffAlmostFullLevel.size() >= 1) &&
            (runStartStopDelay.size() >= 1) &&
            (useExtTrigger.size() >= 1) &&
            (interruptEventCount.size() >= 1) &&
            (aggregatesPerBlockTransfer.size() >= 1)))
    {
        std::cout << "There must be 1 or more entries for each parameter" <<std::endl;
    }
    int rows = linkType.size();
    if(!(   (rows == linkNumber.size()) &&
            (rows == daisyChainNumber.size()) &&
            (rows == vmeBaseAddr.size()) &&
            (rows == enableAutoFlush.size()) &&
            (rows == propogateTrigs.size()) &&
            (rows == recordWaveforms.size()) &&
            (rows == analogProbe.size()) &&
            (rows == dualTrace.size()) &&
            (rows == recExtrasWord.size()) &&
            (rows == digVirtProbe1.size()) &&
            (rows == digVirtProbe2.size()) &&
            (rows == chanBuffPerAgg.size()) &&
            (rows == triggerCountingMethod.size()) &&
            (rows == memFullMode.size()) &&
            (rows == pllRefClock.size()) &&
            (rows == globalChanPairTrigMask.size()) &&
            (rows == globalCoincidenceWindow.size()) &&
            (rows == globalMajorityLevel.size()) &&
            (rows == externalTrigger.size()) &&
            (rows == chanPairTrigOutMask.size()) &&
            (rows == trigOutGenerationLogic.size()) &&
            (rows == trigOutMajorityLevel.size()) &&
            (rows == extTrigInTrigOut.size()) &&
            (rows == memBuffAlmostFullLevel.size()) &&
            (rows == runStartStopDelay.size()) &&
            (rows == useExtTrigger.size()) &&
            (rows == interruptEventCount.size()) &&
            (rows == aggregatesPerBlockTransfer.size())))
    {
        std::cout << "There must the same number of rows for every column" <<std::endl;
    }
    //TODO: Add substantially more validation tests here
}

void DigitizerModuleData::sort()
{
    Utility::TwoKeyIntCompare comp;
    std::vector<std::size_t> permutation = Utility::twoKeySortPermutation(linkNumber, daisyChainNumber, comp);
    linkType = Utility::applyPermutation(linkType, permutation);
    linkNumber = Utility::applyPermutation(linkNumber, permutation);
    daisyChainNumber = Utility::applyPermutation(daisyChainNumber, permutation);
    vmeBaseAddr = Utility::applyPermutation(vmeBaseAddr, permutation);
    enableAutoFlush = Utility::applyPermutation(enableAutoFlush, permutation);
    propogateTrigs = Utility::applyPermutation(propogateTrigs, permutation);
    recordWaveforms = Utility::applyPermutation(recordWaveforms, permutation);
    analogProbe = Utility::applyPermutation(analogProbe, permutation);
    dualTrace = Utility::applyPermutation(dualTrace, permutation);
    recExtrasWord = Utility::applyPermutation(recExtrasWord, permutation);
    digVirtProbe1 = Utility::applyPermutation(digVirtProbe1, permutation);
    digVirtProbe2 = Utility::applyPermutation(digVirtProbe2, permutation);
    chanBuffPerAgg = Utility::applyPermutation(chanBuffPerAgg, permutation);
    triggerCountingMethod = Utility::applyPermutation(triggerCountingMethod, permutation);
    memFullMode = Utility::applyPermutation(memFullMode, permutation);
    pllRefClock = Utility::applyPermutation(pllRefClock, permutation);
    globalChanPairTrigMask = Utility::applyPermutation(globalChanPairTrigMask, permutation);
    globalCoincidenceWindow = Utility::applyPermutation(globalCoincidenceWindow, permutation);
    globalMajorityLevel = Utility::applyPermutation(globalMajorityLevel, permutation);
    externalTrigger = Utility::applyPermutation(externalTrigger, permutation);
    chanPairTrigOutMask = Utility::applyPermutation(chanPairTrigOutMask, permutation);
    trigOutGenerationLogic = Utility::applyPermutation(trigOutGenerationLogic, permutation);
    trigOutMajorityLevel = Utility::applyPermutation(trigOutMajorityLevel, permutation);
    extTrigInTrigOut = Utility::applyPermutation(extTrigInTrigOut, permutation);
    memBuffAlmostFullLevel = Utility::applyPermutation(memBuffAlmostFullLevel, permutation);
    runStartStopDelay = Utility::applyPermutation(runStartStopDelay, permutation);
    useExtTrigger = Utility::applyPermutation(useExtTrigger, permutation);
    interruptEventCount = Utility::applyPermutation(interruptEventCount, permutation);
    aggregatesPerBlockTransfer = Utility::applyPermutation(aggregatesPerBlockTransfer, permutation);
}

std::ostream& operator<<(std::ostream& os, DigitizerModuleData const& dmd)
{
    using std::setw;
    using std::setfill;
    using std::hex;
    using std::dec;
    using std::setprecision;
    using std::fixed;
    int rows = dmd.linkType.size();
    //output the first 7 columns
    os << "__________________________________________________________________________________________________________\n";
    os << "Link Type, Link Number, DC Num, VME Base Address, Enable Auto Flush, Propogate Triggers, Record Waveforms,\n";
    for(int i=0; i<rows; ++i)
    {
        os << setw(9)  << setfill(' ') << dmd.linkType[i]                    << ", ";
        os << setw(11) << setfill(' ') << dmd.linkNumber[i]                  << ", ";
        os << setw(6)  << setfill(' ') << dmd.daisyChainNumber[i]            << ", ";
        os << "      0x" << setw(8) << setfill('0') << hex << dmd.vmeBaseAddr[i] << dec << ", ";
        os << setw(17) << setfill(' ') << (dmd.enableAutoFlush[i] ? "T":"F") << ", ";
        os << setw(18) << setfill(' ') << (dmd.propogateTrigs[i]  ? "T":"F") << ", ";
        os << setw(16) << setfill(' ') << (dmd.recordWaveforms[i] ? "T":"F") << ",\n";
    }
    //output the next 6 columns
    os << "______________________________________________________________________________________________________________\n";
    os << "Link Number, DC Num, Analog Probe, Dual Trace, Rec Extra Word, Virt Probe 1, Virt Probe2, Chan Buff/Aggregate,\n";
    for(int i=0; i<rows; ++i)
    {
        os << setw(11) << setfill(' ') << dmd.linkNumber[i]                << ", ";
        os << setw(6)  << setfill(' ') << dmd.daisyChainNumber[i]          << ", ";
        os << setw(12) << setfill(' ') << dmd.analogProbe[i]               << ", ";
        os << setw(10) << setfill(' ') << dmd.dualTrace[i]                 << ", ";
        os << setw(14) << setfill(' ') << (dmd.recExtrasWord[i] ? "T":"F") << ", ";
        os << setw(12) << setfill(' ') << dmd.digVirtProbe1[i]             << ", ";
        os << setw(12) << setfill(' ') << dmd.digVirtProbe2[i]             << ", ";
        os << setw(19) << setfill(' ') << dmd.chanBuffPerAgg[i]            << ",\n";
    }
    //output the next 6 columns
    os << "______________________________________________________________________________________________________________________\n";
    os << "Link Number, DC Num, Trig Count Method, Mem Full Mode, PLL Ref Clock, Gbl CP Trig Mask, Gbl Coin Window, Gbl Majority,\n";
    for(int i=0; i<rows; ++i)
    {
        os << setw(11) << setfill(' ') << dmd.linkNumber[i]              << ", ";
        os << setw(6)  << setfill(' ') << dmd.daisyChainNumber[i]        << ", ";
        os << setw(17) << setfill(' ') << dmd.triggerCountingMethod[i]   << ", ";
        os << setw(13) << setfill(' ') << dmd.memFullMode[i]             << ", ";
        os << setw(13) << setfill(' ') << dmd.pllRefClock[i]             << ", ";
        os << "            0x" << setw(2) << setfill('0') << hex << dmd.globalChanPairTrigMask[i] << dec << ", ";
        os << setw(15) << setfill(' ') << dmd.globalCoincidenceWindow[i] << ", ";
        os << setw(12) << setfill(' ') << dmd.globalMajorityLevel[i]     << ",\n";
    }
    //output the next 6 columns
    os << "__________________________________________________________________________________________________________________\n";
    os << "Link Number, DC Num, Ext Trig, CP TOut Mask, TOut Gen Log, TOut Majority, Ext Trig For TOut, Mem Buff Almost Full,\n";
    for(int i=0; i<rows; ++i)
    {
        os << setw(11) << setfill(' ') << dmd.linkNumber[i]                  << ", ";
        os << setw(6)  << setfill(' ') << dmd.daisyChainNumber[i]            << ", ";
        os << setw(8)  << setfill(' ') << (dmd.externalTrigger[i] ? "T":"F") << ", ";
        os << "        0x" << setw(2) << setfill('0') << dmd.chanPairTrigOutMask[i] << dec << ", ";
        os << setw(12) << setfill(' ') << dmd.trigOutGenerationLogic[i]      << ", ";
        os << setw(13) << setfill(' ') << dmd.trigOutMajorityLevel[i]        << ", ";
        os << setw(17) << setfill(' ') << dmd.extTrigInTrigOut[i]            << ", ";
        os << setw(20) << setfill(' ') << dmd.memBuffAlmostFullLevel[i]      << ",\n";
    }
    //output the last 4 columns
    os << "_______________________________________________________________________________________________\n";
    os << "Link Number, DC Num, Start/Stop Delay, Use Ext Trig, Interrupt Evnt Count, Aggregates Per Block\n";
    for(int i=0; i<rows; ++i)
    {
        os << setw(11) << setfill(' ') << dmd.linkNumber[i]                 << ", ";
        os << setw(6)  << setfill(' ') << dmd.daisyChainNumber[i]           << ", ";
        os << setw(16) << setfill(' ') << dmd.runStartStopDelay[i]          << ", ";
        os << setw(12) << setfill(' ') << (dmd.useExtTrigger[i] ? "T":"F")  << ", ";
        os << setw(20) << setfill(' ') << dmd.interruptEventCount[i]        << ", ";
        os << setw(20) << setfill(' ') << dmd.aggregatesPerBlockTransfer[i] << "\n";
    }
    
    return os;
}

}
