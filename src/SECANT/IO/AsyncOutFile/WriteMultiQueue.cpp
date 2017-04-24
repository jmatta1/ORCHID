/***************************************************************************//**
********************************************************************************
**
** @author James Till Matta
** @date 19 April, 2017
** @brief Holds the implementation of the singleton class that handles queueing
** of writes from the asynchronous files
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
********************************************************************************
*******************************************************************************/

#include"WriteMultiQueue.h"
// includes for C system headers
// includes for C++ system headers
#include<cassert>
#include<chrono>
// includes from other libraries
// includes from ORCHID

namespace SECANT
{
namespace IO
{
namespace AsyncFile
{

static const int MicrosecondWaitTime = 100;

WriteMultiQueue::WriteMultiQueue(int numFiles, int bufferSize, int numBuffers):
    bufferSizeInBytes(bufferSize), totalBufferCount(numBuffers), queueCount(numFiles)
{
    //simple assertions for sanity
    assert(numBuffers<MaximumWriteQueueSize);
    assert(numFiles<numBuffers);
    //allocate the write counts and write queues
    bufferCount = new std::atomic_int[numFiles];
    writeQueues = WriteQueue[numFiles];
    //allocate the bufferData objects and buffers
    for(int i=0; i<numBuffers; ++i)
    {
        //first allocate the buffer data object and the buffer
        BufferData* tempContainer = new BufferData;
        char* tempBuffer = new char[bufferSize];
        //now push them on to the emptyBuffers and empty containers stack
        emptyContainers.push(tempContainer);
        emptyBuffers.push(tempBuffer);
    }
}

void WriteMultiQueue::pushEmptyBuffer(char* emptyBuffer)
{
    //push the empty buffer onto the write stack
    bool test = emptyBuffers.push(emptyBuffer);
    //if we could not push an empty buffer onto the stack, something is deeply wrong
    assert(test);
    //now wake one thread that might be waiting for data (since we only enqueue one empty buffer)
    producerWait.notify_one();
}

char* WriteMultiQueue::popEmptyBuffer()
{
    //first try and get an empty buffer this gets put into a try and wait loop
    //that way if there is no buffer to be had, we wait until notified that there
    //is a buffer to be had
    char* tempBuffer = nullptr;
    while(!emptyBuffers.pop(tempBuffer))
    {
        //lock the mutex so we can wait on the condition variable
        boost::unique_lock waitLock(producerMutex);
        //do a timed wait on the condition variable. It is timed because there
        //is the possiblity of a new empty buffer being pushed onto the queue
        //between the call to pop and this call to wait. This way, if that did
        //happen we would wake to check for a buffer anyways
        producerWait.wait_for(waitLock, std::chrono::microseconds(MicrosecondWaitTime));
    }
    //we have the buffer, return it
    return tempBuffer;
}

void WriteMultiQueue::pushFileWrite(int fileNumber, char* writeBuffer, int writeSize)
{
    //grab a buffer container from the stack, there should definitely be one there
    BufferData* tempContainer = nullptr;
    bool test = emptyContainers.pop(tempContainer);
    assert(test);//if this fails there was no buffer and something is super wrong
    tempContainer->writeBuffer = writeBuffer;
    tempContainer->writeSize = writeSize;
    //now push the container onto the write queue for the appropriate file
    test = writeQueues[fileNumber].push(tempContainer);
    queuedWrites.fetch_add(1);
    bufferCount[fileNumber].fetch_add(1);
    //if this fails we could not push and something is super wrong, because the fixed
    //size of the write queues is the same as the fixed size of the stacks and should
    //be greater than or equal to the number of buffers created
    assert(test);
    //awaken one of the consumer/write threads to handle this buffer
    consumerWait.notify_one();
}

char* WriteMultiQueue::popFileWrite(int fileNumber, int& writeSize)
{
    //first try and get a full Buffer container from the queue
    BufferData* tempContainer = nullptr;
    bool test = writeQueues[fileNumber].pop(tempContainer);
    if(bufferCount[fileNumber].load() == 0 && !test)
    {
        writeSize = 0;
        return nullptr;
    }
    queuedWrites.fetch_sub(1);
    bufferCount[fileNumber].fetch_sub(1);
    //once we are here we have pulled the write from the queue, take what we need and return it
    char* tempBuffer = tempContainer->writeBuffer;
    writeSize = tempContainer->writeSize;
    //now that we have what we need "empty" the container and push it onto the empty stack
    tempContainer->writeBuffer = nullptr;
    tempContainer->writeSize = 0;
    test = emptyContainers.push(tempContainer);
    assert(test);
    //now return the buffer
    return tempBuffer;
}

void WriteMultiQueue::waitForData()
{
    while(queuedWrites.load() == 0)
    {
        //lock the mutex so we can wait on the condition variable
        boost::unique_lock waitLock(producerMutex);
        //do a timed wait on the condition variable. It is timed because there
        //is the possiblity of a new empty buffer being pushed onto the queue
        //between the call to pop and this call to wait. This way, if that did
        //happen we would wake to check for a buffer anyways
        producerWait.wait(waitLock);
    }
}

}
}
}
