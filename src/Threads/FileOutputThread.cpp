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
#include<algorithm>
// includes from other libraries
#define BOOST_FILESYSTEM_NO_DEPRECATED
#include<boost/filesystem.hpp>
#include<boost/crc.hpp>
#include<boost/date_time/gregorian/gregorian.hpp>
// includes from ORCHID
#include"Utility/OrchidLogger.h"
#include"Utility/OrchidConfig.h"
#include"Events/EventInterface.h"

namespace Threads
{

static const boost::posix_time::ptime epoch(boost::gregorian::date(1970,1,1), boost::posix_time::time_duration(0,0,0,0));

FileOutputThread::FileOutputThread(SlowControlEventQueue* incomingSlowControlEvents,
                                   SlowControlEventQueue* returningSlowControlEvents,
         //                        ProcessedEventQueue* incomingDigitizerEvents,
         //                        ProcessedEventQueue* returningDigitizerEvents,
                                   InterThread::FileData* fileDat,
                                   InterThread::FileOutputThreadController* fileThreadCtrl,
                                   InputParser::GeneralBlock* generalBlock):
    outFile(nullptr), evBufSize(2048), eventBuffer(nullptr), 
    outDirectory(generalBlock->baseOutputDirectory), currentFileName(""),
    currentRunTitle(generalBlock->runTitle), runNumber(0), sequenceNumber(0),
    buffInd(0), bufferNumber(0), eventCount(0),
    incomingSlowControlsEventQueue(incomingSlowControlEvents),
    returningSlowControlsEventQueue(returningSlowControlEvents),
    fileData(fileDat), fileThreadController(fileThreadCtrl), notTerminated(true)
{
    //make directories and prep file name
    this->prepNewRunFolder();
    //load the queue with buffers of the appropriate size
    for(int i=0; i<BufferCount ; ++i)
    {
        char* allocBuffer = new (std::nothrow) char[BufferSizeInBytes];
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
    this->getNextBuffer();
    //make the asynchronous output file
    this->outFile = new AsyncIO::AsyncOutFile<BufferQueue>(this->currentFileName, &(this->bufferQueue));
    //write the file header
    this->writeFileHeader();
    //allocate the event buffer to be 2kB
    this->eventBuffer = new char[evBufSize];
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
    delete[] this->eventBuffer;
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
        this->writeFileHeader();
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
        this->writeFileHeader();
    }
}

void FileOutputThread::writeFileHeader()
{
    //directly manipulate the buffer to contain the header and send it to disk
    reinterpret_cast<unsigned long long*>(&(this->currentBuffer[this->buffInd]))[0] = 0x0102040810204080ULL;
    this->buffInd += 8;
    reinterpret_cast<unsigned long*>(&(this->currentBuffer[this->buffInd]))[0] = 0x00000001;
    this->buffInd += 4;
    reinterpret_cast<unsigned short*>(&(this->currentBuffer[this->buffInd]))[0] = ORCHID_MAJOR_VERSION;
    this->buffInd += 2;
    reinterpret_cast<unsigned short*>(&(this->currentBuffer[this->buffInd]))[0] = ORCHID_MINOR_VERSION;
    this->buffInd += 2;
    reinterpret_cast<unsigned short*>(&(this->currentBuffer[this->buffInd]))[0] = ORCHID_PATCH_VERSION;
    this->buffInd += 2;
    this->currFileTime = boost::posix_time::microsec_clock::universal_time();
    boost::posix_time::time_duration epochTime = (this->currFileTime - epoch);
    reinterpret_cast<long long*>(&(this->currentBuffer[this->buffInd]))[0] = epochTime.total_nanoseconds();
    this->buffInd += 8;
    std::string fileTimeString = boost::posix_time::to_iso_extended_string(currFileTime);
    std::copy(fileTimeString.c_str(), fileTimeString.c_str() + 30, &this->currentBuffer[this->buffInd]);
    this->buffInd += 30;
    if(this->currentRunTitle.size() < 100)
    {
        std::copy(this->currentRunTitle.c_str(), this->currentRunTitle.c_str() + this->currentRunTitle.size(), &this->currentBuffer[this->buffInd]);
        this->buffInd += this->currentRunTitle.size();
        std::fill_n(&this->currentBuffer[this->buffInd], 100-this->currentRunTitle.size(), '\0');
        this->buffInd += (100-this->currentRunTitle.size());
    }
    else
    {
        std::copy(this->currentRunTitle.c_str(), this->currentRunTitle.c_str() + 99, &this->currentBuffer[this->buffInd]);
        this->buffInd += 99;
        this->currentBuffer[this->buffInd] = '\0';
        this->buffInd += 1;
    }
    reinterpret_cast<unsigned long*>(&(this->currentBuffer[this->buffInd]))[0] = this->runNumber;
    this->buffInd += 4;
    reinterpret_cast<unsigned long*>(&(this->currentBuffer[this->buffInd]))[0] = this->sequenceNumber;
    this->buffInd += 4;
    //reserve 3924 bytes in the buffer header for whatever
    std::fill_n(reinterpret_cast<unsigned int*>(&this->currentBuffer[this->buffInd]), 981, 0);
    this->buffInd += 3924;
    reinterpret_cast<unsigned long long*>(&(this->currentBuffer[this->buffInd]))[0] = 0xF0F0F0F0F0F0F0F0ULL;
    this->buffInd += 8;
    //send this 4kB header to the file
    this->writeBufferToDisk(this->buffInd);
}

void FileOutputThread::doWriteLoop()
{
    bool gotData = false;
    while(this->fileThreadController->getCurrentState() == InterThread::FileOutputThreadState::Writing)
    {
        //TODO: Modify this when we have a event processing thread
        //check for data
        /*Events::EventInterface* fastEvent;
        if(this->incomingDigitizerEventQueue->pop(fastEvent))
        {
            gotData = true;
            
        }*/
        Events::EventInterface* slowEvent;
        if(this->incomingSlowControlsEventQueue->pop(slowEvent))
        {
            gotData = true;
            int eventSize = slowEvent->getSizeOfBinaryRepresentation();
            //loop until we double the event buffer past the size of the event
            while(eventSize > this->evBufSize)
            {
                //we need to double the size of the event buffer
                this->doubleEventBuffer();
            }
            //now that we know that we have enough size
            slowEvent->getBinaryRepresentation(this->eventBuffer);
            //now that we are done with the event pass it back to the return queue
            this->returningSlowControlsEventQueue->push(slowEvent);
            //now that we have the data, write it to our large buffer
            this->transferData(eventSize);
        }
        if(gotData != true)
        {
            //we recieved no data from either queue so wait for data
            //TODO, add logic for data waits here
        }
        gotData = false;
    }
}

void FileOutputThread::doubleEventBuffer()
{
    delete[] this->eventBuffer;
    this->evBufSize *= 2;
    this->eventBuffer = new char[this->evBufSize];
}

void FileOutputThread::transferData(int eventSize)
{
    //TODO implement compression when we write data to the buffer
    if(eventSize < (BufferSizeInBytes - this->buffInd))
    {//we have enough space, write the event to the buffer
        std::copy(this->eventBuffer, (this->eventBuffer + eventSize), &(this->currentBuffer[this->buffInd]));
        ++(this->eventCount);
        this->buffInd += eventSize;
    }
    else
    {//otherwise, write the buffer to disk and write this event to the next buffer
        this->finalizeDataBuffer();//this finalizes the event and writes the buffer to disk
        //here we make certain that we are not at capacity
        if (this->bufferNumber < MaxBuffersPerFile)
        {
            //increment sequence number and set up the new run
            ++(this->sequenceNumber);
            this->buildFileName();
            this->outFile->newFile(this->currentFileName);
            //reset the buffer number to 0 now that we have a new file
            this->bufferNumber = 0;
            this->writeFileHeader();
        }
    }
}

void FileOutputThread::finalizeDataBuffer()
{
    //first we load the last of the buffer with zeros
    std::fill_n(&(this->currentBuffer[this->buffInd]), BufferSizeInBytes - this->buffInd, 0);
    //here we go back to the beginning of the buffer and load the number of events into the header
    reinterpret_cast<unsigned int*>(&(this->currentBuffer[12]))[0] = this->eventCount;
    //now skip past the first 32 bytes of the buffer and start writing the 4 byte 32 bit crcs
    unsigned int writeInd   = 32;
    unsigned int writeSize  = 4;
    unsigned int readInd    = BufferOverHead;
    unsigned int readSize   = 1024;
    unsigned int blockCount = (BufferSizeInBytes - BufferOverHead)/readSize;
    //make the crc object
    boost::crc_optimal<32,0x32583499, 0x00000000, 0x00000000, false, false> crcComputer;
    //now cycle through the buffer calculating crc 32 values
    for(int i=0; i<blockCount; ++i)
    {
        crcComputer.process_block(&(this->currentBuffer[readInd]),&(this->currentBuffer[readInd+readSize]));
        reinterpret_cast<unsigned int*>(&(this->currentBuffer[writeInd]))[0] = crcComputer.checksum();
        writeInd += writeSize;
        readInd += readSize;
    }
    //now we have written all the CRC checksumsm now we can write the buffer to disk
    this->writeBufferToDisk(BufferSizeInBytes);
    this->eventCount = 0;
}

void FileOutputThread::writeBufferToDisk(int bufferSize)
{
    this->outFile->writeBuf(this->currentBuffer, bufferSize);
    //now that we have sent that buffer out
    //get another use in the actual file handling
    this->getNextBuffer();
}

void FileOutputThread::getNextBuffer()
{
    this->bufferQueue.pop(this->currentBuffer);
    ++bufferNumber;
    this->buffInd = 0;
    this->writeBufferHeader();
}

void FileOutputThread::writeBufferHeader()
{
    //first we write the buffer separator
    reinterpret_cast<unsigned long long*>(&(this->currentBuffer[this->buffInd]))[0] = 0xF0F0F0F0F0F0F0F0ULL;
    this->buffInd += 8;
    //then we write the buffer ID
    reinterpret_cast<unsigned int*>(&(this->currentBuffer[this->buffInd]))[0] = 0x00000002;
    this->buffInd += 4;
    //then we write a stand in for the number of events
    reinterpret_cast<unsigned int*>(&(this->currentBuffer[this->buffInd]))[0] = 0;
    this->buffInd += 4;
    //then we write which buffer number this is in the file
    reinterpret_cast<unsigned int*>(&(this->currentBuffer[this->buffInd]))[0] = this->bufferNumber;
    this->buffInd += 4;
    //then we write which buffer number this is in the run
    reinterpret_cast<unsigned int*>(&(this->currentBuffer[this->buffInd]))[0] = ((this->sequenceNumber * MaxBuffersPerFile) + this->bufferNumber);
    this->buffInd += 4;
    //then we write zero to the remainder of the 8kb, since we are aligned to an 8byte boundary go in 8 byte chunks
    std::fill_n(reinterpret_cast<unsigned long long*>(&(this->currentBuffer[this->buffInd])), ((BufferOverHead - 24)/8), 0ULL);
    this->buffInd = BufferOverHead;
}

}
