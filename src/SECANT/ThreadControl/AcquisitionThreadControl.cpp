/***************************************************************************//**
********************************************************************************
**
** @file DigitizerThreadControl.h
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
** @details Implementation file for the AcquisitionThreadControl class
**
********************************************************************************
*******************************************************************************/
#include"AcquisitionThreadControl.h"
// includes for C system headers
// includes for C++ system headers
// includes from other libraries
// includes from ORCHID

namespace Secant
{

namespace ThreadCtrl
{

void AcquisitionThreadControl::waitForChange()
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
    while(this->acqState.load() == AcquisitionThreadState::Stopped)
    {
        acqThreadWaitCondition.wait(waitLock);
    }
    //the lock will release on deconstruction
}

}
}
