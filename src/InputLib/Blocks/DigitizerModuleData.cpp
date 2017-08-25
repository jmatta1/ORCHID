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

//stream operator to output the LinkType in a pretty way
std::ostream& operator<<(std::ostream& os, LinkType const& value)
{
    switch(value)
    {
    case LinkType::USB:
        return os << "USB";
    case LinkType::Optical:
        return os << "Optical";
    case LinkType::DirectOptical:
        return os << "DOptical";
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

void DigitizerModuleData::addTrigOverlapHandling(int input)
{
    this->trigOverlapHandling.push_back(input);
}

void DigitizerModuleData::addEnableTestPattern(bool input)
{
    this->enableTestPattern.push_back(input);
}

void DigitizerModuleData::addSelfTrigPolarity(int input)
{
    this->selfTrigPolarity.push_back(input);
}

void DigitizerModuleData::addSamplesPerEvent(int input)
{
    this->samplesPerEvent.push_back(input);
}

void DigitizerModuleData::addTrigCountMode(int input)
{
    this->trigCountMode.push_back(input);
}

void DigitizerModuleData::addMemFullMode(int input)
{
    this->memFullMode.push_back(input);
}

void DigitizerModuleData::addMajorityCoincidenceWindow(int input)
{
    this->majorityCoincidenceWindow.push_back(input);
}

void DigitizerModuleData::addMajorityLevel(int input)
{
    this->majorityLevel.push_back(input);
}

void DigitizerModuleData::addNumPostTrigSamples(int input)
{
    this->numPostTrigSamples.push_back(input);
}

void DigitizerModuleData::addIntteruptEventCount(int input)
{
    this->intteruptEventCount.push_back(input);
}

void DigitizerModuleData::addMaxEventsPerBLT(int input)
{
    this->maxEventsPerBLT.push_back(input);
}

bool DigitizerModuleData::validate()
{
    bool output = true;
    if(!(   (linkType.size() >= 1) &&
            (linkNumber.size() >= 1) &&
            (daisyChainNumber.size() >= 1) &&
            (vmeBaseAddr.size() >= 1) &&
            (trigOverlapHandling.size() >= 1) &&
            (enableTestPattern.size() >= 1) &&
            (selfTrigPolarity.size() >= 1) &&
            (samplesPerEvent.size() >= 1) &&
            (trigCountMode.size() >= 1) &&
            (memFullMode.size() >= 1) &&
            (majorityCoincidenceWindow.size() >= 1) &&
            (majorityLevel.size() >= 1) &&
            (numPostTrigSamples.size() >= 1) &&
            (intteruptEventCount.size() >= 1) &&
            (maxEventsPerBLT.size() >= 1)  ))
    {
        output = false;
    }
    int rows = linkType.size();
    if(!(   (rows == linkNumber.size()) &&
            (rows == daisyChainNumber.size()) &&
            (rows == vmeBaseAddr.size()) &&
            (rows == trigOverlapHandling.size()) &&
            (rows == enableTestPattern.size()) &&
            (rows == selfTrigPolarity.size()) &&
            (rows == samplesPerEvent.size()) &&
            (rows == trigCountMode.size()) &&
            (rows == memFullMode.size()) &&
            (rows == majorityCoincidenceWindow.size()) &&
            (rows == majorityLevel.size()) &&
            (rows == numPostTrigSamples.size()) &&
            (rows == intteruptEventCount.size()) &&
            (rows == maxEventsPerBLT.size()) &&
            (rows == memFullMode.size()) ))
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
            (trigOverlapHandling.size() >= 1) &&
            (enableTestPattern.size() >= 1) &&
            (selfTrigPolarity.size() >= 1) &&
            (samplesPerEvent.size() >= 1) &&
            (trigCountMode.size() >= 1) &&
            (memFullMode.size() >= 1) &&
            (majorityCoincidenceWindow.size() >= 1) &&
            (majorityLevel.size() >= 1) &&
            (numPostTrigSamples.size() >= 1) &&
            (intteruptEventCount.size() >= 1) &&
            (maxEventsPerBLT.size() >= 1)  ))
    {
        std::cout << "There must be 1 or more entries for each parameter" <<std::endl;
    }
    int rows = linkType.size();
    if(!(   (rows == linkNumber.size()) &&
            (rows == daisyChainNumber.size()) &&
            (rows == vmeBaseAddr.size()) &&
            (rows == trigOverlapHandling.size()) &&
            (rows == enableTestPattern.size()) &&
            (rows == selfTrigPolarity.size()) &&
            (rows == samplesPerEvent.size()) &&
            (rows == trigCountMode.size()) &&
            (rows == memFullMode.size()) &&
            (rows == majorityCoincidenceWindow.size()) &&
            (rows == majorityLevel.size()) &&
            (rows == numPostTrigSamples.size()) &&
            (rows == intteruptEventCount.size()) &&
            (rows == maxEventsPerBLT.size()) ))
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
    trigOverlapHandling = Utility::applyPermutation(trigOverlapHandling, permutation);
    enableTestPattern = Utility::applyPermutation(enableTestPattern, permutation);
    selfTrigPolarity = Utility::applyPermutation(selfTrigPolarity, permutation);
    samplesPerEvent = Utility::applyPermutation(samplesPerEvent, permutation);
    trigCountMode = Utility::applyPermutation(trigCountMode, permutation);
    memFullMode = Utility::applyPermutation(memFullMode, permutation);
    majorityCoincidenceWindow = Utility::applyPermutation(majorityCoincidenceWindow, permutation);
    majorityLevel = Utility::applyPermutation(majorityLevel, permutation);
    numPostTrigSamples = Utility::applyPermutation(numPostTrigSamples, permutation);
    intteruptEventCount = Utility::applyPermutation(intteruptEventCount, permutation);
    maxEventsPerBLT = Utility::applyPermutation(maxEventsPerBLT, permutation);
}

std::vector<int>            samplesPerEvent;
std::vector<int>            trigCountMode;
std::vector<int>            memFullMode;
std::vector<int>            majorityCoincidenceWindow;
std::vector<int>            majorityLevel;
std::vector<int>            numPostTrigSamples;
std::vector<int>            intteruptEventCount;
std::vector<int>            maxEventsPerBLT;

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
    os << "_______________________________________________________________________________________________________________________________\n";
    os << "Link Number, DC Num, Link Type, VME Base Address, Trigger Overlap Mode, Enable Test Mode, Trigger Polarity, Samples Per Event,\n";
    for(int i=0; i<rows; ++i)
    {
        os << setw(11) << setfill(' ') << dmd.linkNumber[i]                  << ", ";
        os << setw(6)  << setfill(' ') << dmd.daisyChainNumber[i]            << ", ";
        os << setw(9)  << setfill(' ') << dmd.linkType[i]                    << ", ";
        os << "      0x" << setw(8) << setfill('0') << hex << dmd.vmeBaseAddr[i] << dec << ", ";
        os << setw(20) << setfill(' ') << dmd.trigOverlapHandling[i]<< ", ";
        os << setw(16) << setfill(' ') << (dmd.enableTestPattern[i]  ? "T":"F") << ", ";
        os << setw(16) << setfill(' ') << dmd.selfTrigPolarity[i] << ",";
        os << setw(17) << setfill(' ') << dmd.samplesPerEvent[i]  << ",\n";
    }
    //output the next 6 columns
    os << "___________________________________________________________________________________________________________________________________________________________\n";
    os << "Link Number, DC Num, Trig Count Mode, Mem Full Mode, Majority Coin Window, Majority Level, Num Post Trig Samples, Interrupt Event Count, Max Events per BLT\n";
    for(int i=0; i<rows; ++i)
    {
        os << setw(11) << setfill(' ') << dmd.linkNumber[i]                << ", ";
        os << setw(6)  << setfill(' ') << dmd.daisyChainNumber[i]          << ", ";
        os << setw(15) << setfill(' ') << dmd.trigCountMode[i]                 << ", ";
        os << setw(13) << setfill(' ') << (dmd.memFullMode[i] ? "T":"F") << ", ";
        os << setw(20) << setfill(' ') << dmd.majorityCoincidenceWindow[i]             << ", ";
        os << setw(15) << setfill(' ') << dmd.majorityLevel[i]             << ", ";
        os << setw(21) << setfill(' ') << dmd.numPostTrigSamples[i]            << ",\n";
        os << setw(21) << setfill(' ') << dmd.intteruptEventCount[i]            << ",\n";
        os << setw(19) << setfill(' ') << dmd.maxEventsPerBLT[i]            << ",\n";
    }

    return os;
}

}
