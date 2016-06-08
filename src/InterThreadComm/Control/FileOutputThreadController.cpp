/***************************************************************************//**
********************************************************************************
**
** @file FileOutputThreadController.cpp
** @author James Till Matta
** @date 08 June, 2016
** @brief
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** @details Holds the implementation of the file output thread controller class
**
********************************************************************************
*******************************************************************************/
#include"FileOutputThreadController.h"
// includes for C system headers
// includes for C++ system headers
// includes from other libraries
// includes from ORCHID

namespace InterThread
{

void FileOutputThreadController::waitForNewState()
{
    //lock the wait mutex
    boost::unique_lock<boost::mutex> waitLock(this->waitMutex);
    //store the current state to check against
    FileOutputThreadState oldState = this->currentState.load();
    //use the condition variable to wait until the thread is woken
    //since spurious wake up event are possible put the wait inside a loop to
    //check for the condition that made us wait in the first place
    while(oldState == this->currentState.load())
        this->waitCondition.wait(waitLock);
}

}
