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
** @details Definition file for the AcquisitionThreadControl class
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
#include"Utility/OrchidLogger.h"

namespace InterThread
{

enum class AcquisitionThreadState : char {Acquiring, Stopped, Terminate};

class AcquisitionThreadControl
{
public:
    AcquisitionThreadControl():acqState(AcquisitionThreadState::Stopped),
        termAckCount(0), waitCount(0), lg(OrchidLog::get()) {}
    
    //functions to be accessed by the acquisition threads
    AcquisitionThreadState getCurrentState(){return acqState.load();}
    void waitForChange();
    void acknowledgeTerminate(){++termAckCount;}

    //functions to be accessed by the UI thread
    void setToAcquiring(){changeState(AcquisitionThreadState::Acquiring);}
    void setToStopped(){BOOST_LOG_SEV(lg, Information) << "ATC Set to Stop"; changeState(AcquisitionThreadState::Stopped);}
    void setToTerminate(){changeState(AcquisitionThreadState::Terminate);}
    
    int getThreadsWaiting(){return waitCount.load();}
    int getThreadsTerminated(){return termAckCount.load();}
    
private:
    void changeState(AcquisitionThreadState newState)
    {
        //artificial scope to force destruction of the unique_lock prior to notify all
        {
            boost::unique_lock<boost::mutex> lock(this->waitMutex);
            acqState.store(newState);
        }
        acqThreadWaitCondition.notify_all();
    }
    std::atomic<AcquisitionThreadState> acqState;
    std::atomic_uint termAckCount;
    
    std::atomic_uint waitCount;
    boost::mutex waitMutex;
    boost::condition_variable acqThreadWaitCondition;
    boost::log::sources::severity_logger_mt<LogSeverity>& lg;
};

}

#endif //ORCHID_SRC_INTERTHREADCOMM_CONTROL_DIGITIZERTHREADCONTROL_H
