/***************************************************************************//**
********************************************************************************
**
** @file DigitizerBlock.cpp
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
** @details Implementation file for the DigitizerBlock data structure
**
********************************************************************************
*******************************************************************************/

#include"InputLib/blocks/DigitizerBlock.h"
// includes for C system headers
// includes for C++ system headers
#include<iostream>
// includes from other libraries
// includes from ORCHID

namespace InputParser
{


DigitizerBlock::DigitizerBlock() : numberChannelsUsed(1),
    globalCfdFraction(0.2), perChannelParameterFile(""){}

// required parameters
void DigitizerBlock::numberChannelsUsedSet(int input)
{
    numberChannelsUsed = input;
    numberChannelsUsedSet_ = true;
}

void DigitizerBlock::globalCfdFractionSet(float input)
{
    globalCfdFraction = input;
    globalVoltageOffsetSet_ = true;
}

void DigitizerBlock::perChannelParameterFileSet(std::string input)
{
    perChannelParameterFile = input;
    perChannelParameterFile_ = true;
}

bool DigitizerBlock::validate()
{
    return (numberChannelsUsedSet_ &&
            globalCfdFractionSet_ &&
            perChannelParameterFile_ );
}

void DigitizerBlock::printValidationErrors()
{
    std::cout << "DigitizerBlock Validation Errors:\n";
    if(!numberChannelsUsedSet_)
    {
        std::cout << "    NumberInputChannelsUsed was not set\n";
    }
    if(!globalCfdFractionSet_)
    {
        std::cout << "    GlobalCfdFraction was not set\n";
    }
    if(!perChannelParameterFile_)
    {
        std::cout << "    PerChannelOverrideFile was not set\n";
    }
    std::cout << "End DigitizerBlock Validation Errors\n";
}

std::ostream& operator<<(std::ostream& os, DigitizerBlock const& db) 
{
return os << "[DigitizerBlock]\n"
    << "    NumberInputChannelsUsed  = \"" << db.numberChannelsUsed     << "\"\n"
    << "    GlobalCfdFraction        = "   << db.globalCfdFraction      << "\n"
    << "    PerChannelParameterFile  = "   << db.perChannelOverrideFile << "\n"
    << "[EndBlock]";
}

}
