/***************************************************************************//**
********************************************************************************
**
** @file RateData.cpp
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
** @details Holds the implementation of the class that is used to accumulate
** trigger information from the digitizers, that in turn is used to calculate
** trigger rates by the ui
**
********************************************************************************
*******************************************************************************/
#include"AcquisitionData.h"
// includes for C system headers
// includes for C++ system headers
// includes from other libraries
// includes from ORCHID
namespace InterThread
{

AcquisitionData::AcquisitionData(int numDigiChan, int numMods):
    numChannels(numDigiChan), numModules(numMods)
{
    this->triggers = new std::atomic_uint[this->numChannels];
    for(int i=0; i<this->numChannels; ++i)
    {
        triggers[i].store(0);
    }
    this->dataSizes = new std::atomic_uint[this->numModules];
    for(int i=0; i<this->numModules; ++i)
    {
        dataSizes[i].store(0);
    }
}


void AcquisitionData::clearData()
{
    for(int i=0; i<this->numChannels; ++i)
    {
        triggers[i].store(0, std::memory_order_relaxed);
    }
    for(int i=0; i<this->numModules; ++i)
    {
        dataSizes[i].store(0, std::memory_order_relaxed);
    }
}

}

