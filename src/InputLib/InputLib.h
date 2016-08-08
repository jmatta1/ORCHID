/***************************************************************************//**
********************************************************************************
**
** @file InputLib.h
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
** @details Single file to handle including all the blocks and parsers
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHID_SRC_INPUTLIB_INPUTLIB_H
#define ORCHID_SRC_INPUTLIB_INPUTLIB_H

//grab the definition of the full parameters structure
#include"Blocks/InputParams.h"
//grab the definition of the parameter blocks
#include"Blocks/GeneralBlock.h"
#include"Blocks/PowerBlock.h"
#include"Blocks/DigitizerBlock.h"
#include"Blocks/MpodChannelData.h"
#include"Blocks/MpodModuleData.h"
//#include"Blocks/DigitizerChannelData.h"
#include"Blocks/DigitizerModuleData.h"

//grab the definition of the parsing function
#include"Parsers/Parser.h"
#include"Parsers/MpodParser.h"
#include"Parsers/DigitizerParser.h"


#endif  // ORCHID_SRC_INPUTLIB_INPUTLIB_H
