/***************************************************************************//**
********************************************************************************
**
** @author James Till Matta
** @date 09 April, 2017
** @brief
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** @details Holds the defintion of the singleton class that coordinates the pool
** of threads that perform file writes. Also holds the write mode enumeration
**
********************************************************************************
*******************************************************************************/
#include"WriteThread.h"
// includes for C system headers
// includes for C++ system headers
// includes from other libraries
// includes from ORCHID
#include"WriteThreadControl.h"
#include"WriteMultiQueue.h"

namespace SECANT
{
namespace IO
{
namespace AsyncFile
{

WriteThread::WriteThread(WriteThreadMode mode):
    writeMode(mode), writeQueue(WriteMultiQueue::getInstance()),
    controller(WriteThreadControl::getInstance()){}

void WriteThread::operator ()()
{
    bool notTerminated = true;
    while(notTerminated)
    {
        switch(controller.getCurrentState())
        {
        case WriteThreadState::Terminate:
            this->notTerminated = false;
            break;
        case WriteThreadState::Running:
            controller.acknowledgeRunning();
            if(mode==WriteThreadMode::Greedy)
            { greedyWriteLoop();}
            else
            { austereWriteLoop();}
            // if we are here, clear the writes that are still incoming and then
            // check state flags
            clearWriteBufferLoop();
            break;
        case WriteThreadState::Stopped:
            controller.waitForChange(); // stop gets acked in wait for change
            break;
        }
    }
    //make certain the queues are cleared before terminating
    
    controller.acknowledgeTerminate();
}

void WriteThread::greedyWriteLoop()
{
    
}

void WriteThread::austereWriteLoop()
{

}

}
}
}
