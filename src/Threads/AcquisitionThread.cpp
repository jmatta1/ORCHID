/***************************************************************************//**
********************************************************************************
**
** @file DigitizerThread.cpp
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
** @details Implementation file for the DigitizerThread callable
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
            this->dataOutputQueue->producerPush(currentBuffer);
            //now pop a buffer
            if(!(this->dataOutputQueue->producerPop(currentBuffer)))
            {
                BOOST_LOG_SEV(lg, Information) << "ACQ Thread: Failure in waiting for a buffer";
                this->digitizer->stopAcquisition();
                throw std::runtime_error("ACQ Thread Error: Failure in waiting for a buffer");
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
    currentBuffer->info.startChannel = this->firstChannel;
    currentBuffer->info.boardNumber = this->modNumber;
    currentBuffer->sizeOfData = this->digitizer->performFinalReadout(currentBuffer->dataBuffer);
    if(currentBuffer->sizeOfData != 0)
    {
        //if we have a buffer, there must be space for it to be pushed
        this->dataOutputQueue->producerPush(currentBuffer);
    }
    else
    {
         //since the buffer is empty, dump it back on to the end of the consumer queue
        this->dataOutputQueue->consumerPush(currentBuffer);
    }
}

}
