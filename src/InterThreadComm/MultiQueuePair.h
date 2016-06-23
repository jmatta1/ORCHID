/***************************************************************************//**
********************************************************************************
**
** @file MultiQueuePair.h
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
** @details Holds definition of a system that allows a single consumer to
** consume data of multiple types of producers simultaneously
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
#include"InterThreadQueueSizes.h"
#include"MultiQueueTMP.h"


namespace InterThread
{

template <typename QueueContent, QueueSizes ... QSizes, typename ... QueueTypes>
class MultiQueuePair
{
public:
    MultiQueuePair():consumerWaitCount(false), notForcedAwakening(true){forEachInTuple(this->producerWaitCounters, SetAtomicFunctor<int, 0>);}
    ~MultiQueuePair(){}
    
    //the producer threads pop empty objects from their return queue and push
    //full data objects onto the data queue
    template<int ProducerInd>
    bool producerPop(QueueContent*& data);
    template<int ProducerInd>
    bool producerPush(QueueContent* data);
    template<int ProducerInd>
    bool tryProducerPop(QueueContent*& data);
    template<int ProducerInd>
    bool tryProducerPush(QueueContent* data);
    
    //the consumer thread pops full objects from the data queue and pushes empty
    //objects onto the return queue
    template<int ProducerInd>
    bool consumerPop(QueueContent*& data);
    template<int ProducerInd>
    bool consumerPush(QueueContent* data);
    template<int ProducerInd>
    bool tryConsumerPop(QueueContent*& data);
    template<int ProducerInd>
    bool tryConsumerPush(QueueContent* data);
    
    
    //function for the file thread to use to wait for data
    //this has no checks for the existance of data, such must be outside this class
    void consumerWaitForData()
    {
        boost::unique_lock<boost::mutex> waitLock(this->consumerWaitMutex);
        ++(this->consumerWaitCount);
        this->consumerWaitCondition.wait(waitLock);
        --(this->consumerWaitCount);
    }
    
    void wakeOneConsumer(){if(consumerWaitCount.load() > 0) this->consumerWaitCondition.notify_one();}
    void wakeAllConsumer(){if(consumerWaitCount.load() > 0) this->consumerWaitCondition.notify_all();}
    template<int ProducerInd>
    void wakeOneProducer(){if(std::get<ProducerInd>(this->producerWaitCounters).load() > 0) std::get<ProducerInd>(this->producerWaitConditions).notify_one();}
    template<int ProducerInd>
    void wakeAllProducer(){if(std::get<ProducerInd>(this->producerWaitCounters).load() > 0) std::get<ProducerInd>(this->producerWaitConditions).notify_all();}
    
    void forceWakeAll(){notForcedAwakening.store(false); 
                        forEachInTuple(this->producerWaitConditions, WakeAllConditionsFunctor());
                        this->consumerWaitCondition.notify_all();}
    
    void clearForce(){this->notForcedAwakening.store(true);}
    
private:
    //tuple full of queues for producers to send full objects to the consumer
    QueueTupleTypeGen<QueueContent*, QSizes..., QueueTypes...>::TupleType dataQueues;
    //tuple full of queues for the consumer to send empty objects to the producers
    QueueTupleTypeGen<QueueContent*, QSizes..., QueueTypes...>::TupleType returnQueues;
    
    //thread structures necessary for producer waiting
    TupleTypeGen<sizeof...(QueueTypes), boost::mutex>::TupleType producerWaitMutexes;
    TupleTypeGen<sizeof...(QueueTypes), boost::condition_variable>::TupleType producerWaitConditions;
    TupleTypeGen<sizeof...(QueueTypes), std::atomic_int>::TupleType producerWaitCounters;
    
    //threading structures necessary for the waiting of the consumer queue
    boost::mutex consumerWaitMutex;
    boost::condition_variable consumerWaitCondition;
    std::atomic_int consumerWaitCount;
    
    //hold whether or not our awakening has been forced
    std::atomic_bool notForcedAwakening;
};

template <typename QueueContent, QueueSizes ... QSizes, typename ... QueueTypes>
template<int ProducerInd>
bool MultiQueuePair<QueueContent, QSizes..., QueueTypes...>::producerPop(QueueContent*& data)
{
    bool success = false;
    while(!(success = std::get<ProducerInd>(this->returnQueues).pop(data)) && this->notForcedAwakening.load())
    {
        //if we could not pop from the queue then we should make certain that the consumer has
        //not stalled in some way
        if(this->consumerWaitCount.load() > 0)
        {
            this->consumerWaitCondition.notify_all();
        }
        //then prepare to go to sleep
        ++(std::get<ProducerInd>(this->producerWaitCounters));
        boost::unique_lock<boost::mutex> waitLock(std::get<ProducerInd>(this->producerWaitMutexes));
        std::get<ProducerInd>(this->producerWaitConditions).wait(waitLock);
        --(std::get<ProducerInd>(this->producerWaitCounters));
    }
    return success;
}

template <typename QueueContent, QueueSizes ... QSizes, typename ... QueueTypes>
template<int ProducerInd>
bool MultiQueuePair<QueueContent, QSizes..., QueueTypes...>::producerPush(QueueContent* data)
{
    //this function pushes a filled event to the consumer queue. This should *always* succeed.
    //The guarantee of success comes because if we managed to get an empty event to fill
    //from the producer queue then there *must* be a slot open in the consumer queue to send it to
    //much ugliness where we actually call the push function
    bool success = PushSelect<std::tuple_element<ProducerInd, decltype(this->dataQueues)>::type, QueueContent>::push(std::get<ProducerInd>(this->dataQueues), data);
    //since we have enqueued a data object, make certain that the consumer is awake to get it
    if(this->consumerWaitCount.load() > 0)//no need to check for success it is guaranteed
    {
        this->consumerWaitCondition.notify_one();
    }
    return success;
}

template <typename QueueContent, QueueSizes ... QSizes, typename ... QueueTypes>
template<int ProducerInd>
bool MultiQueuePair<QueueContent, QSizes..., QueueTypes...>::tryProducerPop(QueueContent*& data)
{
    return std::get<ProducerInd>(this->returnQueues).pop(data);
}

template <typename QueueContent, QueueSizes ... QSizes, typename ... QueueTypes>
template<int ProducerInd>
bool MultiQueuePair<QueueContent, QSizes..., QueueTypes...>::tryProducerPush(QueueContent* data)
{
    bool success = PushSelect<std::tuple_element<ProducerInd, decltype(this->dataQueues)>::type, QueueContent>::push(std::get<ProducerInd>(this->dataQueues), data);
    //since we have enqueued a data object, make certain that the consumer is awake to get it
    if(this->consumerWaitCount.load() > 0)//no need to check for success it is guaranteed
    {
        this->consumerWaitCondition.notify_one();
    }
    return success;
}

template <typename QueueContent, QueueSizes ... QSizes, typename ... QueueTypes>
template<int ProducerInd>
bool MultiQueuePair<QueueContent, QSizes..., QueueTypes...>::consumerPop(QueueContent*& data)
{
    bool success = false;
    while(!(success = std::get<ProducerInd>(this->dataQueues).pop(data)) && this->notForcedAwakening.load())
    {
        if(std::get<ProducerInd>(this->producerWaitCounters).load() > 0)
        {
            std::get<ProducerInd>(this->producerWaitConditions).notify_all();
        }
        
        ++(this->consumerWaitCount);
        boost::unique_lock<boost::mutex> waitLock(this->consumerWaitMutex);
        this->consumerWaitCondition.wait(waitLock);
        --(this->consumerWaitCount);
    }
    return success;
}

template <typename QueueContent, QueueSizes ... QSizes, typename ... QueueTypes>
template<int ProducerInd>
bool MultiQueuePair<QueueContent, QSizes..., QueueTypes...>::consumerPush(QueueContent* data)
{
    bool success = PushSelect<std::tuple_element<ProducerInd, devltype(this->returnQueues)>::type, QueueContent>::push(std::get<ProducerInd>(this->returnQueues), data);
    if(std::get<ProducerInd>(this->producerWaitCounters).load() > 0)
    {
        std::get<ProducerInd>(this->producerWaitConditions).notify_one();
    }
    return success;
}

template <typename QueueContent, QueueSizes ... QSizes, typename ... QueueTypes>
template<int ProducerInd>
bool MultiQueuePair<QueueContent, QSizes..., QueueTypes...>::tryConsumerPop(QueueContent*& data)
{
    return std::get<ProducerInd>(this->dataQueues).pop(data);
}

template <typename QueueContent, QueueSizes ... QSizes, typename ... QueueTypes>
template<int ProducerInd>
bool MultiQueuePair<QueueContent, QSizes..., QueueTypes...>::tryConsumerPush(QueueContent* data)
{
    bool success = PushSelect<std::tuple_element<ProducerInd, devltype(this->returnQueues)>::type, QueueContent>::push(std::get<ProducerInd>(this->returnQueues), data);
    if(std::get<ProducerInd>(this->producerWaitCounters).load() > 0)
    {
        std::get<ProducerInd>(this->producerWaitConditions).notify_one();
    }
    return success;
}

}

#endif //ORCHID_SRC_INTERTHREADCOMM_MULTIQUEUEPAIR_H
