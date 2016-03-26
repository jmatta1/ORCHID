/***************************************************************************//**
********************************************************************************
**
** @file PowerBlock.cpp
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
** @details Implementation file for the PowerBlock data structure
**
********************************************************************************
*******************************************************************************/

#include"InputLib/Blocks/PowerBlock.h"
// includes for C system headers
// includes for C++ system headers
#include<iostream>
// includes from other libraries
// includes from ORCHID

namespace InputParser
{


PowerBlock::PowerBlock() : totalChannelsAvailable(1),
                           perChannelParameterFile(""){}

// required parameters
void PowerBlock::totalChannelsAvailableSet(int input)
{
    totalChannelsAvailable = input;
    totalChannelsAvailableSet_ = true;
}

void PowerBlock::perChannelParameterFileSet(std::string input)
{
    perChannelParameterFile = input;
    perChannelParameterFileSet_ = true;
}

bool PowerBlock::validate()
{
    return (totalChannelsAvailableSet_ &&
            perChannelParameterFileSet_ );
}

void PowerBlock::printValidationErrors()
{
    std::cout << "PowerBlock Validation Errors:\n";
    if(!totalChannelsAvailableSet_)
    {
        std::cout << "    TotalChannelsAvailable was not set\n";
    }
    if(!perChannelParameterFileSet_)
    {
        std::cout << "    PerChannelParameterFile was not set\n";
    }
    std::cout << "End PowerBlock Validation Errors\n";
}

std::ostream& operator<<(std::ostream& os, PowerBlock const& pb) 
{
return os << "[PowerBlock]\n"
    << "    TotalChannelsAvailable   = \"" << pb.totalChannelsAvailable     << "\"\n"
    << "    PerChannelParameterFile  = "   << pb.perChannelParameterFile << "\n"
    << "[EndBlock]";
}

}
