/***************************************************************************//**
********************************************************************************
**
** @file GeneralBlock.cpp
** @author James Till Matta
** @date 26 Mar, 2016
** @brief
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** @details Implementation file for the GeneralBlock data structure struct
**
********************************************************************************
*******************************************************************************/

#include"InputLib/blocks/GeneralBlock.h"
// includes for C system headers
// includes for C++ system headers
#include<iostream>
// includes from other libraries
// includes from ORCHID

namespace InputParser
{


GeneralBlock::GeneralBlock() : runTitle(""), warnRate(10000),
    updateFrequency(2), runOutputDirectory(""){}

// required parameters
void GeneralBlock::runTitleSet(std::string input)
{
    runTitle = input;
    runTitleSet_ = true;
}

// optional parameters
void GeneralBlock::warnRateSet(int input)
{
    warnRate = input;
    warnRateSet_ = true;
}

void GeneralBlock::updateFrequencySet(int input)
{
    updateFrequency = input;
    updateFrequencySet_ = true;
}

void GeneralBlock::baseOutputDirectorySet(std::string input)
{
    baseOutputDirectory = input;
    baseOutputDirectorySet_ = true;
}

bool GeneralBlock::validate()
{
    return (runTitleSet_ &&
            warnRateSet_ &&
            updateFrequencySet_ &&
            baseOutputDirectorySet_);
}

void GeneralBlock::printValidationErrors()
{
    std::cout << "GeneralBlock Validation Errors:\n";
    if(!runTitleSet_)
    {
        std::cout << "    RunTitle was not set\n";
    }
    if(!warnRateSet_)
    {
        std::cout << "    WarnRate was not set\n";
    }
    if(!updateFrequency_)
    {
        std::cout << "    UpdateFrequency was not set\n";
    }
    if(!baseOutputDirectorySet_)
    {
        std::cout << "    BaseOutputDirectory was not set\n";
    }
    std::cout << "End GeneralBlock Validation Errors\n";
}

std::ostream& operator<<(std::ostream& os, CalculationBlock const& cb) 
{
return os << "[GeneralBlock]\n"
    << "    RunTitle            = \"" << cb.runTitle            << "\"\n"
    << "    WarnRate            = "   << cb.warnRate            << "\n"
    << "    UpdateFrequency     = "   << cb.updateFrequency     << "\n"
    << "    BaseOutputDirectory = "   << cb.baseOutputDirectory << "\n"
    << "[EndBlock]";
}

}
