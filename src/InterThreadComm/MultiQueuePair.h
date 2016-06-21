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
#include<tuple>
// includes from other libraries
#include<boost/thread.hpp>
// includes from ORCHID


namespace InterThread
{

//a bit of template metaprogramming so that I can expand a single usage of type
//into a parameter pack of arbitrary length
template<int Count, typename Type, typename ...Types>
struct TupleTypeGenRec
{
    typedef TupleType TuplePackGenRec<Count-1, Type, Type, Types>::TupleType;
};

template<typename Type, typename ...Types>
struct TupleTypeGenRec<0, Type, Types...>
{
    typedef TupleType std::tuple<Types...>;
};

//this is the type that actually needs to be called to make the magic happen
template<int Count, typename Type>
struct TupleTypeGen
{
    typedef TupleType TupleTypeGenRec<Count-1, Type, Type>::TupleType;
};

template <typename QueueContent, typename ... QueueTypes>
class MultiQueuePair
{
public:
    MultiQueuePair();
    ~MultiQueuePair();
    
    template<int QueueIndex>
    bool consumerPush(QueueContent* data);
    template<int QueueIndex>
    bool consumerPop(QueueContent* data);
    template<int QueueIndex>
    bool producerPush(QueueContent* data);
    template<int QueueIndex>
    bool producerPop(QueueContent* data);
    
    template<int QueueIndex>
    bool tryConsumerPush(QueueContent* data);
    template<int QueueIndex>
    bool tryConsumerPop(QueueContent* data);
    template<int QueueIndex>
    bool tryProducerPush(QueueContent* data);
    template<int QueueIndex>
    bool tryProducerPop(QueueContent* data);
private:
    std::tuple<QueueTypes...> consumerQueue;
    std::tuple<QueueTypes...> producerQueue;
    
    //lists of threading structures necessary for waiting of the various producer threads
    TupleTypeGen<sizeof...(QueueTypes), boost::mutex>::TupleType producerWaitMutex;
    TupleTypeGen<sizeof...(QueueTypes), boost::condition_variable>::TupleType producerWaitCondition;
    
    //threading structures necessary for the waiting of the consumer queue
    boost::mutex consumerMutex;
    boost::condition_variable consumerWaitCondition;
};



}

#endif //ORCHID_SRC_INTERTHREADCOMM_MULTIQUEUEPAIR_H
