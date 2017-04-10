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
** @details Holds the defintion of the singleton class that handles queueing of
** writes from the known open files
**
********************************************************************************
*******************************************************************************/
#ifndef SECANT_SRC_SECANT_IO_ASYNCOUTFILE_WRITEMULTIQUEUE_H
#define SECANT_SRC_SECANT_IO_ASYNCOUTFILE_WRITEMULTIQUEUE_H

// includes for C system headers
// includes for C++ system headers
#include<atomic>
// includes from other libraries
#include<boost/lockfree/queue.hpp>
// includes from ORCHID
#include"WriteThread.h"
#include"WriteMultiQueue.h"

namespace SECANT
{
namespace IO
{
namespace AsyncFile
{
/**
 * @brief MaximumWriteQueueSize is the maximum number of writes that can be queues for a given file
 * it is also the maximum number of buffers that can be prepared and ready
 */
static const int MaximumWriteQueueSize = 16384;

/*!
 * @class WriteMultiQueue
 * @ingroup SecantIoModule SecantAsyncFile
 * @brief The WriteMultiQueue class serves as manager for the asynchronous write queueing
 * @author James Till Matta
 * 
 * This class holds the individual file write queues as well as the communal
 * queue used to return write buffers back into circulation
 * 
 * @remarks Warning: If there are multiple threads enqueing buffers for a single
 * file the ordering of those buffers on disk is a strictly first come first
 * served basis, therefore if buffer mis-ordering is a problem, just have a single
 * thread enqueing buffers, or build a complicated system to order which thread is
 * sending when (why would you do that to yourself?).
 */
class WriteMultiQueue
{
public:
    /**
     * @brief Gets a reference to the global class instance
     * @return A reference to the global instance of the class
     */
    static WriteMultiQueue& getInstance()
    {
        static WriteMultiQueue multiQueue;
        return multiQueue;
    }

private:
    /**
     * @brief Private default constructor to make the WriteThreadPool when needed
     */
    WriteMultiQueue(){}

public: //Deleted methods go in this section because I want the enhanced error
    //messages of public deleted methods, but don't want to 'gum up' my actual
    //function declarations
    /**
     * @brief Deleted copy constructor because this is a singleton
     */
    WriteMultiQueue(WriteMultiQueue const&) = delete;
    /**
     * @brief Deleted assignment operator because this is a singleton
     */
    void operator=(WriteMultiQueue const&) = delete;
};


}
}
}
#endif //SECANT_SRC_SECANT_IO_ASYNCOUTFILE_WRITEMULTIQUEUE_H
