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

#include"InterThreadComm/QueuePair.h"
#include"InterThreadComm/RawBufferTriple.h"

namespace Utility
{
using ToProcessingBuffer = InterThread::RawBufferTriple<InterThread::BoardInfo>;
using LoggerType = boost::log::sources::severity_logger_mt<LogSeverity>;

//queue types
template<int QueueSize>
using ToProcessingQueue = boost::lockfree::queue<ToProcessingBuffer*, boost::lockfree::capacity<QueueSize> >;

//queues for sending things from digitizer to processing
template<int QueueSize>
typedef InterThread::QueuePair<ToProcessingBuffer*, ToProcessingQueue<QueueSize> > ToProcessingQueuePair;
}
#endif //ORCHID_SRC_UTILITY_COMMONTYPEDEFS_H
