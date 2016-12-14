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

class SlowControlsThreadControl
{
public:
    //construction and destruction
    SlowControlsThreadControl():
        currentState(SlowControlsThreadState::Stopped), threadWriting(false),
        threadPolling(false), threadStopped(false), threadTerminated(false){}
    ~SlowControlsThreadControl(){}
    
    //functions for the slow controls thread to access
    SlowControlsThreadState getState(){return this->currentState.load();}
    void waitForNewState();
    void slowControlsThreadSleep(const boost::chrono::nanoseconds& sleepTime);
    void acknowledgeTerminate(){this->threadTerminated.store(true);}
    void acknowledgePolling(){threadPolling.store(false);}
    void acknowledgeWriting(){threadWriting.store(false);}
    
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
            //check to make sure the state was not already terminate
            if(currentState.load() != SlowControlsThreadState::Terminate) threadTerminated.store(false);
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
            //check to make sure the state was not already Stopped
            if(currentState.load() != SlowControlsThreadState::Stopped) threadStopped.store(false);
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
            //check to make sure the state was not already polling
            if(currentState.load() != SlowControlsThreadState::Polling) threadPolling.store(false);
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
            //check to make sure the state was not already writing
            if(currentState.load() != SlowControlsThreadState::Writing) threadWriting.store(false);
            this->currentState.store(SlowControlsThreadState::Writing);
        }
        this->waitCondition.notify_all();
    }
    bool isWriting(){return this->threadWriting.load();}
    bool isPolling(){return this->threadPolling.load();}
    bool isStopped(){return this->threadStopped.load();}
    bool isTerminated(){return this->threadTerminated.load();}
    
private:
    void acknowledgeStop(){threadStopped.store(false);}
    
    std::atomic<SlowControlsThreadState> currentState;
    //termination checking
    std::atomic_bool threadTerminated;
    std::atomic_bool threadStopped;
    std::atomic_bool threadPolling;
    std::atomic_bool threadWriting;
    
    //thread syncronization
    boost::mutex waitMutex;
    boost::condition_variable waitCondition;
};

}

#endif //ORCHID_SRC_INTERTHREADCOMM_CONTROLS_SLOWCONTROLSTHREADCONTROL_H
