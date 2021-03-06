/***************************************************************************//**
********************************************************************************
**
** @file GeneralBlock.h
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
** @details Definition file for the GeneralBlock data struct
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHID_SRC_INPUTLIB_BLOCKS_GENERALBLOCK_H
#define ORCHID_SRC_INPUTLIB_BLOCKS_GENERALBLOCK_H

// includes for C system headers
// includes for C++ system headers
#include<string>
// includes from other libraries
// includes from ORCHID

class ostream;

namespace InputParser
{

// structure to hold the information in general blocks
struct GeneralBlock
{
    GeneralBlock();
    // configuration parameters
    // required parameters
	int         warnRate; //trigger rate in hertz to warn user at
	int         updateFrequency; //rate in hertz to update display
    int         processingThreadCount; //number of processing threads to spawn
	std::string baseOutputDirectory;
    
    // parameter setters for binding with boost::spirit::qi
    // required parameters
    void warnRateSet(int input);
	void updateFrequencySet(int input);
    void processingThreadCountSet(int input);
	void baseOutputDirectorySet(std::string input);
	
	bool validate();
	void printValidationErrors();
	
	// ostream operator to allow outputting of the block class
    friend std::ostream& operator<<(std::ostream& os, GeneralBlock const& gb) ;

private:
    // parameters to make sure the required parameters were set
    bool warnRateSet_;
    bool updateFrequencySet_;
    bool processingThreadCountSet_;
    bool baseOutputDirectorySet_;
};

}
#endif  // ORCHID_SRC_INPUTLIB_BLOCKS_GENERALBLOCK_H
