/***************************************************************************//**
********************************************************************************
**
** @file MultiQueueTMP.h
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
#ifndef ORCHID_SRC_INTERTHREADCOMM_MULTIQUEUETMP_H
#define ORCHID_SRC_INTERTHREADCOMM_MULTIQUEUETMP_H

// includes for C system headers
// includes for C++ system headers
#include<tuple>
#include<atomic>
// includes from other libraries
#include<boost/lockfree/policies.hpp>
#include<boost/lockfree/spsc_queue.hpp>
#include<boost/lockfree/queue.hpp>
#include<boost/thread.hpp>
// includes from ORCHID
#include"InterThreadComm/InterThreadQueueSizes.h"

namespace InterThread
{

/*
 * Generate a tuple with Count repititions of the given type
 */
//a bit of template metaprogramming so that I can expand a single usage of type
//into a parameter pack of arbitrary length
//The first template parameter is how many copies of the parameter type to stick
//into the parameter pack, the second template parameter is the type to stick
//into the parameter pack, and the final template parameter is the pack with
//the current listing of parameters
template<int Count, typename Type, typename ...Types>
struct TupleTypeGenRec
{
    typedef TupleType TuplePackGenRec<Count-1, Type, Type, Types...>::TupleType;
};

//template specialization that ends the recursion
//at this point the parameter pack Types should hold N copies of Type where N is
//the value passed to the Count parameter of TupleTypeGen
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



/*
 * Generate a tuple with expanded boost lockfree queues with expanded templates
 * to the content type and the given capacity
 */
//this is the recursive part of the template metaprogramming, the parameters are the same as before, but
//we also have a count down integer at the end that is zero when we have everything, and a parameter pack
//that holds the list of expanded queue types
template<typename QueueContent, QueueSizes qSize, QueueSizes ...QSizes, typename qType, typename ...QTypes, int Count, typename ...TemplatedQueues>
struct QueueTupleTypeGenRec
{
    typedef TupleType QueueTupleTypeGenRec<QueueContent, QSizes..., QTypes..., Count-1, TemplatedQueues..., qType<QueueContent, boost::lockfree::capacity<getEnumVal(qSize)>>>::TupleType;
};

//base case to end the recursion
template<typename QueueContent, QueueSizes qSize, QueueSizes ...QSizes, typename qType, typename ...QTypes, typename TemplatedQueues>
struct QueueTupleTypeGenRec<QueueContent, qSize, QSizes..., qType, QTypes..., 1, TemplatedQueues>
{
    typedef TupleType QueueTupleTypeGenFinal<TemplatedQueues..., qType<QueueContent, boost::lockfree::capacity<getEnumVal(qSize)>>>::TupleType;
}; 

//simple result of base case
template<typename ...TemplatedQueues>
struct QueueTupleTypeGenFinal
{
    typedef TupleType std::tuple<TemplatedQueues...>;
};

//this is the type that is called to generate the tuple of queues
//this is the intial loop part of the template metaprogramming recursion where I
//start breaking apart the parameter packs and expanding thing
//The parameter QueueContent is the actual contents of the queue, so if you want
//pointers to something pass a pointer, unlike the multiqueue class this does not
//assume that it needs to convert things into pointers
//the parameter qSize is the first parameter of the QSizes parameter pack peeled off
//the parmeter qType is the first parmeter of the QTypes parameter pack peeled off
template<typename QueueContent, QueueSizes qSize, QueueSizes ...QSizes, typename qType, typename ...QTypes>
struct QueueTupleTypeGen
{
    typedef TupleType QueueTupleTypeGenRec<QueueContent, QSizes, QTypes, sizeof...(QTypes), qType<QueueContent, boost::lockfree::capacity<getEnumVal(qSize)>>>::TupleType;
};



/*
 * Obtain the correct push function, be it bounded push or simple push (ie spsc queue or queue from lockfree)
 */
//general case
template<typename QueueType, typename QueueContent>
struct PushSelect
{
    static bool push(QueueType& queue, QueueContent* data);
};

//speciallization for spsc_queue
template<typename QueueContent, typename QueueParam>
struct PushSelect<boost::lockfree::spsc_queue<QueueContent*, QueueParam>, QueueContent>
{
    static bool push(boost::lockfree::spsc_queue<QueueContent*, QueueParam>& queue, QueueContent* data)
    {return queue.push(data);}
};

//speciallization for queue
template<typename QueueContent, typename QueueParam>
struct PushSelect<boost::lockfree::queue<QueueContent*, QueueParam>, QueueContent>
{
    static bool push(boost::lockfree::queue<QueueContent*, QueueParam>& queue, QueueContent* data)
    {return queue.bounded_push(data);}
};

/*
 * Stuff needed to apply a functor to everything in a tuple
 */
namespace ForEachDetail
{

template<int ...Indices>
struct Sequence{};

template<int index, int ...Indices>
struct GenSequence: GenSequence<index - 1, index - 1, Indices...> {};

template<int ...Indices>
struct GenSequence<0, Indices...>: seq<Indices...> {};

template<typename Type, typename FunctorType, int ...Indices>
void forEachDetail(Type&& t, FunctorType& f, seq<Indices...>)
{
    auto l = { f(std::get(Indices)(t))... };
}

}

template<typename... TupleTypes, typename FunctorType>
void forEachInTuple(std::tuple<TuplesTypes...>& t, FunctorType& f)
{
    ForEachDetail::forEachDetail(t, f, detail::GenSequence<sizeof...(TupleTypes)>());
}

/*
 * Functor to set atomic values
 */
template<typename AtomicType, AtomicType value>
struct SetAtomicFunctor
{
    static void operator()(std::atomic<AtomicType>& toBeSet){toBeSet.store(value);}
};

/*
 * Functor to apply notify_all to all condition variables in tuple
 */
struct WakeAllConditionsFunctor
{
    static void operator()(boost::condition_variable& condVar){condVar.notify_all();}
};

}
#endif //ORCHID_SRC_INTERTHREADCOMM_MULTIQUEUETMP_H
