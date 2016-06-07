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

namespace AsyncIO
{

enum{QueueCapacity = 100};

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

class AsyncOutFile
{
public:
    AsyncOutFile(const std::string& filePath, boost::lockfree::queue* cbQueue);
    ~AsyncOutFile();
    
    //waits until writes are done and then stops writes, and makes a new file
    void newFile(const std::string& filePath);
    //enqueue a buffer for writing
    void writeBuf(char* outBuffer, int bufferSize);
    
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
    boost::lockfree::queue* callBackQueue;
    boost::lockfree::spsc_queue<BufferPair*, boost::lockfree::capacity<QueueCapacity> > writeQueue;
    boost::lockfree::spsc_queue<BufferPair*, boost::lockfree::capacity<QueueCapacity> > returnQueue;
    
    //this is used to tell the write thread to exit when the write queue is empty
    std::atomic_bool terminateWhenEmpty;
    //this is the flag the thread uses to
    std::atomic_bool writeTerminated;
    
    //thread callable and object
    AsyncOutFileWriteThread* writeCallable;
    boost::thread* writeThread;
};


//utility class used inside async file out
struct BufferPair
{
public:
    BufferPair():buffer(nullptr), size(0){}
    void clear(){buffer = nullptr; size = 0;}
    char* buffer;
    int size;
};

}
#endif //ORCHID_SRC_ASYNCIO_ASYNCOUTFILE_H
