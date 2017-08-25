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
#include"InterThreadComm/Data/AcquisitionData.h"
#include"Utility/CommonTypeDefs.h"
#include"Utility/OrchidLogger.h"

namespace Threads
{

class ProcessingThread
{
public:
    ProcessingThread(InterThread::ProcessingThreadControl* prCtrl,
                     Utility::ToProcessingQueuePair* procQueue,
                     Utility::ToFileMultiQueue* eventQueue,
                     InterThread::AcquisitionData* acqDat,
                     int thrdNum):
        controller(prCtrl), dataInputQueue(procQueue), toFileOutputQueue(eventQueue),
        acqData(acqDat),notTerminated(true), threadNumber(thrdNum), lg(OrchidLog::get()) {}
    ~ProcessingThread(){}//delete nothing since we own nothing
    
    void operator()();
    
private:
    void doProcessingLoop();
    void emptyProcessingBuffer();
    void processDataBuffer(Utility::ToProcessingBuffer* buffer);
    int processEvent(Utility::ToProcessingBuffer* buffer, int startOffset);
    
    
    InterThread::ProcessingThreadControl* controller;
    Utility::ToProcessingQueuePair* dataInputQueue;
    Utility::ToFileMultiQueue* toFileOutputQueue;
    InterThread::AcquisitionData* acqData;
    
    bool notTerminated;
    int threadNumber;
    long long counters[16];
    
    //logger
    boost::log::sources::severity_logger_mt<LogSeverity>& lg;
};

}

#endif //ORCHID_SRC_THREADS_PROCESSINGTHREAD_H
