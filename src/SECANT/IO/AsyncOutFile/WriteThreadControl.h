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
** @details Holds the definition of the write thread controller
**
********************************************************************************
*******************************************************************************/
#ifndef SECANT_SRC_SECANT_IO_ASYNCOUTFILE_WRITETHREADCONTROL_H
#define SECANT_SRC_SECANT_IO_ASYNCOUTFILE_WRITETHREADCONTROL_H

// includes for C system headers
// includes for C++ system headers
#include<atomic>
// includes from other libraries
#include<boost/thread.hpp>
// includes from ORCHID


namespace SECANT
{
namespace IO
{
namespace AsyncFile
{

/**
 * @brief The WriteThreadMode enum covers the two modes that write threads need
 * to be in. Running, and set to Terminate
 */
enum class WriteThreadState: char
{
    Running, ///<In this state the threads are running and either waiting for data or writing
    Stopped, ///<In this state the threads are waiting on a wait condition to be told to resume
    Terminate ///<When write threads see this mode they shut down and terminate
};

/**
 * @class WriteThreadControl
 * @ingroup SecantIoModule SecantAsyncFile
 * @brief Control class for the write threads that are pooled
 * @author James Till Matta
 * 
 * This class allows the thread pool to control the write threads
 */
class WriteThreadControl
{
public:
    
    /**
     * @brief Retrieve the currently set state for write threads
     * @return The stored state for write threads
     */
    WriteThreadState getCurrentState(){return writeState.load();}
    
    /**
     * @brief Allows write threads to pause for a state change
     * 
     * When placed in the stopped mode, write threads use this function to
     * pause their operation on a condition variable. When the state is changed
     * all subscribers to that condition variable are woken
     */
    void waitForChange();
    
    /**
     * @brief Used by write threads to declare their termination
     * 
     * Write threads call this function immediately before they leave
     * their operator() block
     */
    void acknowledgeTerminate(){numTerminated.fetch_add(1);}
    /**
     * @brief Used by write threads to declare they have started
     * 
     * Write threads call this function immediately before they enter the
     * cycle of getting data and checking for state changes
     */
    void acknowledgeRunning(){numRunning.fetch_add(1);}
    
    /**
     * @brief Sets the write thread mode to running
     *
     * @remark This function should not be 'hammered' if it is avoidable as, to
     * prevent races with acquisition threads going into a condition var, it
     * locks a mutex.
     */
    void setToRunning(){changeState(WriteThreadState::Running, numRunning);}
    /**
     * @brief Sets the write thread mode to stopped
     *
     * @remark This function should not be 'hammered' if it is avoidable as, to
     * prevent races with acquisition threads going into a condition var, it
     * locks a mutex.
     */
    void setToStopped(){changeState(WriteThreadState::Stopped, numStopped);}
    /**
     * @brief Sets the write thread mode to terminate
     *
     * @remark This function should not be 'hammered' if it is avoidable as, to
     * prevent races with acquisition threads going into a condition var, it
     * locks a mutex.
     */
    void setToTerminate(){changeState(WriteThreadState::Terminate, numTerminated);}
    
    /**
     * @brief Retrieves the number of threads that have acknowledged the start running command
     * @return The value of the atomic integer that stores the number of run acknowledges
     */
    int getRunningCount(){return numRunning.load();}
    /**
     * @brief Retrieves the number of threads that have acknowledges the stop running command
     * @return The value of the atomic integer that stores the number of stop acknowledges
     */
    int getStoppedCount(){return numStopped.load();}
    /**
     * @brief Retrieves the number of threads that have exited their main execution loop
     * @return The value of the atomic integer that stores the number of terminate acknowledges
     */
    int getTerminateCount(){return numTerminated.load();}
    
    /**
     * @brief Gets a reference to the global class instance
     * @return A reference to the global instance of the class
     */
    static WriteThreadControl& getInstance()
    {
        //the constructor is only called the first time so things are only initialized once
        static WriteThreadControl ctrl;
        return ctrl;
    }
private:
    //called by wait for change
    /**
     * @brief This is called by write threads as they wait on the condition var
     * 
     * Write threads call this function immediately after they lock the
     * waitMutex mutex and immediately before they enter the loop to keep them
     * waiting on the condition variable while in the stopped state
     */
    void acknowledgeStop(){numStopped.fetch_add(1);}

    /**
     * @brief Locks the waitMutex, changes the state, resets the ack variable and wakes the threads on the wait condition
     * @param[in] newState The new target state for the threads
     * @param[in] ackVariable The particular atomic integer that holds the acknowledge count for that thread state
     */
    void changeState(WriteThreadState& newState, std::atomic_int& ackVariable)
    {
        //artificial scope to force destruction of the unique_lock prior to notify all
        {
            boost::unique_lock<boost::mutex> lock(this->waitMutex);
            if(writeState.load() != newState)
            {
                ackVariable.store(0);
            }
            writeState.store(newState);
        }
        writeThreadWaitCondition.notify_all();
    }
    
    /**
     * @brief Private default constructor to make the WriteThreadPool when needed
     */
    WriteThreadControl(){}
    
    //member variables
    std::atomic<WriteThreadState> writeState = WriteThreadState::Stopped;///<Current state of the write threads
    std::atomic_int numRunning = 0; ///<Count of the number of threads that ack running mode
    std::atomic_int numStopped = 0; ///<Count of the number of threads that ack stopped mode
    std::atomic_int numTerminated = 0; ///<Count of the number of threads that ack terminate
    boost::mutex waitMutex; ///<Prevents state change races and allows write threads to wait for a state change
    boost::condition_variable writeThreadWaitCondition; ///<Allows write threads to wait for a state change
    
public://Deleted methods go in this section because I want the enhanced error
    //messages of public deleted methods, but don't want to 'gum up' my actual
    //function declarations
    /**
     * @brief Deleted copy constructor because this is a singleton
     */
    WriteThreadControl(WriteThreadControl const&) = delete;
    /**
     * @brief Deleted assignment operator because this is a singleton
     */
    void operator=(WriteThreadControl const&) = delete;
};


}
}
}
#endif //SECANT_SRC_SECANT_IO_ASYNCOUTFILE_WRITETHREADCONTROL_H
