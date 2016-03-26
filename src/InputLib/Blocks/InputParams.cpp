/***************************************************************************//**
********************************************************************************
**
** @file InputParams.cpp
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
** @details this file holds the implementation of the structure that holds
** pointers to all the block structures
**
********************************************************************************
*******************************************************************************/

#include"InputParams.h"
// includes for C system headers
// includes for C++ system headers
// includes from other libraries
// includes from ORCHID

namespace InputParser
{

InputParameters::InputParameters():generalBlock(nullptr),
    digitizerBlock(nullptr), powerBlock(nullptr)
{
    generalBlock   = new GeneralBlock();
    digitizerBlock = new DigitizerBlock();
    powerBlock     = new PowerBlock();
}

// destructor
InputParameters::~InputParameters()
{
    delete generalBlock;
    delete digitizerBlock;
    delete powerBlock;
}

//validation
bool InputParameters::validateInputParameters()
{
    return ( generalBlock->validate() &&
             digitizerBlock->validate() &&
             powerBlock->validate() );

}

void InputParameters::printValidationProblems()
{
    generalBlock->printValidationErrors();
    digitizerBlock->printValidationErrors();
    powerBlock->printValidationErrors();
}

//stream output
std::ostream& operator<<(std::ostream& os, InputParameters const& params) 
{
    return os << "[Start]\n"
              << *(params.generalBlock)   << "\n"
              << *(params.digitizerBlock) << "\n"
              << *(params.powerBlock)     << "\n"
              << "[End]";
}

}
