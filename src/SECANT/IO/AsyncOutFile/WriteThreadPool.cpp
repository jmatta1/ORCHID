/***************************************************************************//**
********************************************************************************
**
** @author James Till Matta
** @date 09 April, 2017
** @brief
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** @details Holds the implementation of the singleton class that coordinates
** the pool of threads that perform file writes. Also holds the write mode
** enumeration that is passed to write threads on startup
**
********************************************************************************
*******************************************************************************/
#include"WriteThreadPool.h"
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

/*
 * WriteMultiQueue& writeQueue; ///<Holds the write queues the threads pull from
    int threadCount; ///<stores the number of threads to use for concurrent asynchronous writing
    WriteThreadPoolMode writeMode; ///<Used to build threads with the appropriate operation mode
    WriteThread* threadCallables; ///<Stores the functors that are run by the threads
    boost::thread_group writeThreads; ///<Boost thread group object that stores the multiple thread objects doing writes
    */

WriteThreadPool::WriteThreadPool(int numWriteThreads, WriteThreadPoolMode mode):
    writeQueue(WriteMultiQueue::getInstance()), threadCount(numWriteThreads),
    writeMode(mode)
{
    threadCallables = new WriteThread*;
    for(int i = 0; i < threadCount; ++i)
    {
        threadCallables[i] = new WriteThread(writeMode);
        
    }
}

}
}
}
