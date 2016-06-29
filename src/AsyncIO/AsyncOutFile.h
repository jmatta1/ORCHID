/***************************************************************************//**
********************************************************************************
**
** @file AsynchronousFile.h
** @author James Till Matta
** @date 06 June, 2016
** @brief
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** @details Holds the defintion of the reentrant class that is used to write a
** file in a sequential but asynchronous manner. Also holds the defintion of the
** callable class used to provide the write thread.
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHID_SRC_ASYNCIO_ASYNCOUTFILE_H
#define ORCHID_SRC_ASYNCIO_ASYNCOUTFILE_H
// includes for C system headers
// includes for C++ system headers
#include<fstream>
#include<string>
#include<atomic>
#include<utility>
// includes from other libraries
#include<boost/thread.hpp>
#include<boost/lockfree/spsc_queue.hpp>
#include<boost/lockfree/queue.hpp>
// includes from ORCHID
#include<InterThreadComm/QueueTMP.h>

namespace AsyncIO
{

enum{QueueCapacity = 100};

//utility class used inside async file out
struct BufferPair
{
public:
    BufferPair():buffer(nullptr), size(0){}
    void clear(){buffer = nullptr; size = 0;}
    char* buffer;
    int size;
};

//predeclare AsyncOutFile so that AsyncOutFileWriteThread knows about it
template <typename RetQueueType>
class AsyncOutFile;

//just a function used by boost thread to run the write loop along with a ptr
//to the data structure with the actual information in it
template <typename RetQueueType>
class AsyncOutFileWriteThread
{
public:
    AsyncOutFileWriteThread(AsyncOutFile<RetQueueType>* aof);
    void operator()();
private:
    AsyncOutFile<RetQueueType>* aOutFile;
};


template <typename RetQueueType>
class AsyncOutFile
{
public:
    AsyncOutFile(const std::string& filePath, RetQueueType* cbQueue);
    ~AsyncOutFile();
    
    //waits until writes are done and then stops writes, and makes a new file
    void newFile(const std::string& filePath);
    //enqueue a buffer for writing
    void writeBuf(char* outBuffer, int bufferSize);
    
    //shutdown the write thread and close the file
    void closeAndTerminate();
    
    //used to check for write thread terminate exit
    bool hasTerminated(){return this->writeTerminated.load();}
    //used to wait for write thread terminate
    void joinWrite(){this->writeThread->join();}
    
    friend class AsyncOutFileWriteThread<RetQueueType>;
private:
    
    //the actual fstream to write the file
    std::ofstream outFile;
    
    //synchronization objects to wait for data or wait to add data
    //used to hold back the consumer thread writing data if it needs to wait for more data
    boost::mutex writeMutex;
    boost::condition_variable writeWaitCondition;
    std::atomic_bool writerWaiting;
    //used to hold back the producer thread enqueing data if we are trying to write too fast
    boost::mutex producerMutex;
    boost::condition_variable producerWaitCondition;
    std::atomic_bool producerWaiting;
    
    //this queue is used to hold data to be written to the file, we fix its capacity to 100 so that there cannot be more than 100 queued file writes
    //TODO: Investigate writing own lock free queue for faster data transfer
    RetQueueType* callBackQueue;
    boost::lockfree::spsc_queue<BufferPair*, boost::lockfree::capacity<QueueCapacity> > writeQueue;
    boost::lockfree::spsc_queue<BufferPair*, boost::lockfree::capacity<QueueCapacity> > returnQueue;
    
    //this is used to tell the write thread to exit when the write queue is empty
    std::atomic_bool terminateWhenEmpty;
    //this is the flag the thread uses to
    std::atomic_bool writeTerminated;
    
    //thread callable and object
    AsyncOutFileWriteThread<RetQueueType>* writeCallable;
    boost::thread* writeThread;
};

/*
 * Implementation of the AsyncOutFileWriteThread
 */
template <typename RetQueueType>
AsyncOutFileWriteThread<RetQueueType>::AsyncOutFileWriteThread(AsyncOutFile<RetQueueType>* aof):
    aOutFile(aof) {}

template <typename RetQueueType>
void AsyncOutFileWriteThread<RetQueueType>::operator ()()
{
    //first lock the write mutex, whenever this thread is not waiting it has this
    //mutex locked to prevent changes to the underlying fstream
    boost::unique_lock<boost::mutex> writeLock(this->aOutFile->writeMutex);
    //the overarching loop. It checks for the termination signal and
    //if it sees one then it exits that loop going to the secondary empty queue
    //loop before terminating
    BufferPair* temp = nullptr;
    while(!(this->aOutFile->terminateWhenEmpty.load()))
    {
        //if we do not have a terminate signal, then check for a lack of data
        //(and terminate signals)
        while(!(this->aOutFile->writeQueue.read_available()) && !(this->aOutFile->terminateWhenEmpty.load()))
        {
            //first make certain the writer thread is not waiting for some reason
            if(this->aOutFile->producerWaiting.load())
            {
                this->aOutFile->producerWaitCondition.notify_one();//just use notify 1 since there is only ever one thread waiting
            }
            //if we are here then we have not been told to terminate, nor have we been told that there is data for reading
            //so instead we go to sleep, this also unlocks the mutex so the fstream can be pointed at a new file while we sleep
            this->aOutFile->writerWaiting.store(true);
            this->aOutFile->writeWaitCondition.wait(writeLock);
        }
        //we are done waiting, tell people
        this->aOutFile->writerWaiting.store(false);
        //if we are here then there is either data available for reading or we were told to terminate
        //if there is data available, grab it and write it
        if(this->aOutFile->writeQueue.pop(temp))
        {
            //write the buffer
            this->aOutFile->outFile.write(temp->buffer, temp->size);
            //run the call back to return the buffer to whoever owns it
            //this->aOutFile->callBackQueue->push(temp->buffer);
            InterThread::PushSelect<RetQueueType, char*>::push(*(this->aOutFile->callBackQueue), temp->buffer);
            //clear the pair
            temp->clear();
            //return the BSCTriple to the return queue if we took this out of the
            // write queue then the must be space for it in the return queue
            this->aOutFile->returnQueue.push(temp);
            //then check to see if we need to wake the producer thread, since it
            //must be waiting for BSCTriples, it can wake up now
            if(this->aOutFile->producerWaiting.load())
            {
                this->aOutFile->producerWaitCondition.notify_one();
            }
        }
        //if we are here there either was data and we popped a piece and wrote it
        //or we recieved a terminate signal (or both) so let the loop either exit
        //or continue
    }
    
    //if we are here we have a terminate signal, so empty the write buffer first
    while(this->aOutFile->writeQueue.pop(temp))
    {
        //if we are in here then there is data in the buffer to empty before
        //termination
        //write the buffer
        this->aOutFile->outFile.write(temp->buffer, temp->size);
        //run the call back to return the buffer to whoever owns it
        this->aOutFile->callBackQueue->push(temp->buffer);
        //clear the pair
        temp->clear();
        //return the BSCTriple to the return queue if we took this out of the
        // write queue then the must be space for it in the return queue
        //this allows it to be deleted by the destructor
        this->aOutFile->returnQueue.push(temp);
        //don't bother waking the producer thread, if we are being terminated
        //then we are being destroyed and the producer thread is not waiting on
        //a wait condition, merely for this thread to terminate
    }

    //finally, notify someone that we have exitted
    this->aOutFile->writeTerminated.store(true);
}

/*
 * Implementation of the AsyncOutFile
 */

template <typename RetQueueType>
AsyncOutFile<RetQueueType>::AsyncOutFile(const std::string &filePath, RetQueueType* cbQueue):
    outFile(filePath.c_str(), std::ios_base::binary | std::ios_base::trunc),
    writerWaiting(false), producerWaiting(false), callBackQueue(cbQueue),
    terminateWhenEmpty(false), writeTerminated(false), writeCallable(nullptr),
    writeThread(nullptr)
{
    //load the return queue with spare BSCTriples
    for(int i=0; i<QueueCapacity; ++i)
    {
        returnQueue.push(new BufferPair);
    }
    //now generate the callable and thread for writing
    writeCallable = new AsyncOutFileWriteThread<RetQueueType>(this);
    writeThread = new boost::thread(*writeCallable);
}

template <typename RetQueueType>
AsyncOutFile<RetQueueType>::~AsyncOutFile()
{
    this->closeAndTerminate();
    //delete all the BSCTriples we allocated initially
    BufferPair* temp;
    while(returnQueue.pop(temp))
    {
        delete[] temp;
    }
    delete writeThread;
    delete writeCallable;
}

template <typename RetQueueType>
void AsyncOutFile<RetQueueType>::closeAndTerminate()
{
    //terminate the write thread if it is not already terminated
    this->terminateWhenEmpty.store(true);
    //if we need to wake the thread for it to terminate, do so
    if(this->writerWaiting.load())
    {
        this->writeWaitCondition.notify_one();
    }
    //wait for the termination
    if(!(this->writeTerminated.load())) writeThread->join();
}

template <typename RetQueueType>
void AsyncOutFile<RetQueueType>::newFile(const std::string& filePath)
{
    //first we try to lock the writeMutex.
    //If we manage that then the write thread is waiting for data.
    //If we have to wait that is ok because we should be the only write thread
    //therefor we block any addition to the write queue until the mutex is locked by us anyways
    boost::unique_lock<boost::mutex> writeLock(this->writeMutex);
    //once we have the lock, we have control of the fstream
    outFile.close();
    outFile.open(filePath.c_str(), std::ios_base::binary | std::ios_base::trunc);
    //our changes are done
    //the lock should release on destruction when the function exits
}

template <typename RetQueueType>
void AsyncOutFile<RetQueueType>::writeBuf(char* outBuffer, int size)
{
    //first get the blank BSCTriple from the return queue
    BufferPair* temp;
    while(!(this->returnQueue.pop(temp)))
    {//there are no pairs available
        //first make certain that the write thread is awake and working so that we will eventually be woken
        if(this->writerWaiting.load())
        {
            this->writeWaitCondition.notify_one();//just use notify 1 since there is only ever one thread waiting
        }
        
        //now that we are sure that things are running along, set to wait
        boost::unique_lock<boost::mutex> producerLock(this->producerMutex);
        this->producerWaiting.store(true);
        this->producerWaitCondition.wait(producerLock);
    }
    this->producerWaiting.store(false);
    //now that we have a triple, prep it and push it onto the write queue
    temp->buffer = outBuffer;
    temp->size = size;
    //if we acquired a triple from the return queue, then we have a slot available in the write queue
    this->writeQueue.push(temp);
    //since there is now data, make certain that the write thread is processing it
    if(this->writerWaiting.load())
    {
        this->writeWaitCondition.notify_one();//just use notify 1 since there is only ever one thread waiting
    }
}

}
#endif //ORCHID_SRC_ASYNCIO_ASYNCOUTFILE_H
