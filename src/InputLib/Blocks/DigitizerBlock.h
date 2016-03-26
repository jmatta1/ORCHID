/***************************************************************************//**
********************************************************************************
**
** @file DigitizerBlock.h
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
** @details Definition file for the Digitizer Block data struct
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHID_SRC_INPUTLIB_BLOCKS_DIGITIZERBLOCK_H
#define ORCHID_SRC_INPUTLIB_BLOCKS_DIGITIZERBLOCK_H

// includes for C system headers
// includes for C++ system headers
#include<string>
// includes from other libraries
// includes from ORCHID

class ostream;

namespace InputParser
{

// structure to hold the information in calculation blocks
struct DigitizerBlock
{
    DigitizerBlock();
    // configuration parameters
    // required parameters
	int totalChannelsAvailable;
	float globalCfdFraction;
	std::string perChannelOverrideFile;
    
    // parameter setters for binding with boost::spirit::qi
    // required parameters
    void totalChannelsAvailableSet(int input);
    void globalCfdFractionSet(float input);
    void perChannelParameterFileSet(std::string input);
	
	bool validate();
	void printValidationErrors();
	
	// ostream operator to allow outputting of the block class
    friend std::ostream& operator<<(std::ostream& os, DigitizerBlock const& db) ;

private:
    // parameters to make sure the required parameters were set
    bool numberChannelsUsedSet_;
    bool globalCfdFractionSet_;
    bool perChannelParameterFileSet_;
};

}
#endif  // ORCHID_SRC_INPUTLIB_BLOCKS_DIGITIZERBLOCK_H
