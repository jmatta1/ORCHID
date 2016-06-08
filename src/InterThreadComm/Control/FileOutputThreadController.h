/***************************************************************************//**
********************************************************************************
**
** @file FileOutputThreadController.h
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
** @details Holds the definition of the file output thread controller class
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHID_SRC_INTERTHREADCOMM_CONTROL_FILEOUTPUTTHREADCONTROLLER_H
#define ORCHID_SRC_INTERTHREADCOMM_CONTROL_FILEOUTPUTTHREADCONTROLLER_H

// includes for C system headers
// includes for C++ system headers
#include<atomic>
#include<boost/thread.hpp>
// includes from other libraries
// includes from ORCHID

namespace InterThread
{

enum class FileOutputThreadState : char {Terminate, Waiting, Writing};

class FileOutputThreadController
{
public:
    FileOutputThreadController():
        currentState(FileOutputThreadState::Waiting),
        threadDone(){}
    ~FileOutputThreadController(){}
    
    //functions for the file thread to access
    FileOutputThreadState getCurrentState(){return this->currentState.load();}
    void waitForNewState();
    void setThreadDone(){this->threadDone.store(true);}
    
    void setToTerminate()
    {
        this->currentState.store(FileOutputThreadState::Terminate);
        this->waitCondition.notify_all();
    }
    void setToWaiting()
    {
        this->currentState.store(FileOutputThreadState::Waiting);
        this->waitCondition.notify_all();
    }
    void setToWriting()
    {
        this->currentState.store(FileOutputThreadState::Writing);
        this->waitCondition.notify_all();
    }
    bool isDone(){return this->threadDone.load();}
private:
    //state variable
    std::atomic<FileOutputThreadState> currentState;
    
    //variables for waiting to wake up
    boost::mutex waitMutex;
    boost::condition_variable waitCondition;
    
    //termination checking
    std::atomic_bool threadDone;
};

}

#endif //ORCHID_SRC_INTERTHREADCOMM_CONTROL_FILEOUTPUTTHREADCONTROLLER_H
