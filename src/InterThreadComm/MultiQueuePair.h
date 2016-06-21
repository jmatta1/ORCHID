/***************************************************************************//**
********************************************************************************
**
** @file DataQueueWaiter.h
** @author James Till Matta
** @date 21 June, 2016
** @brief
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** @details Holds a system that allows a single consumer to consume data of
** multiple types of producers simultaneously
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHID_SRC_INTERTHREADCOMM_MULTIQUEUEPAIR_H
#define ORCHID_SRC_INTERTHREADCOMM_MULTIQUEUEPAIR_H

// includes for C system headers
// includes for C++ system headers
#include<atomic>
// includes from other libraries
#include<boost/thread.hpp>
#include<boost/lockfree/queue.hpp>
#include<boost/lockfree/spsc_queue.hpp>
// includes from ORCHID
#include"Events/EventInterface.h"
#include"InterThreadComm/InterThreadQueueSizes.h"

namespace InterThread
{

typedef boost::lockfree::spsc_queue<Events::EventInterface*, boost::lockfree::capacity<getEnumVal(QueueSizes::SlowControlToFile)>> SlowCtrlsQueue;
typedef boost::lockfree::queue<Events::EventInterface*, boost::lockfree::capacity<getEnumVal(QueueSizes::ProcessingToFile)>> ProcessedQueue;

class MultiQueuePair
{
public:
    MultiQueuePair();
    ~MultiQueuePair();
    
    bool slowCtrlsPop(Events::EventInterface*& data);
    bool processedPop(Events::EventInterface*& data);
    
    bool trySlowCtrlsPop(Events::EventInterface*& data);
    bool tryProcessedPop(Events::EventInterface*& data);
    
    bool slowCtrlsPush(Events::EventInterface* data);
    bool processedPush(Events::EventInterface* data);
    
    bool trySlowCtrlsPush(Events::EventInterface* data);
    bool tryProcessedPush(Events::EventInterface* data);
    
    
    //functions for the file output thread to use to try to get data
    bool tryFilePopSlowCtrls(Events::EventInterface*& data);
    bool tryFilePopProcessed(Events::EventInterface*& data);
    
    bool tryFilePushSlowCtrls(Events::EventInterface* data);
    bool tryFilePushProcessed(Events::EventInterface* data);
    
    //function for the file thread to use to wait for data
    //this has no checks for the existance of data, such must be outside this class
    void fileWaitForData();
    
private:
    //pair of queues for slow controls events
    SlowCtrlsQueue slowCtrlsConsumerQueue;
    SlowCtrlsQueue slowCtrlsProducerQueue;
    
    //pair of queues for processed events
    ProcessedQueue processedConsumerQueue;
    ProcessedQueue processedProducerQueue;
    
    //thread structures necessary for slow controls waiting
    boost::mutex slowCtrlsMutex;
    boost::condition_variable slowCtrlsWaitCondition;
    std::atomic_bool slowCtrlsWaiting;
    
    //thread structurs necessary for processing threads waiting
    boost::mutex processedMutex;
    boost::condition_variable processedWaitCondition;
    std::atomic_int processedWaiting;
    
    //threading structures necessary for the waiting of the consumer queue
    boost::mutex fileMutex;
    boost::condition_variable fileWaitCondition;
    std::atomic_bool fileWaiting;
    
    //hold whether or not our awakening has been forced
    std::atomic_bool notForcedAwakening;
};

}

#endif //ORCHID_SRC_INTERTHREADCOMM_MULTIQUEUEPAIR_H
