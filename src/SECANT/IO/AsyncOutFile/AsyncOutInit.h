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
#ifndef SECANT_SRC_SECANT_IO_ASYNCOUTFILE_ASYNCOUTINIT_H
#define SECANT_SRC_SECANT_IO_ASYNCOUTFILE_ASYNCOUTINIT_H
// includes for C system headers
// includes for C++ system headers
// includes from other libraries
// includes from ORCHID
#include"WriteThreadPool.h"
#include"WriteMultiQueue.h"
#include"ConcurrentOfstreamCollection.h"

namespace SECANT
{
namespace IO
{
namespace AsyncFile
{

/**
 * @class AsyncOutInit
 * @ingroup SecantIoModule SecantAsyncFile
 * @brief Constructs the AsyncOutFile singletons in the correct order with the correct parameters
 * @author James Till Matta
 * 
 * Simple class with a single job, upon construction it forces the construction
 * of the asyncoutfile singletons in the correct order with correct parameters.
 * The fate of this object after its constructor is run does not matter.
 */
class AsyncOutInit
{
public:
    AsyncOutInit(int numFiles=4, WriteThreadMode mode=WriteThreadMode::Greedy,
                 int numWriteThreads=2, int bufferSize=2*1024*1024, int numBuffers=200):
        queues(WriteMultiQueue::getInstance(numFiles, bufferSize, numBuffers)),
        collection(ConcurrentOfstreamCollection::getInstance(numFiles)),
        pool(WriteThreadPool::getInstance(numWriteThreads, mode)){}
private:
    WriteMultiQueue& queues;
    ConcurrentOfstreamCollection& collection;
    WriteThreadPool& pool;
};

}
}
}
#endif //SECANT_SRC_SECANT_IO_ASYNCOUTFILE_ASYNCOUTINIT_H
