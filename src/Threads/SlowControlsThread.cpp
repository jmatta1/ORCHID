/***************************************************************************//**
********************************************************************************
**
** @file SlowControlsThread.h
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
** @details Holds the definition of the slow controls class used to poll the
** voltage and temperature controls
**
********************************************************************************
*******************************************************************************/
#include"SlowControlsThread.h"
// includes for C system headers
// includes for C++ system headers
// includes from other libraries
#include<boost/thread.hpp>
// includes from ORCHID

namespace Threads
{
SlowControlsThread::SlowControlsThread(SlowControls::MpodReader* mRead,
                                       InterThread::SlowData* slDat,
                                       InterThread::SlowControlsThreadController* sctCtrl,
                                       int refreshRate):
    mpodReader(mRead), slowData(slDat), sctControl(sctCtrl), notTerminated(true)
{
    long long int refreshNanoseconds = (1000000000/refreshRate);
    this->refreshPeriod = boost::chrono::nanoseconds(refreshNanoseconds);
}


void SlowControlsThread::operator ()()
{
    while(notTerminated)
    {
        //TODO: add actual querying of the temperature controller
        boost::this_thread::sleep_for(this->refreshPeriod);
        switch(this->sctControl->getState())
        {
        case InterThread::SlowControlsThreadState::Terminate:
            //stop looping
            this->notTerminated = false;
            break;
        case InterThread::SlowControlsThreadState::Stopped:
            //if we are stopped, then wait to not be stopped
            this->sctControl->waitForNewState();
            break;
        case InterThread::SlowControlsThreadState::Polling:
            //here we poll the mpod but do not send any voltage events
            this->mpodReader->readAll();
            slowData->readVoltageData(this->mpodReader->voltageData);
            break;
        case InterThread::SlowControlsThreadState::Writing:
            this->mpodReader->readAll();
            slowData->readVoltageData(this->mpodReader->voltageData);
            //TODO: write code to package slow controls data for event queue
            break;
        }
    }
    //just before we exit, tell the controller that we have exited
    this->sctControl->setDone();
}

}
