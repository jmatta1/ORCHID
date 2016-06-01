/***************************************************************************//**
********************************************************************************
**
** @file UIThread.h
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
** @details Holds the definition of the slow controls class used to poll the
** voltage and temperature controls
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
#include"SlowControls/HVLib/MpodReader.h"
#include"InterThreadComm/Data/SlowData.h"

namespace Threads
{

class SlowControlsThread
{
public:
    //construction and destruction
    SlowControlsThread(SlowControls::MpodReader* mRead, InterThread::SlowData* slDat, int refreshRate);
    ~SlowControlsThread();
    
    // the function that makes this object callable which actually executes the thread
    void operator() ();
private:
    SlowControls::MpodReader* mpodReader;
    InterThread::SlowData* slowData;
    boost::chrono::nanoseconds refreshPeriod;
};

}

#endif //ORCHID_SRC_THREADS_SLOWCONTROLSTHREAD_H
