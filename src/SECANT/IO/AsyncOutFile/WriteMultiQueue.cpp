/***************************************************************************//**
********************************************************************************
**
** @author James Till Matta
** @date 19 April, 2017
** @brief Holds the implementation of the singleton class that handles queueing
** of writes from the known open files
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
********************************************************************************
*******************************************************************************/

#include"WriteMultiQueue.h"
// includes for C system headers
// includes for C++ system headers
#include<cassert>
// includes from other libraries
// includes from ORCHID

namespace SECANT
{
namespace IO
{
namespace AsyncFile
{
WriteMultiQueue::WriteMultiQueue(int numFiles, int bufferSize, int numBuffers)
{
    assert(numBuffers<MaximumWriteQueueSize);
    assert(numFiles<numBuffers);
    
}

}
}
}
