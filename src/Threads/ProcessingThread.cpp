/***************************************************************************//**
********************************************************************************
**
** @file ProcessingThread.cpp
** @author James Till Matta
** @date 27 June, 2016
** @brief
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** @details Definition file for the ProcessingThread callable
**
********************************************************************************
*******************************************************************************/
#include"ProcessingThread.h"
// includes for C system headers
// includes for C++ system headers
#include<iomanip>
// includes from other libraries
// includes from ORCHID
#include"Events/WavedumpEvent.h"

namespace Threads
{

void ProcessingThread::operator()()
{
    //run the event loop
    while(this->notTerminated)
    {
        switch(this->controller->getCurrentState())
        {
        case InterThread::ProcessingThreadState::Terminate:
            BOOST_LOG_SEV(lg, Information) << "PR Thread " << threadNumber << ": Terminating";
            this->notTerminated = false;
            break;
        case InterThread::ProcessingThreadState::Stopped:
            BOOST_LOG_SEV(lg, Information) << "PR Thread " << threadNumber << ": Stopped and waiting for instructions";
            this->controller->waitForChange();
            break;
        case InterThread::ProcessingThreadState::Running:
            BOOST_LOG_SEV(lg, Information) << "PR Thread " << threadNumber << ": Starting Processing";
            this->doProcessingLoop();
            BOOST_LOG_SEV(lg, Information) << "PR Thread " << threadNumber << ": Clearing buffer queue";
            this->emptyProcessingBuffer();
            break;
        }
    }
    //if we are here then we are exiting
    this->controller->acknowledgeTerminate();
}

void ProcessingThread::doProcessingLoop()
{
    Utility::ToProcessingBuffer* dataBuffer = nullptr;
    while(this->controller->getCurrentState() == InterThread::ProcessingThreadState::Running)
    {
        //first pull a buffer from the file thread queue, this may cause a wait
        if(dataInputQueue->consumerPop(dataBuffer))
        {//if we got the buffer, proceed, if not hit the top of the loop again to be
            //certain we are not being asked to terminate
            processDataBuffer(dataBuffer); //when this finishes the data buffer
            //is completely handled and can be put back on the return queue
            this->dataInputQueue->consumerPush(dataBuffer);
            dataBuffer = nullptr;
        }
    }
    if(dataBuffer != nullptr)
    {
        this->dataInputQueue->consumerPush(dataBuffer);
    }
}

void ProcessingThread::emptyProcessingBuffer()
{//essentially loop until we cannot pull more buffers
    Utility::ToProcessingBuffer* dataBuffer = nullptr;
    while(dataInputQueue->tryConsumerPop(dataBuffer))
    {
        //asked to terminate
        processDataBuffer(dataBuffer); //when this finishes the data buffer
        //is completely handled and can be put back on the return queue
        this->dataInputQueue->consumerPush(dataBuffer);
        dataBuffer = nullptr;
    }
    if(dataBuffer != nullptr)
    {
        this->dataInputQueue->consumerPush(dataBuffer);
    }
}

void ProcessingThread::processDataBuffer(Utility::ToProcessingBuffer* buffer)
{
    //grab a couple useful things in the buffer
    unsigned int* rawBuffer = buffer->dataBuffer;
    int dataSize = buffer->sizeOfData;
    int board = buffer->info.boardNumber;
    int offset = 0;
    acqData->addData(board, 4*dataSize);
    //now loop through the board aggregates
    int loopCount = 0;
    while(offset < dataSize)
    {
        //first makes sure the board aggregate leads with the right info
        if(((rawBuffer[offset] & 0xF0000000UL)!=0xA0000000UL) && ((rawBuffer[offset] & 0x0FFFFFFFUL) <= (dataSize - offset)))
        {
            BOOST_LOG_SEV(lg, Warning) << "PR Thread " << threadNumber << ": Found corrupted board aggregate";
            BOOST_LOG_SEV(lg, Warning) << "PR Thread " << threadNumber << ": BA Header is: 0x" << std::hex << rawBuffer[offset] << " 0x" << rawBuffer[offset+1] << " 0x" << rawBuffer[offset+2] << " 0x" << rawBuffer[offset+3];
            BOOST_LOG_SEV(lg, Warning) << "PR Thread " << threadNumber << ": Brd #: " << std::dec << board << " Size: " << dataSize << " Offset: " << offset;
            this->acqData->addProcErr(10);
            //now skip the rest of the buffer
            break;
        }
        //if we have the right header, process the event
        offset += processEvent(buffer, offset);
        ++loopCount;
    }
}

int ProcessingThread::processEvent(Utility::ToProcessingBuffer* buffer, int startOffset)
{
    //get the useful stuff from the buffer
    unsigned int* rawBuffer = buffer->dataBuffer;
    int startChan = buffer->info.startChannel;
    int offset = startOffset;
    //read things from the event header
    int stopOffset = startOffset + (rawBuffer[offset] & 0x0FFFFFFFUL);
    ++offset;
    unsigned int pattern = ((rawBuffer[offset] & 0x00FFFF00UL) >> 8);
    int chanMask = (rawBuffer[offset] & 0x000000FFUL);
    ++offset;
    chanMask |= ((rawBuffer[offset] & 0xFF000000UL) >> 16);
    int eventCount = (rawBuffer[offset] & 0x00FFFFFFUL);
    ++offset;
    unsigned int lowTime = rawBuffer[offset];
    ++offset;
    //now loop through the board aggregates
    int loopCount = 0;
    while(offset < stopOffset)
    {
        if(((chanMask >> loopCount) & 0x01) == 1)
        {
            Events::EventInterface* prEvent=nullptr;
            while(!(this->toFileOutputQueue->producerPop<Utility::ProcessingQueueIndex>(prEvent)))
            {
                BOOST_LOG_SEV(lg, Information) << "PR Thread " << threadNumber << ": waiting for empty events after a force wake";
                boost::this_thread::sleep_for(boost::chrono::nanoseconds(1000));
            }
            //put the data into the event
            Events::WavedumpEvent* event = static_cast<Events::WavedumpEvent*>(prEvent);
            event->setBoard(buffer->info.boardNumber);
            event->setChannel((loopCount+startChan));
            event->setTimeStamp(lowTime);
            event->setExtraTimeStamp(pattern);
            event->setEventNumber(eventCount);
            offset += event->setDataArray(&(rawBuffer[offset]));
            //push the event back into the queue
            this->toFileOutputQueue->producerPush<Utility::ProcessingQueueIndex>(prEvent);
        }
        ++loopCount;
    }
    return (offset - startOffset);
}

}
