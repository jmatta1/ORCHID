/***************************************************************************//**
********************************************************************************
**
** @file SlowControlsThreadController.cpp
** @author James Till Matta
** @date 01 June, 2016
** @brief
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** @details Holds implementation of the slow controls thread controller class
**
********************************************************************************
*******************************************************************************/

#include"SlowControlsThreadControl.h"
// includes for C system headers
// includes for C++ system headers
// includes from other libraries
// includes from ORCHID

namespace Secant
{

namespace InterThreadCtrl
{

void SlowControlsThreadControl::waitForNewState()
{
    //lock the wait mutex
    boost::unique_lock<boost::mutex> waitLock(this->waitMutex);
    this->acknowledgeStop();
    //use the condition variable to wait until the thread is woken
    //since spurious wake up event are possible put the wait inside a loop to
    //check for the condition that made us wait in the first place
    while(this->currentState.load() == SlowControlsThreadState::Stopped)
    {
        this->waitCondition.wait(waitLock);
    }
}

void SlowControlsThreadControl::slowControlsThreadSleep(const boost::chrono::nanoseconds& sleepTime)
{
    //lock the wait mutex
    boost::unique_lock<boost::mutex> waitLock(this->waitMutex);
    //figure out the time that we need to sleep until
    auto endTime(boost::chrono::system_clock::now() + sleepTime);
    //set up the variable that will hold how the condition variable returned (timeout or something else)
    boost::cv_status condVarStatus(boost::cv_status::no_timeout);
    //store the current state to check against, this way if we are wakened by a state 
    SlowControlsThreadState oldState = this->currentState.load();
    //use the condition variable to sleep. We do this instead of this_thread::sleep because this way
    //another thread can wake us prematurely if there was a change of state while we are sleeping
    //allowing a faster response to startup and shutdown of acquisition. To handle this we have the
    //loop (which squashes spurious wakeups (however infrequent they are) by check two conditions, the first
    //condition is if the wait returned timeout, if it did then we woke up naturally and it is time to resume
    //if we did not get time out, but instead the old state is modified, then we have a condition where we are
    //being woke up early because of a condition change so we can respond to it. If neither condition is true
    //then we had a spurious wakeup and we need to go back to sleep until our chosen time
    while(oldState == this->currentState.load() && condVarStatus != boost::cv_status::timeout)
    {
        condVarStatus = this->waitCondition.wait_until(waitLock, endTime);
    }
}

}
}
