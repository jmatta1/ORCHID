/***************************************************************************//**
********************************************************************************
**
** @file CsvBlock.h
** @author James Till Matta
** @date 25 Apr, 2016
** @brief
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** @details file with the template class for storing parsed csv data
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHID_SRC_INPUTLIB_BLOCKS_CSVBLOCK_H
#define ORCHID_SRC_INPUTLIB_BLOCKS_CSVBLOCK_H

// includes for C system headers
// includes for C++ system headers
#include<vector>
#include<iostream>
// includes from other libraries
// includes from ECHOS

namespace InputParser
{


class MpodCsvData
{
public:
    // a vector for each column
    std::vector<int>   board;
    std::vector<int>   channel;
    std::vector<bool>  online;
    std::vector<float> voltage;
    std::vector<float> maxCurrent;
    
    // now an adder for each vector
    void addBoard(int input);
    void addChannel(int input);
    void addOnline(bool input);
    void addVoltage(float input);
    void addMaxCurrent(float input);
    
    bool validate();
    void printValidationErrors();

    // ostream operator to allow outputting of the block class
    friend std::ostream& operator<<(std::ostream& os, MpodCsvData const& mcd);

};

}
#endif  // ORCHID_SRC_INPUTLIB_BLOCKS_CSVBLOCK_H
