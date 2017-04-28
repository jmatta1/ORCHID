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
** @details Holds the defintion of the singleton class that coordinates the pool
** of threads that perform file writes. Also holds the write mode enumeration
**
********************************************************************************
*******************************************************************************/
#ifndef SECANT_SRC_IO_ASYNCOUTFILE_WRITETHREADPOOL_H
#define SECANT_SRC_IO_ASYNCOUTFILE_WRITETHREADPOOL_H

// includes for C system headers
// includes for C++ system headers
#include<atomic>
// includes from other libraries
#include<boost/thread.hpp>
// includes from ORCHID
#include"WriteThread.h"
#include"WriteMultiQueue.h"
#include"WriteThreadControl.h"

namespace SECANT
{
namespace IO
{
namespace AsyncFile
{

/**
 * @class WriteThreadPool
 * @ingroup SecantIoModule SecantAsyncFile
 * @brief This class serves as the manager for the Asynchronous Write thread pool
 * @author James Till Matta
 * 
 * This class holds the necessary logic and data to manage the thread pool and
 * the multiqueue that feeds it.
 */
class WriteThreadPool
{
public:
    /**
     * @brief Sets threads to terminate and calls join_all on the thread group
     */
    ~WriteThreadPool()
    {
        //Tell the write threads that it is time to go
        threadController.setToTerminate();
        //make sure the write threads are not waiting on data from the queues to get said message
        writeQueues.forceWakeFromWait();
        //wait for everything to close down
        writeThreads.join_all();
    }
    
    /**
     * @brief Gets the number of write threads in the pool
     * @return The number of threads in the thread_group
     */
    int size(){return writeThreads.size();}
    
    /**
     * @brief Gets the mode that threads were initialized with
     * @return The write mode that write threads were initialized with
     */
    WriteThreadMode getThreadMode(){return writeMode;}
    
    /**
     * @brief Gets a reference to the global class instance
     * @param[in] numWriteThreads How many write threads to create when first called
     * @param[in] mode What mode to create the write threads in when first called
     * @return A reference to the global instance of the class
     * 
     * The very first time this function is called in an execution it generates
     * a new global/singleton instance from the parameters passed and returns a
     * reference to it. All subsequent times, the parameters are ignored and it
     * simply returns a reference to the already generated instance
     */
    static WriteThreadPool& getInstance(int numWriteThreads=2,
                                        WriteThreadMode mode=WriteThreadMode::Greedy)
    {
        //the constructor is only called the first time so things are only initialized once
        static WriteThreadPool pool(numWriteThreads, mode);
        return pool;
    }
    
private:
    /**
     * @brief Private default constructor to make the WriteThreadPool when needed
     */
    WriteThreadPool(int numWriteThreads, WriteThreadMode mode);
    
    WriteThreadControl& threadController; ///<Class to hold control for writeThreads
    WriteMultiQueue& writeQueues; ///<Class to hold write and buffer queues
    boost::thread_group writeThreads; ///<Boost thread group object that stores the multiple thread objects doing writes
    WriteThreadMode writeMode; ///<Used to build threads with the appropriate operation mode
    

public: //Deleted methods go in this section because I want the enhanced error
    //messages of public deleted methods, but don't want to 'gum up' my actual
    //function declarations
    /**
     * @brief Deleted copy constructor because this is a singleton
     */
    WriteThreadPool(WriteThreadPool const&) = delete;
    /**
     * @brief Deleted assignment operator because this is a singleton
     */
    void operator=(WriteThreadPool const&) = delete;
};


}
}
}
#endif //SECANT_SRC_IO_ASYNCOUTFILE_WRITETHREADPOOL_H
