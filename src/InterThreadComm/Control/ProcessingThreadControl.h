/***************************************************************************//**
********************************************************************************
**
** @file ProcessingThreadControl.h
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
** @details Definition file for the ProcessingThreadControl class
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHID_SRC_INTERTHREADCOMM_CONTROL_PROCESSINGTHREADCONTROL_H
#define ORCHID_SRC_INTERTHREADCOMM_CONTROL_PROCESSINGTHREADCONTROL_H
// includes for C system headers
// includes for C++ system headers
#include<atomic>
// includes from other libraries
#include<boost/thread.hpp>
// includes from ORCHID

namespace InterThread
{

enum class ProcessingThreadState : char {Running, Stopped, Terminate};

class ProcessingThreadControl
{
public:
    ProcessingThreadControl():procState(ProcessingThreadState::Stopped),
        termAckCount(0), waitCount(0) {}
    
    //functions to be accessed by the processing threads
    ProcessingThreadState getCurrentState(){return procState.load();}
    void waitForChange();
    void acknowledgeTerminate(){termAckCount.fetch_add(1);}

    //functions to be accessed by the UI thread
    void setToRunning(){changeState(ProcessingThreadState::Running);}
    void setToStopped(){changeState(ProcessingThreadState::Stopped);}
    void setToTerminate(){changeState(ProcessingThreadState::Terminate);}
    
    int getThreadsWaiting(){return waitCount.load();}
    int getThreadsTerminated(){return termAckCount.load();}
    
private:
    void changeState(ProcessingThreadState newState)
    {
        //artificial scope to force destruction of the unique_lock prior to notify all
        {
            boost::unique_lock<boost::mutex> lock(this->waitMutex);
            procState.store(newState);
        }
        procThreadWaitCondition.notify_all();
    }
    
    
    std::atomic<ProcessingThreadState> procState;
    std::atomic_uint termAckCount;
    
    std::atomic_uint waitCount;
    boost::mutex waitMutex;
    boost::condition_variable procThreadWaitCondition;
};

}
#endif //ORCHID_SRC_INTERTHREADCOMM_CONTROL_PROCESSINGTHREADCONTROL_H
