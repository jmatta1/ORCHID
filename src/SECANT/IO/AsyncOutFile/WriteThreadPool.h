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
#ifndef SECANT_SRC_SECANT_IO_ASYNCOUTFILE_WRITETHREADPOOL_H
#define SECANT_SRC_SECANT_IO_ASYNCOUTFILE_WRITETHREADPOOL_H

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

/*!
 * @class WriteThreadPool
 * @ingroup SecantIoModule SecantAsyncFile
 * @brief This class serves as manager for the Asynchronous Write thread pool
 * @author James Till Matta
 * 
 * This class holds the necessary logic and data to manage the thread pool and
 * the multiqueue that feeds it.
 */
class WriteThreadPool
{
public:
    /**
     * @brief Gets a reference to the global class instance
     * @return A reference to the global instance of the class
     */
    static WriteThreadPool& getInstance(int numWriteThreads=2,
                                        WriteThreadPoolMode mode=WriteThreadPoolMode::Greedy)
    {
        //the constructor is only called the first time so things are only initialized once
        static WriteThreadPool pool(numWriteThreads, mode);
        return pool;
    }
    
private:
    /**
     * @brief Private default constructor to make the WriteThreadPool when needed
     */
    WriteThreadPool(int numWriteThreads, WriteThreadPoolMode mode);
    
    WriteMultiQueue& writeQueue; ///<Holds the write queues the threads pull from
    int threadCount; ///<stores the number of threads to use for concurrent asynchronous writing
    WriteThreadPoolMode writeMode; ///<Used to build threads with the appropriate operation mode
    WriteThread** threadCallables; ///<Stores the functors that are run by the threads
    boost::thread_group writeThreads; ///<Boost thread group object that stores the multiple thread objects doing writes
    WriteThreadControl threadController;
    

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
#endif //SECANT_SRC_SECANT_IO_ASYNCOUTFILE_WRITETHREADPOOL_H
