/***************************************************************************//**
********************************************************************************
**
** @file ParseAndValidate.h
** @author James Till Matta
** @date 27 Apr, 2016
** @brief
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** @details Holds the function prototypes for the functions called by
** int main(...) to actually parse and validate the various data files
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHID_SRC_UTILITY_PARSEANDVALIDATE_H
#define ORCHID_SRC_UTILITY_PARSEANDVALIDATE_H

// includes for C system headers
// includes for C++ system headers
#include<string>
// includes from other libraries
// includes from ORCHID
#include"InputLib/InputLib.h"


bool parseAndValidateInput(InputParser::InputParameters& params,
                           const std::string& inputFileName);

bool parseAndValidateMpodChannel(InputParser::MpodChannelData& mpodChannelData,
                                 const std::string& inputFileName);

bool parseAndValidateMpodModule(InputParser::MpodModuleData& mpodModuleData,
                                const std::string& inputFileName);

#endif //ORCHID_SRC_UTILITY_PARSEANDVALIDATE_H
