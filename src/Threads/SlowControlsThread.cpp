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
#include"Events/SlowControlsEvent.h"

namespace Threads
{
SlowControlsThread::SlowControlsThread(SlowControls::MpodReader* mRead,
                                       InterThread::SlowData* slDat,
                                       InterThread::SlowControlsThreadController* sctCtrl,
                                       Utility::ToFileMultiQueue* toFileQueue,
                                       int refreshRate):
    mpodReader(mRead), slowData(slDat), sctControl(sctCtrl),
    toFileOutputQueue(toFileQueue), notTerminated(true)
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
            //poll the mpod
            this->mpodReader->readAll();
            //publish the data for the UI thread but do not bother with writing it
            slowData->readVoltageData(this->mpodReader->voltageData);
            break;
        case InterThread::SlowControlsThreadState::Writing:
            //poll the mpod
            this->mpodReader->readAll();
            //publish the data for the UI Thread
            slowData->readVoltageData(this->mpodReader->voltageData);
            //first pop an empty event to hold the event
            Events::EventInterface* scEvent;
            this->toFileOutputQueue->producerPop<Utility::SlowControlsQueueIndex>(scEvent);
            //put the data into the event
            static_cast<Events::SlowControlsEvent*>(scEvent)->ReadVoltageData(this->mpodReader->voltageData);
            //push the event into the outgoing queue
            this->toFileOutputQueue->producerPush<Utility::SlowControlsQueueIndex>(static_cast<Events::EventInterface*>(scEvent));
            break;
        }
    }
    //just before we exit, tell the controller that we have exited
    this->sctControl->setDone();
}

}
