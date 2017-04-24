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
#include"WriteThread.h"
// includes for C system headers
// includes for C++ system headers
// includes from other libraries
// includes from ORCHID
#include"WriteThreadControl.h"
#include"WriteMultiQueue.h"
#include"ConcurrentOfstreamCollection.h"

namespace SECANT
{
namespace IO
{
namespace AsyncFile
{

WriteThread::WriteThread(WriteThreadMode mode):
    writeMode(mode), writeQueue(WriteMultiQueue::getInstance()),
    controller(WriteThreadControl::getInstance()),
    files(ConcurrentOfstreamCollection::getInstance()){}

void WriteThread::operator ()()
{
    bool notTerminated = true;
    while(notTerminated)
    {
        switch(controller.getCurrentState())
        {
        case WriteThreadState::Terminate:
            this->notTerminated = false;
            break;
        case WriteThreadState::Running:
            controller.acknowledgeRunning();
            if(mode==WriteThreadMode::Greedy)
            { greedyWriteLoop();}
            else
            { austereWriteLoop();}
            // if we are here, clear the writes that are still incoming and then
            // check state flags
            clearWriteBufferLoop();
            break;
        case WriteThreadState::Stopped:
            controller.waitForChange(); // stop gets acked in wait for change
            break;
        }
    }
    //make certain the queues are cleared before terminating
    
    controller.acknowledgeTerminate();
}

void WriteThread::greedyWriteLoop()
{
    int numFiles = files.getNumberFiles();
    //Keep looping while we are set to running
    while(controller.getCurrentState() == WriteThreadState::Running)
    {
        //keep going while there is data available
        while(writeQueue.dataAvailable())
        {
            doGreedyWriteCycle();
        }
        //if we are here then there is no more data available and it is time to wait for more data
        writeQueue.waitForData();
    }
}

void WriteThread::austereWriteLoop()
{
    int numFiles = files.getNumberFiles();
    //Keep looping while we are set to running
    while(controller.getCurrentState() == WriteThreadState::Running)
    {
        //keep going while there is data available
        while(writeQueue.dataAvailable())
        {
            doGreedyWriteCycle();
        }
        //if we are here then there is no more data available and it is time to wait for more data
        writeQueue.waitForData();
    }
}

void WriteThread::doGreedyWriteCycle()
{
    //iterate through all the files
    for(int i=0; i<numFiles; ++i)
    {
        //first check if there are writes for this file
        if(writeQueue.fileHasWrites(i))
        {
            //if we see that there are writes to the file, get a file ref
            //and then try to lock the file
            ConcurrentOfstreamWrapper& currFile = files.getFileReference(i);
            if(currFile.tryForLock())//if false another thread beat us to the lock, go on to the next queue / file
            {
                //we have the lock, check if there is still data
                if(!writeQueue.fileHasWrites(i))
                {//if we are here then another thread beat us to the punch and did the writing before we locked
                    currFile.unlock();
                    continue;
                }
                //if we are here, then there is data to be written for the file
                while(writeQueue.fileHasWrites(i))
                {
                    doPopWritePushAction(i, currFile);
                }
                //we have exhausted the write queue, unlock the file and go to the next file
                currFile.unlock();
            }
        }
    }
}

void WriteThread::doAustereWriteCycle()
{
    //iterate through all the files
    for(int i=0; i<numFiles; ++i)
    {
        //first check if there are writes for this file
        if(writeQueue.fileHasWrites(i))
        {
            //if we see that there are writes to the file, get a file ref
            //and then try to lock the file
            ConcurrentOfstreamWrapper& currFile = files.getFileReference(i);
            if(currFile.tryForLock())//if false another thread beat us to the lock, go on to the next queue / file
            {
                //we have the lock, check if there is still data
                if(!writeQueue.fileHasWrites(i))
                {//if we are here then another thread beat us to the punch and did the writing before we locked
                    currFile.unlock();
                    continue;
                }
                //if we are here, then there is data to be written for the file
                doPopWritePushAction(i, currFile);
                //we have done one write, unlock the file and go to the next file
                currFile.unlock();
            }
        }
    }
}

void WriteThread::doPopWritePushAction(int fileNum, ConcurrentOfstreamWrapper& currFile)
{
    int writeSize = 0;
    char* tempBuffer = writeQueue.popFileWrite(fileNum, writeSize);
    if(tempBuffer == nullptr && writeSize == 0)
    {//there was no buffer to pop when we got here, which should not have been able to happen
        return;
    }
    //write the buffer
    currFile.unsafeWrite(tempBuffer, writeSize);
    //return the buffer
    writeQueue.pushEmptyBuffer(tempBuffer);
}

}
}
}
