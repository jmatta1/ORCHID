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

void DigitizerChannelData::addParticipateInTrigger(bool input)
{
    this->participateInTrigger.push_back(input);
}

void DigitizerChannelData::addLargeRange(bool input)
{
    this->largeRange.push_back(input);
}

void DigitizerChannelData::addPulseWidth(int input)
{
    this->pulseWidth.push_back(input);
}

void DigitizerChannelData::addTrigThreshold(int input)
{
    this->trigThreshold.push_back(input);
}

void DigitizerChannelData::addCoupleTrigLogic(int input)
{
    this->coupleTrigLogic.push_back(input);
}

void DigitizerChannelData::addCouplePulseType(int input)
{
    this->couplePulseType.push_back(input);
}

void DigitizerChannelData::addDcOffset(int input)
{
    this->dcOffset.push_back(input);
}

bool DigitizerChannelData::validate()
{
    bool output = true;
    if(!(   (moduleNumber.size() >= 1) &&
            (channelNumber.size() >= 1) &&
            (channelEnable.size() >= 1) &&
            (participateInTrigger.size() >= 1) &&
            (largeRange.size() >= 1) &&
            (pulseWidth.size() >= 1) &&
            (trigThreshold.size() >= 1) &&
            (coupleTrigLogic.size() >= 1) &&
            (couplePulseType.size() >= 1) &&
            (dcOffset.size() >= 1)  ))
    {
        output = false;
    }
    int rows = moduleNumber.size();
    if(!(   (rows == channelNumber.size()) &&
            (rows == channelEnable.size()) &&
            (rows == participateInTrigger.size()) &&
            (rows == largeRange.size()) &&
            (rows == largeRange.size()) &&
            (rows == pulseWidth.size()) &&
            (rows == trigThreshold.size()) &&
            (rows == coupleTrigLogic.size()) &&
            (rows == couplePulseType.size()) &&
            (rows == dcOffset.size())     ))
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
            (participateInTrigger.size() >= 1) &&
            (largeRange.size() >= 1) &&
            (pulseWidth.size() >= 1) &&
            (trigThreshold.size() >= 1) &&
            (coupleTrigLogic.size() >= 1) &&
            (couplePulseType.size() >= 1) &&
            (dcOffset.size() >= 1)  ))
    {
        std::cout << "There must be 1 or more entries for each parameter" <<std::endl;
    }
    int rows = moduleNumber.size();
    if(!(   (rows == channelNumber.size()) &&
            (rows == channelEnable.size()) &&
            (rows == participateInTrigger.size()) &&
            (rows == largeRange.size()) &&
            (rows == largeRange.size()) &&
            (rows == pulseWidth.size()) &&
            (rows == trigThreshold.size()) &&
            (rows == coupleTrigLogic.size()) &&
            (rows == couplePulseType.size()) &&
            (rows == dcOffset.size())     ))
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
    participateInTrigger = Utility::applyPermutation(participateInTrigger, permutation);
    largeRange = Utility::applyPermutation(largeRange, permutation);
    pulseWidth = Utility::applyPermutation(pulseWidth, permutation);
    trigThreshold = Utility::applyPermutation(trigThreshold, permutation);
    coupleTrigLogic = Utility::applyPermutation(couplePulseType, permutation);
    dcOffset = Utility::applyPermutation(dcOffset, permutation);
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
    os << "Mod#, Chan#, Enabled, Add To Trigger, Big Range, Trig Pulse Width, TrigThreshold, Couple Trig Logic, DC Offset,\n";
    for(int i=0; i<rows; ++i)
    {
        os << setw(4)  << setfill(' ') << dcd.moduleNumber[i]                       << ", ";
        os << setw(5)  << setfill(' ') << dcd.channelNumber[i]                      << ", ";
        os << setw(7)  << setfill(' ') << (dcd.channelEnable[i] ? "T":"F")          << ", ";
        os << setw(14) << setfill(' ') << (dcd.participateInTrigger[i] ? "T":"F")   << ", ";
        os << setw(9)  << setfill(' ') << (dcd.largeRange[i] ? "T":"F")             << ", ";
        os << setw(16) << setfill(' ') << dcd.pulseWidth[i]                         << ", ";
        os << setw(13) << setfill(' ') << dcd.trigThreshold[i]                      << ", ";
        os << setw(16) << setfill(' ') << dcd.coupleTrigLogic[i]                    << ", ";
        os << setw(9)  << setfill(' ') << dcd.dcOffset[i]                           << "\n";
    }
    return os;
}

}
