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
** @details Implementation file for the DigitizerThreadControl class
**
********************************************************************************
*******************************************************************************/
#include"AcquisitionThreadControl.h"
// includes for C system headers
// includes for C++ system headers
// includes from other libraries
// includes from ORCHID

namespace InterThread
{

void AcquisitionThreadControl::waitForStart()
{
    //creation of the mutexes happen rarely, so we can suck it up
    //since we can have multiple threads waiting here simultaneously, if they all
    //used the same mutex they would serlialize getting out of the function,
    //so instead we create the threads on the fly
    boost::mutex waitMutex;
    boost::unique_lock waitLock(waitMutex);
    while(this->acqState.load() == AcquisitionThreadState::Stopped)
    {
        ++waitCount;
        acqThreadWaitCondition.wait(waitLock);
        --waitCount;
    }
    //the lock will release on deconstruction
}

}