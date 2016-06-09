/***************************************************************************//**
********************************************************************************
**
** @file FileOutputThread.h
** @author James Till Matta
** @date 08 June, 2016
** @brief
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** @details Holds the definition of the callable class that will be used to
** build the thread that handles taking events, buffering those events, and
** and writing those events
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHID_SRC_THREADS_FILEOUTPUTTHREAD_H
#define ORCHID_SRC_THREADS_FILEOUTPUTTHREAD_H

// includes for C system headers
// includes for C++ system headers
// includes from other libraries
#include<boost/lockfree/queue.hpp>
#include<boost/lockfree/spsc_queue.hpp>
#include<boost/thread.hpp>
// includes from ORCHID
#include"InterThreadComm/Control/FileOutputThreadController.h"
#include"AsyncIO/AsyncOutFile.h"
namespace Threads
{
enum {BufferQueueSize=64};

class FileOutputThread
{
public:
    FileOutputThread();
    ~FileOutputThread();
    
    void operator()();

private:
    //this is the underlying asynchronous file that we use to dump full buffers
    AsyncIO::AsyncOutFile outFile;
    //this is the underlying queue full of buffers to store data to be sent to
    //the file when the time comes
    boost::lockfree::spsc_queue<char*, boost::lockfree::capacity<BufferQueueSize> > bufferQueue;
    
};
}
#endif //ORCHID_SRC_THREADS_FILEOUTPUTTHREAD_H
