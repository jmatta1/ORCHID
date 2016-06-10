/***************************************************************************//**
********************************************************************************
**
** @file FileOutputThread.cpp
** @author James Till Matta
** @date 09 June, 2016
** @brief
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** @details Holds the implementation of the callable class that will be used to
** build the thread that handles taking events, buffering those events, and
** and writing those events
**
********************************************************************************
*******************************************************************************/

#include"FileOutputThread.h"
// includes for C system headers
// includes for C++ system headers
#include<sstream>
#include<iomanip>
// includes from other libraries
#define BOOST_FILESYSTEM_NO_DEPRECATED
#include<boost/filesystem.hpp>
// includes from ORCHID
#include"Utility/OrchidLogger.h"
#include"Events/EventInterface.h"

namespace Threads
{

FileOutputThread::FileOutputThread(SlowControlEventQueue* incomingSlowControlEvents,
                                   SlowControlEventQueue* returningSlowControlEvents,
         //                        ProcessedEventQueue* incomingDigitizerEvents,
         //                        ProcessedEventQueue* returningDigitizerEvents,
                                   InterThread::FileData* fileDat,
                                   InterThread::FileOutputThreadController* fileThreadCtrl,
                                   InputParser::GeneralBlock* generalBlock):
    outFile(nullptr), outDirectory(generalBlock->baseOutputDirectory), currentFileName(""),
    currentRunTitle(generalBlock->runTitle), runNumber(0), sequenceNumber(0), buffInd(0),
    incomingSlowControlsEventQueue(incomingSlowControlEvents),
    returningSlowControlsEventQueue(returningSlowControlEvents), fileData(fileDat),
    fileThreadController(fileThreadCtrl), notTerminated(true)
{
    //make directories and prep file name
    this->prepNewRunFolder();
    //load the queue with buffers of the appropriate size
    for(int i=0; i<BufferCount ; ++i)
    {
        char* allocBuffer = new (std::nothrow) char[BufferSizeMB*1024*1024];
        if(allocBuffer == nullptr)
        {
            BOOST_LOG_SEV(OrchidLog::get(), Critical) << "Error In File Write Buffer Allocation";
            throw std::runtime_error("Error In File Write Buffer Allocation");
        }
        //technically this thread should not push, only pop, but I think that it
        //is legal in this case since concurrent use of the queue has no hope of 
        //start until after this loop
        this->bufferQueue.push(allocBuffer);
    }
    //pop a buffer from the queue to serve as what we are filling now
    this->bufferQueue.pop(this->currentBuffer);
    //make the asynchronous output file
    this->outFile = new AsyncIO::AsyncOutFile<BufferQueue>(this->currentFileName, &(this->bufferQueue));
}

FileOutputThread::~FileOutputThread()
{
    delete this->outFile;
    for(int i=0; i<BufferCount ; ++i)
    {
        char* allocBuffer;
        this->bufferQueue.pop(allocBuffer);
        delete[] allocBuffer;
    }
}

void FileOutputThread::operator()()
{
    //Prep the event loop
    while(this->notTerminated)
    {
        switch(this->fileThreadController->getCurrentState())
        {
        case InterThread::FileOutputThreadState::Terminate:
            this->notTerminated = false;
            break;
        case InterThread::FileOutputThreadState::Waiting:
            this->fileThreadController->waitForNewState();
            break;
        case InterThread::FileOutputThreadState::NewRunParams:
            this->grabNewRunParameters();
            break;
        case InterThread::FileOutputThreadState::Writing:
            this->doWriteLoop();
            break;
        }
    }
    //if we are here then we are exiting
    this->fileThreadController->setThreadDone();
}

void FileOutputThread::doWriteLoop()
{
    while(this->fileThreadController->getCurrentState() == InterThread::FileOutputThreadState::Writing)
    {
        //TODO: Modify this when we have a event processing thread
        //check for data
        bool gotData = false;
        /*Events::EventInterface* fastEvent;
        if(this->incomingDigitizerEventQueue->pop(fastEvent))
        {
            gotData = true;
            
        }*/
        Events::EventInterface* slowEvent;
        if(this->incomingSlowControlsEventQueue->pop(slowEvent))
        {
            gotData = true;
        }
    }
}

void FileOutputThread::grabNewRunParameters()
{
    std::string tempRunTitle;
    int tempRunNumber;
    this->fileThreadController->getNewRunParams(tempRunTitle, tempRunNumber);
    if(tempRunTitle == this->currentRunTitle && tempRunNumber == this->runNumber)
    {
        //everything is the same, do nothing
        return;
    }
    else if(tempRunTitle == this->currentRunTitle && tempRunNumber != this->runNumber)
    {
        //the run number has changed, we do not need to prep a new run folder,
        //we merely need to build a new file name and make a new file
        this->runNumber = tempRunNumber;
        this->sequenceNumber = 0;
        this->buildFileName();
        //now make a new file
        this->outFile->newFile(this->currentFileName);
    }
    else
    {
        //either the run title or both the run title and number have changed
        //prep a new folder and build a new run title then set a new file
        this->runNumber = tempRunNumber;
        this->currentRunTitle = tempRunTitle;
        this->sequenceNumber = 0;
        this->prepNewRunFolder();
        //now make a new file
        this->outFile->newFile(this->currentFileName);
    }
    
}

void FileOutputThread::prepNewRunFolder()
{
    boost::filesystem::path writePath(this->outDirectory);
    boost::system::error_code mkDirErr;
    bool success = boost::filesystem::create_directories(writePath, mkDirErr);
    if(!success)
    {
        BOOST_LOG_SEV(OrchidLog::get(), Critical) << "Could not create data directory in file thread";
        throw std::runtime_error("Could not create data directory in file thread");
    }
    writePath.append(this->currentRunTitle);
    success = boost::filesystem::create_directories(writePath, mkDirErr);
    if(!success)
    {
        BOOST_LOG_SEV(OrchidLog::get(), Critical) << "Could not create run directory in file thread";
        throw std::runtime_error("Could not create run directory in file thread");
    }
    if(!boost::filesystem::is_directory(writePath, mkDirErr))
    {
        BOOST_LOG_SEV(OrchidLog::get(), Critical) << "No run directory detected despite creation";
        throw std::runtime_error("No run directory detected despite creation");
    }
    this->writeDirectory = writePath.native();
    this->buildFileName();
}

void FileOutputThread::buildFileName()
{
    std::ostringstream builder;
    builder << this->writeDirectory;
    if(this->writeDirectory.back()!=boost::filesystem::path::preferred_separator)
    {
        builder << boost::filesystem::path::preferred_separator;
    }
    builder << this->currentRunTitle << "_";
    builder << std::setw(4) << std::setfill('0') << this->runNumber << ".dat.";
    builder << std::setw(4) << std::setfill('0') << this->sequenceNumber ;
    this->currentFileName = builder.str();
}

}
