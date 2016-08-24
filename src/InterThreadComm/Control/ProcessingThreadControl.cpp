/***************************************************************************//**
********************************************************************************
**
** @file ProcessingThreadControl.h
** @author James Till Matta
** @date 27 June, 2016
** @brief
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** @details Implementation file for the ProcessingThreadControl class
**
********************************************************************************
*******************************************************************************/
#include"ProcessingThreadControl.h"
// includes for C system headers
// includes for C++ system headers
// includes from other libraries
// includes from ORCHID

namespace InterThread
{

void ProcessingThreadControl::waitForChange()
{
    //creation of the mutexes happen rarely, so we can suck it up
    //since we can have multiple threads waiting here simultaneously, if they all
    //used the same mutex they would serlialize getting out of the function,
    //so instead we create the threads on the fly
    boost::mutex waitMutex;
    boost::unique_lock<boost::mutex> waitLock(waitMutex);
    while(this->procState.load() == ProcessingThreadState::Stopped)
    {
        waitCount.fetch_add(1);
        procThreadWaitCondition.wait(waitLock);
        waitCount.fetch_add(-1);
    }
    //the lock will release on deconstruction
}

}
