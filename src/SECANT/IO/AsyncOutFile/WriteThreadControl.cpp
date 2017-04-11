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
** @details Holds the definition of the write thread controller
**
********************************************************************************
*******************************************************************************/
#include"WriteThreadControl.h"
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

void WriteThreadControl::waitForChange()
{
    //lock to prevent state change races and to pass to wait condition
    boost::unique_lock<boost::mutex> waitLock(waitMutex);
    //tell things we are stopping
    acknowledgeStop();
    //to deal with the possibility of spurious wakeups, check if we are in the
    //stopped state with a loop condition. That way if we wake up spuriously,
    //we automatically start waiting again. This also prevents the case where
    //The system is set to stop, gets to just before the lock, has state changed
    //again and it waits until it somehow wakes up when it should have been doing
    //something else
    while(this->writeState.load() == WriteThreadMode::Stopped)
    {
        //waiting on the condition variable unlocks the lock allowing state changes
        writeThreadWaitCondition.wait(waitLock);
        //waking from the condition variable locks the lock preventing state changes while threads are waking
    }
    //the lock will release on deconstruction
}


}
}
}
