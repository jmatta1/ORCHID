/***************************************************************************//**
********************************************************************************
**
** @file AcquisitionThreadControl.h
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
** @details Definition file for the DigitizerThreadControl class
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHID_SRC_INTERTHREADCOMM_CONTROL_DIGITIZERTHREADCONTROL_H
#define ORCHID_SRC_INTERTHREADCOMM_CONTROL_DIGITIZERTHREADCONTROL_H
// includes for C system headers
// includes for C++ system headers
#include<atomic>
// includes from other libraries
#include<boost/thread.hpp>
// includes from ORCHID

namespace InterThread
{

enum class AcquisitionThreadState : char {Acquiring, Stopped, Terminate};

class AcquisitionThreadControl
{
public:
    AcquisitionThreadControl():acqState(AcquisitionThreadState::Stopped),
        termAckCount(0), waitCount(0) {}
    
    //functions to be accessed by the acquisition threads
    AcquisitionThreadState getCurrentState(){return acqState.load();}
    void waitForChange();
    void acknowledgeTerminate(){++termAckCount;}

    //functions to be accessed by the UI thread
    void setToAcquiring(){acqState.store(AcquisitionThreadState::Acquiring); acqThreadWaitCondition.notify_all();}
    void setToStopped(){acqState.store(AcquisitionThreadState::Stopped);}
    void setToTerminate(){acqState.store(AcquisitionThreadState::Terminate); acqThreadWaitCondition.notify_all();}
    
    int getThreadsWaiting(){return waitCount.load();}
    int getThreadsTerminated(){return termAckCount.load();}
    
private:
    std::atomic<AcquisitionThreadState> acqState;
    std::atomic<int> termAckCount;
    
    std::atomic<int> waitCount;
    boost::condition_variable acqThreadWaitCondition;//we create the mutex for this on the fly so everyone can wake up simultaneously
    
};

}

#endif //ORCHID_SRC_INTERTHREADCOMM_CONTROL_DIGITIZERTHREADCONTROL_H
