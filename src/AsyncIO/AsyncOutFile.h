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
// includes from ORCHID

template <typename FunctorType>
namespace AsyncIO
{

//just a function used by boost thread to run the write loop along with a ptr
//to the data structure with the actual information in it
class AsyncOutFileWriteThread
{
public:
    AsyncOutFileWriteThread(AsyncOutFile* aof);
    void operator()();
private:
    AsyncOutFile* aOutFile;
};


template <typename FunctorType>
struct BSCTriple
{
public:
    BSCTriple();
};


template <typename FunctorType>
class AsyncOutFile
{
public:
    AsyncOutFile(const std::string& filePath);
    ~AsyncOutFile();
    
    //waits until writes are done and then stops writes, and makes a new file
    void newFile(const std::string& filePath);
    //enqueue a buffer for writing
    template <typename FunctorType>
    void writeBuf(char* outBuffer, int bufferSize, FunctorType* callBack);
    
    //used to check for write thread terminate exit
    bool hasTerminated(){return this->writeTerminated.load();}
    //used to wait for write thread terminate
    void joinWrite(){this->writeThread->join();}
    
    friend class AsyncOutFileWriteThread;
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
    boost::lockfree::spsc_queue<std::pair<char*, int>, boost::lockfree::capacity<100> > writeQueue;
    boost::lockfree::spsc_queue<std::pair<char*, int>, boost::lockfree::capacity<100> > returnQueue;
    
    //this is used to tell the write thread to exit when the write queue is empty
    std::atomic_bool terminateWhenEmpty;
    //this is the flag the thread uses to
    std::atomic_bool writeTerminated;
    
    //thread callable and object
    AsyncOutFileWriteThread* writeCallable;
    boost::thread* writeThread;
};

template <typename FunctorType>
AsyncOutFileWriteThread<FunctorType>::AsyncOutFileWriteThread(AsyncOutFile *aof):
    aOutFile(aof) {}

template <typename FunctorType>
void AsyncOutFileWriteThread<FunctorType>::operator ()()
{
    
}

template <typename FunctorType>
AsyncOutFile<FunctorType>::AsyncOutFile(const std::string &filePath):
    outFile(filePath.c_str(), std::ios_base::binary | std::ios_base::trunc),
    writerWaiting(false), producerWaiting(false), terminateWhenEmpty(false), writeTerminated(false),
    writeCallable(nullptr), writeThread(nullptr)
{
    writeCallable = new AsyncOutFileWriteThread(this);
    writeThread = new boost::thread(*writeCallable);
}

template <typename FunctorType>
void AsyncOutFile<FunctorType>::newFile(const std::string& filePath)
{
    //first we try to lock the writeMutex.
    //If we manage that then the write thread is waiting for data.
    //If we have to wait that is ok because we should be the only write thread
    //therefor we block any addition to the write queue until the mutex is locked by us anyways
    boost::unique_lock<boost::mutex> consLock(consMutex);
    //once we have the lock, we have control of the fstream
    outFile.close();
    outFile.open(filePath.c_str(), std::ios_base::binary | std::ios_base::trunc);
    //our changes are done
    //the lock should release on deconstruction
}


template <typename FunctorType>
void AsyncOutFile<FunctorType>::writeBuf(char* outBuffer, int size, FunctorType* callBack)
{
    std::pair<char*, int> temp = std::make_pair<char*, int>(outBuffer, size);
    //test to see if there is space to write the buffer pair
    while(!(this->writeQueue.write_available()))
    {
        //if there is no space, wait for there to be space
        boost::unique_lock<boost::mutex> producerLock(this->producerMutex);
          
    }

}

}
#endif //ORCHID_SRC_ASYNCIO_ASYNCOUTFILE_H
