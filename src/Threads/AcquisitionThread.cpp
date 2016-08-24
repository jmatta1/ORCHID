/***************************************************************************//**
********************************************************************************
**
** @file AcquisitionThread.cpp
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
** @details Implementation file for the AcquisitionThread callable
**
********************************************************************************
*******************************************************************************/

#include"AcquisitionThread.h"
// includes for C system headers
// includes for C++ system headers
// includes from other libraries
// includes from ORCHID

namespace Threads
{

void AcquisitionThread::operator ()()
{
    //run the event loop
    while(this->notTerminated)
    {
        switch(this->controller->getCurrentState())
        {
        case InterThread::AcquisitionThreadState::Terminate:
            BOOST_LOG_SEV(lg, Information) << "ACQ Thread: Terminating";
            this->notTerminated = false;
            break;
        case InterThread::AcquisitionThreadState::Stopped:
            BOOST_LOG_SEV(lg, Information) << "ACQ Thread: Stopped and waiting for instructions";
            this->controller->waitForChange();
            break;
        case InterThread::AcquisitionThreadState::Acquiring:
            BOOST_LOG_SEV(lg, Information) << "ACQ Thread: Starting Acquisition";
            this->doAcquisitionLoop();
            break;
        }
    }
    //if we are here then we are exiting
    this->controller->acknowledgeTerminate();
}


void AcquisitionThread::doAcquisitionLoop()
{
    this->startAcquisition();
    while(this->controller->getCurrentState() == InterThread::AcquisitionThreadState::Acquiring)
    {
        //we have the buffer prepped and ready for the first read, so go about things
        currentBuffer->info.startChannel = this->firstChannel;
        currentBuffer->info.boardNumber = this->modNumber;
        currentBuffer->sizeOfData = this->digitizer->getData(currentBuffer->dataBuffer);
        if(currentBuffer->sizeOfData != 0)
        {
            //if we have a filled buffer, there must be space for it to be pushed
            if(this->dataOutputQueue->producerPush(currentBuffer))
            {
                BOOST_LOG_SEV(lg, Information) << "ACQ Thread: Pushed a buffer";
            }
            else
            {
                BOOST_LOG_SEV(lg, Information) << "ACQ Thread: Failed to push a buffer";
            }
            currentBuffer = nullptr;
            //now pop a buffer
            if(!(this->dataOutputQueue->producerPop(currentBuffer)))
            {
                //if this failed we are being asked to exit the loop while we were waiting for a buffer
                currentBuffer = nullptr;
                break;
            }
            if(currentBuffer != nullptr)
            {
                BOOST_LOG_SEV(lg, Information) << "ACQ Thread: Popped a buffer";
            }
        }
        
    }
    this->endAcquisition();
}

void AcquisitionThread::startAcquisition()
{
    //prime the current buffer
    if(!(this->dataOutputQueue->producerPop(currentBuffer)))
    {
        
        //we were unable to obtain a buffer at acquisition start... this should
        //be impossible since on acquisition start all the buffers should be stocked up
        //in the producer queue ready to be used
        BOOST_LOG_SEV(lg, Information) << "ACQ Thread: Unable to obtain an empty buffer on acq start, something is wrong";
        throw std::runtime_error("ACQ Thread Error: Unable to obtain an empty buffer on acq start");
    }
    else
    {
        BOOST_LOG_SEV(lg, Information) << "ACQ Thread: Popped a buffer";
    }
    //start acquisition
    this->digitizer->startAcquisition();
}

void AcquisitionThread::endAcquisition()
{
    this->digitizer->stopAcquisition();
    //now get the final data
    this->doFinalRead();
}

void AcquisitionThread::doFinalRead()
{
    bool haveBuffer = (currentBuffer == nullptr);
    while(!haveBuffer)
    {
        currentBuffer == nullptr;
        haveBuffer = this->dataOutputQueue->producerPop(currentBuffer);
        if(!haveBuffer && currentBuffer != nullptr)
        {
            BOOST_LOG_SEV(lg, Information) << "ACQ Thread: Popped a buffer but think I didnt";
        }
    }
    currentBuffer->info.startChannel = this->firstChannel;
    currentBuffer->info.boardNumber = this->modNumber;
    currentBuffer->sizeOfData = this->digitizer->performFinalReadout(currentBuffer->dataBuffer);
    if(currentBuffer->sizeOfData != 0)
    {
        //if we have a buffer, there must be space for it to be pushed
        if(this->dataOutputQueue->producerPush(currentBuffer))
        {
            BOOST_LOG_SEV(lg, Information) << "ACQ Thread: Producer Pushed a buffer";
        }
        else
        {
            BOOST_LOG_SEV(lg, Information) << "ACQ Thread: Producer push failed";
        }
        currentBuffer = nullptr;
    }
    else
    {
         //since the buffer is empty, dump it back on to the end of the consumer queue
        if(this->dataOutputQueue->consumerPush(currentBuffer))
        {
            BOOST_LOG_SEV(lg, Information) << "ACQ Thread: Consumer Pushed a buffer";
        }
        else
        {
            BOOST_LOG_SEV(lg, Information) << "ACQ Thread: Consumer push failed";
        }
        currentBuffer = nullptr;
    }
}

}
