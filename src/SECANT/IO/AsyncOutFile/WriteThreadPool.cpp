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

WriteThreadPool::WriteThreadPool(int numWriteThreads, WriteThreadMode mode):
    threadController(WriteThreadControl::getInstance()), 
    writeQueues(WriteMultiQueue::getInstance()), writeMode(mode)
{
    for(int i = 0; i < numWriteThreads; ++i)
    {
        //by adding threads like this I think that I make it such that I do not
        //need to manage the resources for the threads or the callable objects
        writeThreads.add_thread(new boost::thread(WriteThread(writeMode)));
    }
    
    //wait until all threads have come to stop on the controller condition var
    //then move them into running mode so that they can wait for data/file
    //objects to come down the queue to them
    while( threadController.getStoppedCount() != numWriteThreads )
    {
        boost::this_thread::sleep_for(boost::chrono::microseconds(50));
    }
    //if we are here then all the threads are waiting on the wait condition,
    //set things to running so that we can get moving
    threadController.setToRunning();
}

}
}
}
