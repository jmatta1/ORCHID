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
    toFileOutputQueue(toFileQueue), notTerminated(true), lg(OrchidLog::get())
{
    long long int refreshNanoseconds = (1000000000/refreshRate);
    this->refreshPeriod = boost::chrono::nanoseconds(refreshNanoseconds);
}


void SlowControlsThread::operator ()()
{
    long long int loopCount = 0;
    InterThread::SlowControlsThreadState lastState = InterThread::SlowControlsThreadState::Stopped;
    InterThread::SlowControlsThreadState currState;
    while(notTerminated)
    {
        //TODO: add actual querying of the temperature controller
        boost::this_thread::sleep_for(this->refreshPeriod);
        currState = this->sctControl->getState();
        switch(currState)
        {
        case InterThread::SlowControlsThreadState::Terminate:
            BOOST_LOG_SEV(lg, Information) << "SC Thread: Terminating";
            //stop looping
            this->notTerminated = false;
            break;
        case InterThread::SlowControlsThreadState::Stopped:
            //if we are stopped, then wait to not be stopped
            BOOST_LOG_SEV(lg, Information) << "SC Thread: Stopped";
            this->sctControl->waitForNewState();
            break;
        case InterThread::SlowControlsThreadState::Polling:
            //here we poll the mpod but do not send any voltage events
            if(lastState != currState)
            {
                loopCount = 0;
                BOOST_LOG_SEV(lg, Information) << "SC Thread: Started Polling";
                //first time we enter into a mode, read *everything*
                //poll the mpod
                this->mpodReader->readAll();
                //publish the data for the UI thread but do not bother with writing it
                BOOST_LOG_SEV(lg, Information) << "SC Thread: Calling Read Voltage Data";
                slowData->readVoltageData(this->mpodReader->voltageData);
            }
            else if((loopCount % 30)==0)
            {
                BOOST_LOG_SEV(lg, Information) << "SC Thread: Still Polling (# Loops: "<<loopCount<<")";
                //after that only read the things that actually change
                //poll the mpod
                this->mpodReader->readActive();
                //publish the data for the UI thread but do not bother with writing it
                BOOST_LOG_SEV(lg, Information) << "SC Thread: Calling Read Voltage Data";
                slowData->readVoltageData(this->mpodReader->voltageData);
            }
            else
            {//after that only read the things that actually change
                //poll the mpod
                this->mpodReader->readActive();
                //publish the data for the UI thread but do not bother with writing it
                BOOST_LOG_SEV(lg, Information) << "SC Thread: Calling Read Voltage Data";
                slowData->readVoltageData(this->mpodReader->voltageData);
            }
            break;
        case InterThread::SlowControlsThreadState::Writing:
            if(lastState != currState)
            {
                loopCount = 0;
                BOOST_LOG_SEV(lg, Information) << "SC Thread: Started Writing";
                //first time we enter into a mode, read *everything*
                //poll the mpod
                this->mpodReader->readAll();
            }
            else if((loopCount % 120)==0)
            {
                BOOST_LOG_SEV(lg, Information) << "SC Thread: Still Writing (# Loops: "<<loopCount<<")";
                //after that only read the things that actually change
                this->mpodReader->readActive();
            }
            else
            {//after that only read the things that actually change
                this->mpodReader->readActive();
                
            }
            
            //publish the data for the UI Thread
            BOOST_LOG_SEV(lg, Information) << "SC Thread: Calling Read Voltage Data";
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
        lastState = currState;
        ++loopCount;
    }
    //just before we exit, tell the controller that we have exited
    this->sctControl->setDone();
}

}
