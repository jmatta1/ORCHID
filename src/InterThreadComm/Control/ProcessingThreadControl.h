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
    void acknowledgeTerminate(){++termAckCount;}

    //functions to be accessed by the UI thread
    void setToRunning(){procState.store(ProcessingThreadState::Running); procThreadWaitCondition.notify_all();}
    void setToStopped(){procState.store(ProcessingThreadState::Stopped);}
    void setToTerminate(){procState.store(ProcessingThreadState::Terminate); procThreadWaitCondition.notify_all();}
    
    int getThreadsWaiting(){return waitCount.load();}
    int getThreadsTerminated(){return termAckCount.load();}
    
private:
    std::atomic<ProcessingThreadState> procState;
    std::atomic_uint termAckCount;
    
    std::atomic_uint waitCount;
    boost::condition_variable procThreadWaitCondition;//we create the mutex for this on the fly so everyone can wake up simultaneously
};

}
#endif //ORCHID_SRC_INTERTHREADCOMM_CONTROL_PROCESSINGTHREADCONTROL_H
