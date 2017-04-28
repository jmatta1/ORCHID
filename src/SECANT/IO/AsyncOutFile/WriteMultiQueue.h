/***************************************************************************//**
********************************************************************************
**
** @author James Till Matta
** @date 09 April, 2017
** @brief Holds the defintion of the singleton class that handles queueing of
** writes from the known open files
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
********************************************************************************
*******************************************************************************/
#ifndef SECANT_SRC_IO_ASYNCOUTFILE_WRITEMULTIQUEUE_H
#define SECANT_SRC_IO_ASYNCOUTFILE_WRITEMULTIQUEUE_H

// includes for C system headers
// includes for C++ system headers
#include<atomic>
// includes from other libraries
#include<boost/lockfree/queue.hpp>
#include<boost/lockfree/stack.hpp>
#include<boost/thread.hpp>
// includes from ORCHID
#include"WriteThread.h"
#include"WriteMultiQueue.h"

class std::string;

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
static const int MaximumWriteQueueSize = 2048;

/**
 * @struct BufferData
 * @ingroup SecantIoModule SecantAsyncFile
 * @brief The BufferData struct is POD that simple contains a pointer to the
 * write buffer and the size of the data to be written
 * @author James Till Matta
 */
struct BufferData
{
public:
    char* writeBuffer = nullptr;
    int writeSize = 0;
};

/**
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
 * sending when (why would you do that to yourself?). Also, if multiple threads
 * are enquing writes, you still need to find some mechanism to ensure that a
 * single thread has the responsibility of requesting file name changes
 */
class WriteMultiQueue
{
    using ContainerStack = boost::lockfree::stack<BufferData*, boost::lockfree::capacity<MaximumWriteQueueSize> >;
    using BufferStack = boost::lockfree::stack<char*, boost::lockfree::capacity<MaximumWriteQueueSize> >;
    using WriteQueue = boost::lockfree::queue<BufferData*, boost::lockfree::capacity<MaximumWriteQueueSize> >;
public:
    ~WriteMultiQueue(){delete[] bufferCount; delete[] writeQueues;}
    /**
     * @brief Puts an empty buffer back on the buffer return queue for use in another write
     * @param[in] emptyBuffer Pointer to the empty write buffer to be returned for reuse
     * 
     * This function does not block ever, there should always be enough space on
     * the return buffer stack for buffers that were allocated by the constructor
     */
    void pushEmptyBuffer(char* emptyBuffer);
    
    /**
     * @brief Retrieves an empty buffer from the queue, if there are none, wait until there are
     * @return Pointer to the empty buffer retrieved from the queue
     */
    char* popEmptyBuffer();
    
    /**
     * @brief Pushes a full buffer ready for writing onto the write queue for a file
     * @param[in] The number of the file to write
     * @param[in] The byte array containing the data to be written
     * @param[in] The number of bytes to be written in the writeBuffer
     */
    void pushFileWrite(int fileNumber, char* writeBuffer, int writeSize);
    
    /**
     * @brief Pulls a write from the queue for file number fileNumber. 
     * @param[in] fileNumber the index of the file to be written to
     * @param[out] writeSize the size in bytes of the write
     * @return pointer to the byte array containing the data to write or nullptr
     * 
     * This tries to pop a file buffer so that it can be written by one of the
     * writer threads. However, in certain cases it is possible that the write
     * could have been grabbed already and disposed of by a different writer
     * thread. In that case, if there are no further writes available for the
     * requested file, this function will return a nullptr after it sets
     * writeSize to zero
     */
    char* popFileWrite(int fileNumber, int& writeSize);
    
    /**
     * @brief Allows a writer thread to sit and wait for writes to be enqueued
     * for *any* of the files
     * 
     * A thread will return from this function when a write has been enqueued
     */
    void waitForData();
    
    /**
     * @brief Forces threads waiting for data to wake even if there is no data
     * 
     * This is used in shutdown to forcibly wake threads that are waiting so that
     * they can see the terminate flag
     */
    void forceWakeFromWait(){notForceWake.store(false); consumerWait.notify_all(); producerWait.notify_all();}
    
    /**
     * @brief Checks if there are writes pending in the queue
     * @return True if there are pending writes, false otherwise
     */
    bool dataAvailable(){return 0 < queuedWrites.load();}
    
    /**
     * @brief Checks if the write queue for that file has queued writes
     * @param[in] fileNumber the index of the file to be written to
     * @return True if there are queued writes for that file, false otherwise
     */
    bool fileHasWrites(int fileNumber){return 0 < bufferCount[fileNumber].load();}
    
    /**
     * @brief Returns the buffer size the WriteMultiQueue was constructed with
     * @return The allocated size of buffers in the queues
     */
    int getBufferSize(){return bufferSizeInBytes;}
    
    /**
     * @brief Returns the total number of buffers created at construction
     * @return The number of buffers that were allocated at WriteMultiQueue construction
     */
    int getNumberOfBuffers(){return totalBufferCount;}
    
    /**
     * @brief Returns the number of files / write queues
     * @return The number of files / write queues
     */
    int getNumberOfFiles(){return queueCount;}
    
    /**
     * @brief Gets a reference to the global class instance
     * @param[in] numFiles The number of output files to be managed, defaults to 4
     * @param[in] bufferSize The size of a buffer in bytes, defaults to 2MB
     * @param[in] numBuffers The number of buffers to allocate for use, defaults to 200
     * @return A reference to the global instance of the class
     */
    static WriteMultiQueue& getInstance(int numFiles=4, int bufferSize=2*1024*1024, int numBuffers=200)
    {
        static WriteMultiQueue multiQueue(numFiles, bufferSize, numBuffers);
        return multiQueue;
    }

private:
    /**
     * @brief Private default constructor to make the WriteThreadPool when needed
     */
    WriteMultiQueue(int numFiles, int bufferSize, int numBuffers);
    
    std::atomic_bool notForceWake = true;
    
    std::atomic_int* bufferCount; ///<Array of atomic integers that count the number of writes in each queue
    std::atomic_int queuedWrites = 0; ///<Total number of writes enqueued
    WriteQueue* writeQueues; ///<Array of write queues for each file
    BufferStack emptyBuffers; ///<Stack of buffers available for filling, this is a stack so that if write speeds are not insane, only a small subset of buffers ever get used
    ContainerStack emptyContainers; ///<Stack of empty BufferData objects for use in pushing the two push functions
    int bufferSizeInBytes; ///<Holds the buffer size in bytes that this queue was constructed with
    int totalBufferCount; ///<Holds the number of buffers that this queue was constructed with
    int queueCount; ///<Holds the number of write queues / buffers
    
    boost::mutex consumerMutex; ///<Locked by the write threads when they want to wait for data to appear for any file
    boost::condition_variable consumerWait; ///<Used by the write threads to wait for data to appear for any file
    
    boost::mutex producerMutex; ///<Locked by the threads that enqueue writes when they want to wait for an empty buffer to appear on the stack
    boost::condition_variable producerWait; ///<Used by the threads that enqueue writes when they want to wait for an empty buffer

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
#endif //SECANT_SRC_IO_ASYNCOUTFILE_WRITEMULTIQUEUE_H
