/***************************************************************************//**
********************************************************************************
**
** @file blockinputfileparser.h
** @author James Till Matta
** @date 26 Mar, 2016
** @brief Definition file for the class BlockInputFileParser
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** @details Definition for the function that does the parsing
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHID_SRC_INPUTLIB_BLOCKINPUTFILEPARSER_H
#define ORCHID_SRC_INPUTLIB_BLOCKINPUTFILEPARSER_H

// includes for C system headers
// includes for C++ system headers
#include<string>
// includes from other libraries
// includes from ECHOS

namespace InputParser
{

//predeclare the InputParameters class to get a pointer to it
class InputParameters;

//function prototype for the parsing function
bool parseBlockInputFile(InputParameters* inParams,
                         const std::string& inputFileName);

}
#endif  // ECHOS_SRC_INPUTLIB_PARSERS_BLOCKINPUTFILEPARSER_H
