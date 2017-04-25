/***************************************************************************//**
********************************************************************************
**
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
** @details Holds the implementation of the struct that stores the data
** garnished from the slow controls like voltage and temperature
**
********************************************************************************
*******************************************************************************/

#include"SlowControlsData.h"
// includes for C system headers
// includes for C++ system headers
// includes from other libraries
// includes from ORCHID

namespace SECANT
{

namespace OperationData
{

SlowControlsData::SlowControlsData(int numScDevices):numSlowControlsDevices(numScDevices)
{
    readTimes = new std::atomic_ullong[numSlowControlsDevices];
    readSizes = new std::atomic_ullong[numSlowControlsDevices];
    readCount = new std::atomic_uint[numSlowControlsDevices];
    for(int i=0; i<numSlowControlsDevices; ++i)
    {
        readTimes[i].store(0ULL);
        readSizes[i].store(0ULL);
        readCount[i].store(0);
    }
}

void SlowControlsData::clearData()
{
    for(int i=0; i<numSlowControlsDevices; ++i)
    {
        readTimes[i].store(0ULL);
        readSizes[i].store(0ULL);
        readCount[i].store(0);
    }
}

}
}
