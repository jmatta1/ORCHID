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
#ifndef SECANT_SRC_SECANT_IO_ASYNCOUTFILE_WRITEMULTIQUEUE_H
#define SECANT_SRC_SECANT_IO_ASYNCOUTFILE_WRITEMULTIQUEUE_H

// includes for C system headers
// includes for C++ system headers
#include<atomic>
// includes from other libraries
#include<boost/lockfree/queue.hpp>
#include<boost/lockfree/stack.hpp>
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
    char* writeBuffer;
    int writeSize;
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
    using BufferStack = boost::lockfree::stack<BufferData*, boost::lockfree::capacity<MaximumWriteQueueSize> >;
    using WriteQueue = boost::lockfree::queue<BufferData*, boost::lockfree::capacity<MaximumWriteQueueSize> >;
public:
    
    /**
     * @brief Puts an empty buffer back on the buffer return queue for use in another write
     * @param[in] emptyBuffer Pointer to the empty write buffer to be returned for reuse
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
     * @return pointer to the byte array containing the data to write
     */
    char* popFileWrite(int fileNumber, int& writeSize);
    
    /**
     * @brief Checks if there are writes pending in the queue
     * @return True if there are pending writes, false otherwise
     */
    bool dataAvailable(){return 0!=queuedWrites.load();}
    
    /**
     * @brief Waits for lock of the file, closes the file and opens the new one
     * @param[in] fileNumber the index of the file whose name is to be changed
     * @param[in] filePath a constant reference to the string that contains the path to be written
     * 
     * This function will block the calling thread until the thread can lock the
     * file to change the file name etc. If multiple threads are writing to the
     * same file, you may not like the result if they all try to use this
     * function simultaneously... maybe
     */
    void changeFileName(int fileNumber, const std::string& filePath);
    
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
     * @brief Gets a reference to the global class instance
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
    
    
    std::atomic_int* bufferCounts; ///<Array of atomic integers that count the number of writes in each queue
    std::atomic_int queuedWrites = 0; ///<Total number of writes enqueued
    WriteQueue* writeQueues; ///<Array of write queues for each file
    BufferStack emptyBuffers; ///<Stack of buffers available for filling, this is a stack so that if write speeds are not insane, only a small subset of buffers ever get used
    BufferStack emptyContainers; ///<Stack of empty BufferData objects for use in pushing the two push functions
    int bufferSizeInBytes; ///<Holds the buffer size in bytes that this queue was constructed with
    int totalBufferCount; ///<Holds the number of buffers that this queue was constructed with
    

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
