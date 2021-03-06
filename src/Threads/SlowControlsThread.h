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
** @details Holds the definition of the slow controls thread class used to poll
** the slow controls like voltage and temperature
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHID_SRC_THREADS_SLOWCONTROLSTHREAD_H
#define ORCHID_SRC_THREADS_SLOWCONTROLSTHREAD_H

// includes for C system headers
// includes for C++ system headers
// includes from other libraries
#include<boost/chrono.hpp>
// includes from ORCHID
#include"Hardware/HVLib/MpodReader.h"
#include"InterThreadComm/Data/SlowData.h"
#include"InterThreadComm/Control/SlowControlsThreadController.h"
#include"Utility/CommonTypeDefs.h"
#include"Utility/OrchidLogger.h"

namespace Threads
{

class SlowControlsThread
{
public:
    //construction and destruction
    SlowControlsThread(SlowControls::MpodReader* mRead, InterThread::SlowData* slDat,
                       InterThread::SlowControlsThreadController* sctCtrl,
                       Utility::ToFileMultiQueue* toFileQueue, int refreshRate);
    ~SlowControlsThread(){}
    
    // the function that makes this object callable which actually executes the thread
    void operator() ();
private:
    SlowControls::MpodReader* mpodReader;
    InterThread::SlowData* slowData;
    InterThread::SlowControlsThreadController* sctControl;
    Utility::ToFileMultiQueue* toFileOutputQueue;
    boost::chrono::nanoseconds refreshPeriod;
    
    bool notTerminated;
    
    //logger
    boost::log::sources::severity_logger_mt<LogSeverity>& lg;
};

}

#endif //ORCHID_SRC_THREADS_SLOWCONTROLSTHREAD_H
