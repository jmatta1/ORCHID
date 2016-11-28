/***************************************************************************//**
********************************************************************************
**
** @file SlowControlsThreadController.h
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
** @details Holds the definition of the slow controls thread controller class
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHID_SRC_INTERTHREADCOMM_CONTROLS_SLOWCONTROLSTHREADCONTROL_H
#define ORCHID_SRC_INTERTHREADCOMM_CONTROLS_SLOWCONTROLSTHREADCONTROL_H

// includes for C system headers
// includes for C++ system headers
#include<atomic>
#include<boost/chrono.hpp>
#include<boost/thread.hpp>
// includes from other libraries
// includes from ORCHID

namespace InterThread
{

enum class SlowControlsThreadState  : char {Terminate, Stopped, Polling, Writing};

class SlowControlsThreadController
{
public:
    //construction and destruction
    SlowControlsThreadController():
        currentState(SlowControlsThreadState::Stopped), threadWaiting(false),
        threadSleeping(false), threadDone(false){}
    ~SlowControlsThreadController(){}
    
    //functions for the slow controls thread to access
    SlowControlsThreadState getState(){return this->currentState.load();}
    void waitForNewState();
    void slowControlsThreadSleep(const boost::chrono::nanoseconds& sleepTime);
    void setDone(){this->threadDone.store(true);}
    
    //functions for the UI thread to access
    void setToTerminate()
    {
        //enter an artificial block to create and destroy lock before we notify
        //the lock prevents a weird data race that, in a very strange and possibly
        //incorrect usage mode, cause the slowControls thread to wait without
        //end. This would happen because it would start waiting for a change
        //(or to sleep) and after it locks the mutex, but before it grabs the
        //current state the state is changed. That would then cause it to think
        //that the changed state was the old state and it would wait for yet
        //another state change, this way it cannot have that happen
        {
            boost::unique_lock<boost::mutex> lock(this->waitMutex);
            this->currentState.store(SlowControlsThreadState::Terminate);
        }
        this->waitCondition.notify_all();
    }
    void setToStop()
    {
        //enter an artificial block to create and destroy lock before we notify
        //the lock prevents a weird data race that, in a very strange and possibly
        //incorrect usage mode, cause the slowControls thread to wait without
        //end. This would happen because it would start waiting for a change
        //(or to sleep) and after it locks the mutex, but before it grabs the
        //current state the state is changed. That would then cause it to think
        //that the changed state was the old state and it would wait for yet
        //another state change, this way it cannot have that happen
        {
            boost::unique_lock<boost::mutex> lock(this->waitMutex);
            this->currentState.store(SlowControlsThreadState::Stopped);
        }
        this->waitCondition.notify_all();
    }
    void setToPolling()
    {
        //enter an artificial block to create and destroy lock before we notify
        //the lock prevents a weird data race that, in a very strange and possibly
        //incorrect usage mode, cause the slowControls thread to wait without
        //end. This would happen because it would start waiting for a change
        //(or to sleep) and after it locks the mutex, but before it grabs the
        //current state the state is changed. That would then cause it to think
        //that the changed state was the old state and it would wait for yet
        //another state change, this way it cannot have that happen
        {
            boost::unique_lock<boost::mutex> lock(this->waitMutex);
            this->currentState.store(SlowControlsThreadState::Polling);
        }
        this->waitCondition.notify_all();
    }
    void setToWriting()
    {
        //enter an artificial block to create and destroy lock before we notify
        //the lock prevents a weird data race that, in a very strange and possibly
        //incorrect usage mode, cause the slowControls thread to wait without
        //end. This would happen because it would start waiting for a change
        //(or to sleep) and after it locks the mutex, but before it grabs the
        //current state the state is changed. That would then cause it to think
        //that the changed state was the old state and it would wait for yet
        //another state change, this way it cannot have that happen
        {
            boost::unique_lock<boost::mutex> lock(this->waitMutex);
            this->currentState.store(SlowControlsThreadState::Writing);
        }
        this->waitCondition.notify_all();
    }
    bool isWaiting(){return this->threadWaiting.load();}
    bool isSleeping(){return this->threadSleeping.load();}
    bool isDone(){return this->threadDone.load();}
    
private:
    std::atomic<SlowControlsThreadState> currentState;
    
    //thread syncronization
    boost::mutex waitMutex;
    boost::condition_variable waitCondition;
    std::atomic_bool threadWaiting;
    std::atomic_bool threadSleeping;
    
    //termination checking
    std::atomic_bool threadDone;
};

}

#endif //ORCHID_SRC_INTERTHREADCOMM_CONTROLS_SLOWCONTROLSTHREADCONTROL_H
