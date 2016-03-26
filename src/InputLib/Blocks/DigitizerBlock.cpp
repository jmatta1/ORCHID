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

#include"InputLib/Blocks/DigitizerBlock.h"
// includes for C system headers
// includes for C++ system headers
#include<iostream>
// includes from other libraries
// includes from ORCHID

namespace InputParser
{


DigitizerBlock::DigitizerBlock() : totalChannelsAvailable(1),
    globalCfdFraction(0.2), perChannelParameterFile(""){}

// required parameters
void DigitizerBlock::totalChannelsAvailableSet(int input)
{
    totalChannelsAvailable = input;
    totalChannelsAvailableSet_ = true;
}

void DigitizerBlock::globalCfdFractionSet(float input)
{
    globalCfdFraction = input;
    globalCfdFractionSet_ = true;
}

void DigitizerBlock::perChannelParameterFileSet(std::string input)
{
    perChannelParameterFile = input;
    perChannelParameterFileSet_ = true;
}

bool DigitizerBlock::validate()
{
    return (totalChannelsAvailableSet_ &&
            globalCfdFractionSet_ &&
            perChannelParameterFileSet_ );
}

void DigitizerBlock::printValidationErrors()
{
    std::cout << "DigitizerBlock Validation Errors:\n";
    if(!totalChannelsAvailableSet_)
    {
        std::cout << "    TotalChannelsAvailable was not set\n";
    }
    if(!globalCfdFractionSet_)
    {
        std::cout << "    GlobalCfdFraction was not set\n";
    }
    if(!perChannelParameterFileSet_)
    {
        std::cout << "    PerChannelParameterFile was not set\n";
    }
    std::cout << "End DigitizerBlock Validation Errors\n";
}

std::ostream& operator<<(std::ostream& os, DigitizerBlock const& db) 
{
return os << "[DigitizerBlock]\n"
    << "    TotalChannelsAvailable   = \"" << db.totalChannelsAvailable  << "\"\n"
    << "    GlobalCfdFraction        = "   << db.globalCfdFraction       << "\n"
    << "    PerChannelParameterFile  = "   << db.perChannelParameterFile << "\n"
    << "[EndBlock]";
}

}
