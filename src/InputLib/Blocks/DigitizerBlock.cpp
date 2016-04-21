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
#include<iomanip>
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

void DigitizerBlock::boardAddressListSet(unsigned int input)
{
	boardAddressList.push_back(input);
	boardAddressListSet_ = true;
	++boardAddressListCount_;
}

bool DigitizerBlock::validate()
{
    return (totalChannelsAvailableSet_ &&
            globalCfdFractionSet_ &&
            perChannelParameterFileSet_ &&
            boardAddressListSet_);
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
    if(!boardAddressListSet_)
    {
        std::cout << "    BoardAddressList was not set\n";
    }
    std::cout << "End DigitizerBlock Validation Errors\n";
}

std::ostream& operator<<(std::ostream& os, DigitizerBlock const& db) 
{
	using std::hex;
	using std::dec;
	using std::setw;
	using std::setfill;
	os << "[DigitizerBlock]\n"
    << "    GlobalCfdFraction        = "   << db.globalCfdFraction       << "\n"
    << "    BoardAddressList         = 0x"   << hex << setw(8) << setfill('0') << db.boardAddressList[0] << dec << "\n";
    for(int i=1; i < db.boardAddressListCount_; ++i)
    {
    	os << "                               0x"   << hex << setw(8) << setfill('0') << db.boardAddressList[i] << dec << "\n";
    }
    os << "    TotalChannelsAvailable   = " << db.totalChannelsAvailable  << "\n"
       << "    PerChannelParameterFile  = "   << db.perChannelParameterFile << "\n";
    
    return os << "[EndBlock]";
}

}
