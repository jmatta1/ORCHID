/***************************************************************************//**
********************************************************************************
**
** @file FileOutputThread.h
** @author James Till Matta
** @date 24 June, 2016
** @brief
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** @details Holds a few type defs of heavily templated types for my sanity
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHID_SRC_UTILITY_COMMONTYPEDEFS_H
#define ORCHID_SRC_UTILITY_COMMONTYPEDEFS_H

#include"Events/EventInterface.h"
#include"InterThreadComm/InterThreadQueueSizes.h"
#include"InterThreadComm/MultiQueuePair.h"
#include"InterThreadComm/QueuePair.h"
#include"InterThreadComm/RawBufferTriple.h"

namespace Utility
{
typedef InterThread::RawBufferTriple<int> ToProcessingBuffer;

//queue types
typedef boost::lockfree::queue<Events::EventInterface*, boost::lockfree::capacity<InterThread::getEnumVal(InterThread::QueueSizes::ProcessingToFile)> > ProcessedEventQueue;
typedef boost::lockfree::spsc_queue<Events::EventInterface*, boost::lockfree::capacity<InterThread::getEnumVal(InterThread::QueueSizes::SlowControlToFile)> > SlowControlEventQueue;
typedef boost::lockfree::queue<ToProcessingBuffer*, boost::lockfree::capacity<InterThread::getEnumVal(InterThread::QueueSizes::DigitizerToProcessing)> > ToProcessingQueue;

//Queues for sending things to file type
typedef InterThread::MultiQueuePair<Events::EventInterface*, ProcessedEventQueue, SlowControlEventQueue> ToFileMultiQueue;
enum{ProcessingQueueIndex = 0, SlowControlsQueueIndex = 1};

//queues for sending things from digitizer to processing
typedef InterThread::QueuePair<ToProcessingBuffer*, ToProcessingQueue> ToProcessingQueuePair;
}
#endif //ORCHID_SRC_UTILITY_COMMONTYPEDEFS_H
