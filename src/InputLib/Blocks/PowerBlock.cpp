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


PowerBlock::PowerBlock() : perChannelParameterFile(""),
    mpodIpAddress(""), perChannelParameterFileSet_(false),
    mpodIpAddressSet_(false), pollingRate(1) {}

// required parameters
void PowerBlock::perChannelParameterFileSet(const std::string& input)
{
    perChannelParameterFile = input;
    perChannelParameterFileSet_ = true;
}

void PowerBlock::perModuleParameterFileSet(const std::string& input)
{
    perModuleParameterFile = input;
    perModuleParameterFileSet_ = true;
}

void PowerBlock::mpodIpAddressSet(const std::string& input)
{
    mpodIpAddress = input;
    mpodIpAddressSet_ = true;
}

void PowerBlock::weinerMibFileDirectorySet(const std::string& input)
{
    weinerMibFileDirectory = input;
    weinerMibFileDirectorySet_ = true;
}

void PowerBlock::pollingRateSet(int input)
{
    pollingRate = input;
}

bool PowerBlock::validate()
{
    return (perChannelParameterFileSet_ &&
            perModuleParameterFileSet_ &&
            mpodIpAddressSet_&&
            weinerMibFileDirectorySet_ &&
            (pollingRate > 0));
}

void PowerBlock::printValidationErrors()
{
    std::cout << "PowerBlock Validation Errors:\n";
    if(!perModuleParameterFileSet_)
    {
        std::cout << "    PerModuleParameterFile was not set\n";
    }
    if(!perChannelParameterFileSet_)
    {
        std::cout << "    PerChannelParameterFile was not set\n";
    }
    if(!mpodIpAddressSet_)
    {
        std::cout << "    IPAddress was not set\n";
    }
    if(!weinerMibFileDirectorySet_)
    {
        std::cout << "    MIB file directory was not set\n";
    }
    if(!(pollingRate > 0))
    {
        std::cout << "    Polling rate must be greater than 0\n";
    }
    std::cout << "End PowerBlock Validation Errors\n";
}

std::ostream& operator<<(std::ostream& os, PowerBlock const& pb) 
{
return os << "[PowerBlock]\n"
    << "    PerModuleParameterFile   = "   << pb.perModuleParameterFile  << "\n"
    << "    PerChannelParameterFile  = "   << pb.perChannelParameterFile << "\n"
    << "    IPAddress                = "   << pb.mpodIpAddress           << "\n"
    << "    WienerMibFileDirectory   = "   << pb.weinerMibFileDirectory  << "\n"
    << "    PollingRate              = "   << pb.pollingRate             << "\n"
    << "[EndBlock]";
}

}
