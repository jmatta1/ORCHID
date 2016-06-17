/***************************************************************************//**
********************************************************************************
**
** @file DataQueueWaiter.h
** @author James Till Matta
** @date 14 June, 2016
** @brief
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** @details Holds a system that is used to wait for data to be in recieving
** queues or to wait for space to be available in sending queues
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHID_SRC_INTERTHREADCOMM_QUEUEPAIR_H
#define ORCHID_SRC_INTERTHREADCOMM_QUEUEPAIR_H

// includes for C system headers
// includes for C++ system headers
#include<atomic>
// includes from other libraries
#include<boost/thread.hpp>
#include<boost/lockfree/queue.hpp>
#include<boost/lockfree/spsc_queue.hpp>
// includes from ORCHID
#include"InterThreadQueueSizes.h"
namespace InterThread
{

template <typename QueueType, typename QueueContents, QueueSizes qSize>
class QueuePair
{
public:
    QueuePair():producerWaiting(false), consumerWaiting(false), notForceWake(true),
        consumerQueue(nullptr), producerQueue(nullptr)
    {
        consumerQueue = new QueueType<QueueContents*, boost::lockfree::capacity<getEnumVal(qSize)>>();
        //by adding this tiny bit of extra size we never need to wait on the return queue, it always works
        producerQueue = new QueueType<QueueContents*, boost::lockfree::capacity<getEnumVal(qSize)>>();
    }
    
    bool pushToConsumerQueue(QueueContents* data);
    bool popFromConsumerQueue(QueueContents*& data);
    bool pushToProducerQueue(QueueContents* data);
    bool popFromProducerQueue(QueueContents*& data);
    
    //these are short and easy functions but I cannot write them here because I am specializing them
    inline bool tryPushToConsumerQueue(QueueContents* data);
    inline bool tryPopFromConsumerQueue(QueueContents*& data);
    inline bool tryPushToProducerQueue(QueueContents* data);
    inline bool tryPopFromProducerQueue(QueueContents*& data);
    
    void wakeOneConsumer(){if(this->consumerWaiting.load() > 0) this->consumerWaitCondition.notify_one();}
    void wakeOneProducer(){if(this->producerWaiting.load() > 0) this->producerWaitCondition.notify_one();}
    void wakeAllConsumer(){if(this->consumerWaiting.load() > 0) this->consumerWaitCondition.notify_all();}
    void wakeAllProducer(){if(this->producerWaiting.load() > 0) this->producerWaitCondition.notify_all();}
    
    void forceWakeAll(){notForceWake.store(false); consumerWaitCondition.notify_all(); producerWaitCondition.notify_all();}
    
    void clearForce(){notForceWake.store(true);}
    
private:
    //variables for producer waiting
    boost::mutex producerWaitMutex;
    boost::condition_variable producerWaitCondition;
    std::atomic_int producerWaiting;
    
    //variables for consumer waiting
    boost::mutex consumerWaitMutex;
    boost::condition_variable consumerWaitCondition;
    std::atomic_int consumerWaiting;
    
    //parameter used to force termination of loops when force override is called
    std::atomic_bool notForceWake;
    
    //queues
    //queue for full data objects to be sent from producer to consumer
    QueueType<QueueContents*, boost::lockfree::capacity<getEnumVal(qSize)>>* consumerQueue;
    //queue for empty/read data objects to be sent from consumer to producer
    QueueType<QueueContents*, boost::lockfree::capacity<getEnumVal(qSize)>>* producerQueue;
};


template <typename QueueContents, QueueSizes qSize>
bool QueuePair<boost::lockfree::spsc_queue, QueueContents, qSize>::pushToConsumerQueue(QueueContents* data)
{
    //this function pushes a filled event to the consumer queue. This should *always* succeed.
    //The guarantee of success comes because if we managed to get an empty event to fill
    //from the producer queue then there *must* be a slot open in the consumer queue to send it to
    bool success = this->consumerQueue->push(data);
    //since we have enqueued a data object, make certain that the consumer is awake to get it
    if(this->consumerWaiting.load() > 0)//no need to check for success it is guaranteed
    {
        this->consumerWaitCondition.notify_one();
    }
    return success;
}

template <typename QueueContents, QueueSizes qSize>
bool QueuePair<boost::lockfree::spsc_queue, QueueContents, qSize>::popFromConsumerQueue(QueueContents*& data)
{
    //this function trys to pop from the queue and waits until there is a data available to do so
    bool success = false;
    while (!(success = this->consumerQueue->pop(data)) && this->notForceWake.load())
    {
        //we were not able to pop from the queue, so clearly the producer is lagging
        //make certain the producer is not waiting on something and then wait until the consumer has cleared an element
        if(this->producerWaiting.load() > 0)
        {
            this->producerWaitCondition.notify_all();
        }
        //now wait for some full events
        ++(this->consumerWaiting);
        boost::unique_lock<boost::mutex> waitLock(this->consumerWaitMutex);
        this->consumerWaitCondition.wait(waitLock);
        //if we are here then we have woken from the wait, note this
        --(this->consumerWaiting);
    }
    return success;
}

template <typename QueueContents, QueueSizes qSize>
bool QueuePair<boost::lockfree::spsc_queue, QueueContents, qSize>::pushToProducerQueue(QueueContents* data)
{
    //this function tries to push to the producer queue (which returns empty objects to the producer)
    //as, by design, the producer queue has 5 extra slots relative to the consumer queue, and the producer queue
    //was only initially filled by empties to the capacity of the consumer queue, this will *always* succeed.
    bool success = this->producerQueue->push(data);
    //if we have enqueued an empty event, make certain that the producer queue knows
    if(this->producerWaiting.load() > 0)//no need to check for success it is guaranteed
    {
        this->producerWaitCondition.notify_one();
    }
    return success;
}

template <typename QueueContents, QueueSizes qSize>
bool QueuePair<boost::lockfree::spsc_queue, QueueContents, qSize>::popFromProducerQueue(QueueContents*& data)
{
    //this function tries to pop from the producer queue, if it is empty (ie all the buffers are currently in
    //the producer queue) then we wait for the consumer to make some more empties for us to use
    bool success = false;
    while (!(success = this->producerQueue->pop(data)) && this->notForceWake.load())
    {
        //if we are here then we are not able to pop from the queue so clearly the
        //consumer is lagging, make certain that the consumer is not waiting for
        //some silly reason
        if(this->consumerWaiting.load())
        {
            this->consumerWaitCondition->notify_all();
        }
        //now wait for some empty events
        ++(this->producerWaiting);
        boost::unique_lock<boost::mutex> waitLock(this->producerWaitMutex);
        this->producerWaitCondition.wait(waitLock);
        //if we are here then we need to note that we have woken up
        --(this->producerWaiting);
    }
    return success;
}

template <typename QueueContents, QueueSizes qSize>
bool QueuePair<boost::lockfree::queue, QueueContents, qSize>::pushToConsumerQueue(QueueContents* data)
{
    //this function pushes a filled event to the consumer queue. This should *always* succeed.
    //The guarantee of success comes because if we managed to get an empty event to fill
    //from the producer queue then there *must* be a slot open in the consumer queue to send it to
    bool success =  this->consumerQueue->bounded_push(data);
    //since we have enqueued a data object, make certain that the consumer is awake to get it
    if(this->consumerWaiting.load() > 0)//no need to check for success it is guaranteed
    {
        this->consumerWaitCondition.notify_one();
    }
    return success;
}

template <typename QueueContents, QueueSizes qSize>
bool QueuePair<boost::lockfree::queue, QueueContents, qSize>::popFromConsumerQueue(QueueContents*& data)
{
    //this function trys to pop from the queue and waits until there is a data available to do so
    bool success = false;
    while (!(success = this->consumerQueue->pop(data)) && this->notForceWake.load())
    {
        //we were not able to pop from the queue, so clearly the producer is lagging
        //make certain the producer is not waiting on something and then wait until the consumer has cleared an element
        if(this->producerWaiting.load() > 0)
        {
            this->producerWaitCondition.notify_all();
        }
        //now wait for some full events
        ++(this->consumerWaiting);
        boost::unique_lock<boost::mutex> waitLock(this->consumerWaitMutex);
        this->consumerWaitCondition.wait(waitLock);
        //if we are here then we have woken from the wait, note this
        --(this->consumerWaiting);
    }
    return success;
}

template <typename QueueContents, QueueSizes qSize>
bool QueuePair<boost::lockfree::queue, QueueContents, qSize>::pushToProducerQueue(QueueContents* data)
{
    //this function tries to push to the producer queue (which returns empty objects to the producer)
    //as, by design, the producer queue has 5 extra slots relative to the consumer queue, and the producer queue
    //was only initially filled by empties to the capacity of the consumer queue, this will *always* succeed.
    bool success = this->producerQueue->bounded_push(data);
    //if we have enqueued an empty event, make certain that the producer queue knows
    if(this->producerWaiting.load() > 0)//no need to check for success it is guaranteed
    {
        this->producerWaitCondition.notify_one();
    }
    return success;
}

template <typename QueueContents, QueueSizes qSize>
bool QueuePair<boost::lockfree::queue, QueueContents, qSize>::popFromProducerQueue(QueueContents*& data)
{
    //this function tries to pop from the producer queue, if it is empty (ie all the buffers are currently in
    //the producer queue) then we wait for the consumer to make some more empties for us to use
    bool success = false;
    while (!(success = this->producerQueue->pop(data)) && this->notForceWake.load())
    {
        //if we are here then we are not able to pop from the queue so clearly the
        //consumer is lagging, make certain that the consumer is not waiting for
        //some silly reason
        if(this->consumerWaiting.load())
        {
            this->consumerWaitCondition->notify_all();
        }
        //now wait for some empty events
        ++(this->producerWaiting);
        boost::unique_lock<boost::mutex> waitLock(this->producerWaitMutex);
        this->producerWaitCondition.wait(waitLock);
        //if we are here then we need to note that we have woken up
        --(this->producerWaiting);
    }
    return success;
}

template <typename QueueContents, QueueSizes qSize>
inline bool QueuePair<boost::lockfree::spsc_queue, QueueContents, qSize>::tryPushToConsumerQueue(QueueContents* data)
{
    //as stated above, push always works, therefor push, wake and return
    bool success = this->consumerQueue->push(data);
    //since we have enqueued a data object, make certain that the consumer is awake to get it
    if(this->consumerWaiting.load() > 0)//no need to check for success it is guaranteed
    {
        this->consumerWaitCondition.notify_one();
    }
    return success;
}

template <typename QueueContents, QueueSizes qSize>
inline bool QueuePair<boost::lockfree::spsc_queue, QueueContents, qSize>::tryPopFromConsumerQueue(QueueContents*& data)
{
    //try to pop from the consumer queue, return true for success or false for failure
    return this->consumerQueue->pop(data);
}

template <typename QueueContents, QueueSizes qSize>
inline bool QueuePair<boost::lockfree::spsc_queue, QueueContents, qSize>::tryPushToProducerQueue(QueueContents* data)
{
    bool success =  this->producerQueue->push(data);
    //since we have enqueued a data object, make certain that the consumer is awake to get it
    if(this->producerWaiting.load() > 0)//no need to check for success it is guaranteed
    {
        this->producerWaitCondition.notify_one();
    }
    return success;
}

template <typename QueueContents, QueueSizes qSize>
inline bool QueuePair<boost::lockfree::spsc_queue, QueueContents, qSize>::tryPopFromProducerQueue(QueueContents*& data)
{
    //try to pop from the producer queue
    return this->producerQueue->pop(data);
}

template <typename QueueContents, QueueSizes qSize>
inline bool QueuePair<boost::lockfree::queue, QueueContents, qSize>::tryPushToConsumerQueue(QueueContents* data)
{
    //as stated above, push always works, therefor push, wake and return
    bool success = this->consumerQueue->bounded_push(data);
    //since we have enqueued a data object, make certain that the consumer is awake to get it
    if(this->consumerWaiting.load() > 0)//no need to check for success it is guaranteed
    {
        this->consumerWaitCondition.notify_one();
    }
    return success;
}

template <typename QueueContents, QueueSizes qSize>
inline bool QueuePair<boost::lockfree::queue, QueueContents, qSize>::tryPopFromConsumerQueue(QueueContents*& data)
{
    //try to pop from the consumer queue, return true for success or false for failure
    return this->consumerQueue->pop(data);
}

template <typename QueueContents, QueueSizes qSize>
inline bool QueuePair<boost::lockfree::queue, QueueContents, qSize>::tryPushToProducerQueue(QueueContents* data)
{
    bool success =  this->producerQueue->bounded_push(data);
    //since we have enqueued a data object, make certain that the consumer is awake to get it
    if(this->producerWaiting.load() > 0)//no need to check for success it is guaranteed
    {
        this->producerWaitCondition.notify_one();
    }
    return success;
}

template <typename QueueContents, QueueSizes qSize>
inline bool QueuePair<boost::lockfree::queue, QueueContents, qSize>::tryPopFromProducerQueue(QueueContents*& data)
{
    //try to pop from the producer queue
    return this->producerQueue->pop(data);
}

}
#endif //ORCHID_SRC_INTERTHREADCOMM_QUEUEPAIR_H
