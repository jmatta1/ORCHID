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
#include<string>
// includes from other libraries
#include<boost/lockfree/queue.hpp>
#include<boost/lockfree/spsc_queue.hpp>
#include<boost/thread.hpp>
// includes from ORCHID
#include"InterThreadComm/Control/FileOutputThreadController.h"
#include"AsyncIO/AsyncOutFile.h"
#include"InterThreadComm/InterThreadQueueSizes.h"
#include"Events/SlowControlEvent.h"
#include"InterThreadComm/Data/FileData.h"
#include"Events/TriggerEvent.h"
#include"InputLib/Blocks/GeneralBlock.h"

namespace Threads
{
//256MB of memory for buffers, approx 2 second of HDD write
enum {BufferCount=128, BufferSizeMB=2};
//typedef boost::lockfree::spsc_queue<TriggerEvent*, boost::lockfree::capacity<InterThread::getEnumVal(InterThread::QueueSizes::SlowControlToFile)> > ProcessedEventQueue;
typedef boost::lockfree::spsc_queue<SlowControlEvent*, boost::lockfree::capacity<InterThread::getEnumVal(InterThread::QueueSizes::SlowControlToFile)> > SlowControlEventQueue;
typedef boost::lockfree::spsc_queue<char*, boost::lockfree::capacity<BufferCount> > BufferQueue;

class FileOutputThread
{
public:
    FileOutputThread(SlowControlEventQueue* incomingSlowControlEvents, SlowControlEventQueue* returningSlowControlEvents,
             //      ProcessedEventQueue* incomingDigitizerEvents, ProcessedEventQueue* returningDigitizerEvents,
                     InterThread::FileData* fileDat, InterThread::FileOutputThreadController* fileThreadCtrl,
                     InputParser::GeneralBlock* generalBlock);
    ~FileOutputThread();
    
    void operator()();

private:
    //make certain that a directory exists, or create it if it doesnt and the like
    void prepNewRunFolder();
    //make the file name based on run info
    void buildFileName();
    
    /*
     * File Data
     */
    //this is the underlying asynchronous file that we use to dump full buffers
    AsyncIO::AsyncOutFile* outFile;
    //this is the underlying queue full of buffers to store data to be sent to
    //the file when the time comes
    BufferQueue bufferQueue;
    //the directory for all data (sub directories hold different runs)
    std::string outDirectory;
    //the directory that we are currently writing to (outDirectory/runTitle/filename.dat.#)
    std::string writeDirectory;
    //the file name
    std::string currentFileName;
    //the run title
    std::string currentRunTitle;
    //the run number
    int runNumber;
    //the sequence number for the run
    int sequenceNumber;
    
    /*
     * Interthread Queues
     */
    //this is the queue that takes events from the event processing threads to this thread
    //ProcessedEventQueue* incomingDigitizerEventQueue;
    //this is the queue that takes empty events from this thread to the event processing threads
    //ProcessedEventQueue* returningDigitizerEventQueue;
    //this is the queue that takes events from the slow controls thread to this thread
    SlowControlEventQueue* incomingSlowControlsEventQueue;
    //this is the queue that takes empty events from this thread to the slow controls thread
    SlowControlEventQueue* returningSlowControlsEventQueue;
    
    /*
     * Interthread Statistics and Control
     */
    //this is the variable that stores file statistics
    InterThread::FileData* fileData;
    //this is the variable that points to the control structure for this thread
    InterThread::FileOutputThreadController* fileThreadController;
};
}
#endif //ORCHID_SRC_THREADS_FILEOUTPUTTHREAD_H
