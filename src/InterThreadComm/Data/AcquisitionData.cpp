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

RateData::RateData(int numDigiChan): numDigitizerChannels(numDigiChan)
{
    this->triggers = new std::atomic_uint[this->numDigitizerChannels];
    for(int i=0; i<this->numDigitizerChannels; ++i)
    {
        triggers[i].store(0);
    }
}

RateData::~RateData()
{
    delete[] this->triggers;
}

void RateData::clearTrigs()
{
    for(int i=0; i<this->numDigitizerChannels; ++i)
    {
        triggers[i].store(0, std::memory_order_relaxed);
    }
}

void RateData::addTrigs(int chan, unsigned count)
{
    triggers[chan].fetch_add(count, std::memory_order_relaxed);
}

}

