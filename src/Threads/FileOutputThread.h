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
#include<boost/thread.hpp>
#include "boost/date_time/posix_time/posix_time.hpp"
// includes from ORCHID
#include"InterThreadComm/Control/FileOutputThreadController.h"
#include"AsyncIO/AsyncOutFile.h"
#include"InterThreadComm/InterThreadQueueSizes.h"
#include"Events/SlowControlsEvent.h"
#include"InterThreadComm/Data/FileData.h"
#include"Events/TriggerEvent.h"
#include"InputLib/Blocks/GeneralBlock.h"
#include"Utility/CommonTypeDefs.h"

namespace Threads
{
//256MB of memory for buffers, approx 2 second of HDD write
enum {BufferCount=128, BufferSizeInBytes=2097152, BufferOverHead=8192, MaxBuffersPerFile=975};
typedef boost::lockfree::spsc_queue<char*, boost::lockfree::capacity<BufferCount> > BufferQueue;

class FileOutputThread
{
public:
    FileOutputThread(Utility::ToFileMultiQueue* queueInput, InterThread::FileData* fileDat,
                     InterThread::FileOutputThreadController* fileThreadCtrl,
                     InputParser::GeneralBlock* generalBlock);
    ~FileOutputThread();
    
    void operator()();

private:
    //make certain that a directory exists, or create it if it doesnt and the like
    void prepNewRunFolder();
    //make the file name based on run info
    void buildFileName();
    //this function handles new parameters
    void grabNewRunParameters();
    //this function handles the actual writing loop
    void doWriteLoop();
    //this function simply doubles the size of the event buffer
    void doubleEventBuffer();
    //this function performs the necessary actions to write the event to the buffer
    //if the buffer is full it finalizes it and sends it to disk
    void transferData(int eventSize);
    //takes a full buffer, and writes the checksums for its contents into the header then sends it to disk
    void finalizeDataBuffer();
    //write buffers of indeterminate size to disk
    void writeBufferToDisk(int bufferSize);
    //this function generates the file header and writes it to the disk
    void writeFileHeader();
    //this function pops a buffer from the buffer queue
    //the length of the buffer queue is deliberately longer than the internal queue used
    //by the async file to move buffers to its write thread. In this way there is always a buffer in the queue
    void getNextBuffer();
    //this function writes the header of the buffer to the current buffer
    void writeBufferHeader();
    void emptyWriteQueueBeforeChange();
    
    
    /*
     * File Data
     */
    //this is the underlying asynchronous file that we use to dump full buffers
    AsyncIO::AsyncOutFile<BufferQueue>* outFile;
    //this is the underlying queue full of buffers to store data to be sent to
    //the file when the time comes
    BufferQueue bufferQueue;
    //stores the size of the event buffer
    int evBufSize;
    //buffer to hold the event to be placed into the data stream
    char* eventBuffer;
    //current buffer to fill
    char* currentBuffer;
    //index into the current buffer
    int buffInd;
    //stores the current buffer number of the file
    int bufferNumber;
    //stores how many events are in the current buffer
    int eventCount;
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
    //the start time of the current file
    boost::posix_time::ptime currFileTime;
    
    /*
     * Interthread Queues
     */
    Utility::ToFileMultiQueue* inputQueues;
    
    /*
     * Interthread Statistics and Control
     */
    //this is the variable that stores file statistics
    InterThread::FileData* fileData;
    //this is the variable that points to the control structure for this thread
    InterThread::FileOutputThreadController* fileThreadController;
    
    /*
     * Loop Control
     */
    bool notTerminated;
};
}
#endif //ORCHID_SRC_THREADS_FILEOUTPUTTHREAD_H
