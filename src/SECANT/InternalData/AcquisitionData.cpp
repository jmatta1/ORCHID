/***************************************************************************//**
********************************************************************************
**
** @author James Till Matta
** @date 27 Apr, 2016
** @brief Implementation for AcquisitionData
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** @details Holds the implementation of the class that is used to accumulate
** read sizes from the digitizers
**
********************************************************************************
*******************************************************************************/
#include"AcquisitionData.h"
// includes for C system headers
// includes for C++ system headers
// includes from other libraries
// includes from ORCHID
namespace Secant
{

namespace OperationData
{

AcquisitionData::AcquisitionData(int numMods): numModules(numMods)
{
    //allocate and initialize the array of atomic integers
    dataSizes = new std::atomic_ullong[numModules];
    bufferCount = new std::atomic_uint[numModules];
    for(int i=0; i<numModules; ++i)
    {
        dataSizes[i].store(0);
        bufferCount[i].store(0);
    }
}


void AcquisitionData::clearData()
{
    //clear the array of atomic integers
    for(int i=0; i<this->numModules; ++i)
    {
        dataSizes[i].store(0);
        bufferCount[i].store(0);
    }
}

}
}
