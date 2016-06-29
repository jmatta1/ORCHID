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


DigitizerBlock::DigitizerBlock() : perChannelParameterFile(""),
    perModuleParameterFile(""){}

// required parameters
void DigitizerBlock::perChannelParameterFileSet(std::string& input)
{
    perChannelParameterFile = input;
    perChannelParameterFileSet_ = true;
}

void DigitizerBlock::perModuleParameterFileSet(std::string& input)
{
    perModuleParameterFile = input;
    perModuleParameterFileSet_ = true;
}

bool DigitizerBlock::validate()
{
    return (perChannelParameterFileSet_ &&
            perModuleParameterFileSet_);
}

void DigitizerBlock::printValidationErrors()
{
    std::cout << "DigitizerBlock Validation Errors:\n";
    if(!perChannelParameterFileSet_)
    {
        std::cout << "    PerChannelParameterFile was not set\n";
    }
    if(!perModuleParameterFileSet_)
    {
        std::cout << "    PerModuleParameterFile was not set\n";
    }
    std::cout << "End DigitizerBlock Validation Errors\n";
}

std::ostream& operator<<(std::ostream& os, DigitizerBlock const& db) 
{
	os << "[DigitizerBlock]\n"
       << "    PerChannelParameterFile  = " << db.perChannelParameterFile << "\n"
       << "    PerModuleParameterFile   = " << db.perModuleParameterFile  << "\n";
    return os << "[EndBlock]";
}

}
