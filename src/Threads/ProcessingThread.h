/***************************************************************************//**
********************************************************************************
**
** @file ProcessingThread.h
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
** @details Definition file for the ProcessingThread callable
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHID_SRC_THREADS_PROCESSINGTHREAD_H
#define ORCHID_SRC_THREADS_PROCESSINGTHREAD_H
// includes for C system headers
// includes for C++ system headers
// includes from other libraries
// includes from ORCHID
#include"InterThreadComm/Control/ProcessingThreadControl.h"
#include"Utility/CommonTypeDefs.h"
#include"Utility/OrchidLogger.h"

namespace Threads
{

class AcquisitionThread
{
public:
    AcquisitionThread(InterThread::ProcessingThreadControl* prCtrl,
                      Utility::ToProcessingQueuePair* procQueue,
                      Utility::ToFileMultiQueue* eventQueue):
        controller(prCtrl), dataInputQueue(procQueue), eventOutputQueue(eventQueue),
        notTerminated(true),lg(OrchidLog::get()) {}
    ~AcquisitionThread(){}//delete nothing since we own nothing
    
    void operator()();
    
private:
    InterThread::ProcessingThreadControl* controller;
    Utility::ToProcessingQueuePair* dataInputQueue;
    Utility::ToFileMultiQueue* eventOutputQueue;
    
    Utility::ToProcessingBuffer* currentBuffer;
    
    bool notTerminated;
    
    //logger
    boost::log::sources::severity_logger_mt<LogSeverity>& lg;
};

}

#endif //ORCHID_SRC_THREADS_PROCESSINGTHREAD_H
