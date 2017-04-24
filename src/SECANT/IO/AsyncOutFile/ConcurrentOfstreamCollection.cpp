/***************************************************************************//**
********************************************************************************
**
** @author James Till Matta
** @date 24 April, 2017
** @brief Holds the defintion of the singleton class allows write threads to
** access multiple underlying ConcurrentOfstreamWrappers
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
********************************************************************************
*******************************************************************************/
#include"ConcurrentOfstreamCollection.h"
// includes for C system headers
// includes for C++ system headers
// includes from other libraries
// includes from ORCHID
namespace SECANT
{
namespace IO
{
namespace AsyncFile
{

ConcurrentOfstreamCollection::ConcurrentOfstreamCollection(int numFiles):
    numberFiles(numFiles)
{
    collection = ConcurrentOfstreamWrapper[numberFiles];
}

}
}
}
