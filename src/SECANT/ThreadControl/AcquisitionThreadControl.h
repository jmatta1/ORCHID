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

namespace Secant
{

namespace ThreadCtrl
{
/**
 * @ingroup SecantThreadControl
 * @brief Represents the possible states of Acquisition threads
 */
enum class AcquisitionThreadState : char 
{
    Acquiring, ///<Acquisition thread is running and taking data
    Stopped, ///<Acquisition thread is stopped and waiting for a state change
    Terminate ///<Acquisition thread will clean up and exit once its state is set to this
};

/*!
 * @class AcquisitionThreadControl
 * @ingroup SecantThreadControl
 * @brief This class is used to control operation of acquisition threads
 * @author James Till Matta
 * 
 * This class holds the state variables and logic necessary to control the
 * operation of the acquisition threads SECANT is running
 */
class AcquisitionThreadControl
{
public:
    /**
     * @brief Create the thread control in the stopped state.
     * 
     * When this object is passed to the acquisition threads upon their
     * creation, they will be in the stopped state after they initialize
     */
    AcquisitionThreadControl():
        acqState(AcquisitionThreadState::Stopped), termAckCount(0),
        stopAckCount(0), startAckCount(0) {}
    
    //functions to be accessed by the acquisition threads
    /**
     * @brief Retrieve the currently set state for acquisition threads
     * @return The stored state for acquisition threads
     */
    AcquisitionThreadState getCurrentState(){return acqState.load();}
    
    /**
     * @brief Allows acquisition threads to pause for a state change
     * 
     * When placed in the stopped mode, acquisition threads use this function to
     * pause their operation on a condition variable. When the state is changed
     * all subscribers to that condition variable are woken
     */
    void waitForChange();
    
    /**
     * @brief Used by acquisition threads to declare their termination
     * 
     * Acquisition threads call this function immediately before they leave
     * their operator() block
     */
    void acknowledgeTerminate(){termAckCount.fetch_add(1);}
    
    /**
     * @brief Used by acquisition threads to declare they have started
     * 
     * Acquisition threads call this function immediately before they enter the
     * cycle of getting data and checking for state changes
     */
    void acknowledgeStart(){startAckCount.fetch_add(1);}
    
    //functions to be accessed by controlling threads
    /**
     * @brief Changes the desired state to Acquiring
     * 
     * This function should not be 'hammered' if it is avoidable as, to prevent
     * races with acquisition threads going into a condition var, it locks a
     * mutex.
     */
    void setToAcquiring()
    {
        //enter an artificial block to create and destroy lock before we notify
        {
            boost::unique_lock<boost::mutex> lock(this->waitMutex);
            if(acqState.load() != AcquisitionThreadState::Acquiring) startAckCount.store(0);
            acqState.store(AcquisitionThreadState::Acquiring);
        }
        acqThreadWaitCondition.notify_all();
    }
    /**
     * @brief Changes the desired state to Stopped
     * 
     * This function should not be 'hammered' if it is avoidable as, to prevent
     * races with acquisition threads going into a condition var, it locks a
     * mutex.
     */
    void setToStopped()
    {
        //enter an artificial block to create and destroy lock before we notify
        {
            boost::unique_lock<boost::mutex> lock(this->waitMutex);
            if(acqState.load() != AcquisitionThreadState::Stopped) stopAckCount.store(0);
            acqState.store(AcquisitionThreadState::Stopped);
        }
        acqThreadWaitCondition.notify_all();
    }
    /**
     * @brief Changes the desired state to Terminate
     * 
     * This function should not be 'hammered' if it is avoidable as, to prevent
     * races with acquisition threads going into a condition var, it locks a
     * mutex.
     */
    void setToTerminate()
    {
        //enter an artificial block to create and destroy lock before we notify
        {
            boost::unique_lock<boost::mutex> lock(this->waitMutex);
            if(acqState.load() != AcquisitionThreadState::Terminate) termAckCount.store(0);
            acqState.store(AcquisitionThreadState::Terminate);
        }
        acqThreadWaitCondition.notify_all();
    }
    
    /**
     * @brief Returns the number of threads that are waiting in the condition var
     * @return The number of acq threads that have entered the condition variable
     */
    int getThreadsStopped(){return stopAckCount.load();}
    
    /**
     * @brief Returns the number of threads that have started getting data from their acq sources
     * @return The number of acq threads that have called acknowledgeStart()
     */
    int getThreadsStarted(){return startAckCount.load();}
    /**
     * @brief Returns the number of threads that have exitted their operator()
     * @return The number of threads that have called acknowledgeTerminate()
     */
    int getThreadsTerminated(){return termAckCount.load();}
    
private:
    //called by wait for change
    /**
     * @brief This is called by acquisition threads as they wait on the condition var
     * 
     * Acquisition threads call this function immediately after they lock the
     * waitMutex mutex and immediately before they enter the loop to keep them
     * waiting on the condition variable while in the stopped state
     */
    void acknowledgeStop(){stopAckCount.fetch_add(1);}

    std::atomic<AcquisitionThreadState> acqState;///<Stores the current/desired state of the acquisition threads
    std::atomic_uint termAckCount;///<Stores how many acq threads have acknowledged the change in state to AcquisitionThreadState::Terminate
    std::atomic_uint stopAckCount;///<Stores how many acq threads have acknowledged the change in state to AcquisitionThreadState::Stopped
    std::atomic_uint startAckCount;///<Stores how many acq threads have acknowledged the change in state to AcquisitionThreadState::Acquiring
    
    boost::mutex waitMutex; ///<Prevents state change races and allows acq threads to wait for a state change
    boost::condition_variable acqThreadWaitCondition; ///<Allows acq threads to wait for a state change
};

}
}
#endif //ORCHID_SRC_INTERTHREADCOMM_CONTROL_DIGITIZERTHREADCONTROL_H
