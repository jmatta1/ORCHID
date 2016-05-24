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
#include<string>
// includes from other libraries
#include<boost/chrono.hpp>
// includes from ORCHID
#include"SlowControls/SlowData.h"
#include"DigiLib/RateData.h"
namespace Threads
{
class UIThread
{
public:
    UIThread(SlowControls::SlowData* slDat, FastData::RateData* rtDat, int refreshFrequency);
    ~UIThread(){}

    //this is the function that is called by boost::thread when making a thread
    void operator() ();
private:
    //Private Member Functions
    //This function is used to handle key presses
    void handleKeyPress(int inChar);
    //This function handles commands when the user presses enter
    void handleCommand();
    
    //Private Member Variables
    
    //we may have a pointer to these data but we do not own it so: No deletions!
    // *vader says shaking his finger at Boba Fett*
    SlowControls::SlowData* slowData;
    FastData::RateData* rateData;
    //amount of time to sleep for
    boost::chrono::nanoseconds refreshPeriod;
    //multiplier for calculating rate from number of triggers
    float rateMultiplier;
    //the actual refresh rate in Hz
    int refreshRate;
    //string to hold commands in
    std::string command;
    //string to hold persistent messages in
    std::string persistentMessage;
    //counter for number of refreshes persistent messages persist for
    long persistCount;
    //flag to continue or kill loop
    bool runLoop;
};

}
#endif //ORCHID_SRC_THREADS_UITHREAD_H
