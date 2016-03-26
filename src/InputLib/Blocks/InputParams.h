/***************************************************************************//**
********************************************************************************
**
** @file InputParams.h
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
** @details this file holds the definition of the structure that holds pointers
** to all the block structures
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHID_SRC_INPUTLIB_BLOCKS_INPUTPARAMS_H
#define ORCHID_SRC_INPUTLIB_BLOCKS_INPUTPARAMS_H

// includes for C system headers
// includes for C++ system headers
#include<iostream>
// includes from other libraries
// includes from ORCHID
#include"outputblock.h"
#include"calculationblock.h"

namespace InputParser
{

struct GeneralBlock;
struct DigitizerBlock;
struct PowerBlock;

struct InputParameters
{
    // constructor
    InputParameters();
    // destructor
    ~InputParameters();
    
    //validation
    bool validateInputParameters();
    void printValidationProblems();
    
    //stream output
    friend std::ostream& operator<<(std::ostream& os, InputParameters const& params);
    
    // input block structs
    GeneralBlock*   generalBlock;
    DigitizerBlock* digitizerBlock;
    PowerBlock*     powerBlock;
};

}
#endif  // ORCHID_SRC_INPUTLIB_BLOCKS_INPUTPARAMS_H
