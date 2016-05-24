/***************************************************************************//**
********************************************************************************
**
** @file UIThread.h
** @author James Till Matta
** @date 27 Apr, 2016
** @brief
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** @details Holds the definition of the class that is run by main to handle the
** user interface
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHID_SRC_THREADS_UITHREAD_H
#define ORCHID_SRC_THREADS_UITHREAD_H

// includes for C system headers
// includes for C++ system headers
// includes from other libraries
#include<boost/chrono.hpp>
// includes from ORCHID
#include"SlowControls/SlowData.h"
namespace Threads
{
class UIThread
{
public:
    UIThread(SlowControls::SlowData* slDat, int refreshFrequency);
    ~UIThread(){}

    //this is the function that is called by boost::thread when making a thread
    void operator() ();
private:
    //we may have a pointer to the slowData but we do not own it so no deletions
    // *vader says shaking his finger at Boba Fett*
    SlowControls::SlowData* slowData;
    boost::chrono::nanoseconds refreshPeriod;
};

}
#endif //ORCHID_SRC_THREADS_UITHREAD_H
