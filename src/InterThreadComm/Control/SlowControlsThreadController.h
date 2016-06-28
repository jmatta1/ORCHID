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
        threadDone(false){}
    ~SlowControlsThreadController(){}
    
    //functions for the slow controls thread to access
    SlowControlsThreadState getState(){return this->currentState.load();}
    void waitForNewState();
    void setDone(){this->threadDone.store(true);}
    
    //functions for the UI thread to access
    void setToTerminate()
    {
        this->currentState.store(SlowControlsThreadState::Terminate);
        this->waitCondition.notify_all();
    }
    void setToStop()
    {
        this->currentState.store(SlowControlsThreadState::Stopped);
        this->waitCondition.notify_all();
    }
    void setToPolling()
    {
        this->currentState.store(SlowControlsThreadState::Polling);
        this->waitCondition.notify_all();
    }
    void setToWriting()
    {
        this->currentState.store(SlowControlsThreadState::Writing);
        this->waitCondition.notify_all();
    }
    bool isWaiting(){return this->threadWaiting.load();}
    bool isDone(){return this->threadDone.load();}
    
private:
    std::atomic<SlowControlsThreadState> currentState;
    
    //thread syncronization
    boost::mutex waitMutex;
    boost::condition_variable waitCondition;
    std::atomic_bool threadWaiting;
    
    //termination checking
    std::atomic_bool threadDone;
};

}

#endif //ORCHID_SRC_INTERTHREADCOMM_CONTROLS_SLOWCONTROLSTHREADCONTROL_H
