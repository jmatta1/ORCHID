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
    typedef typename TupleTypeGenRec<Count-1, Type, Type, Types...>::TupleType TupleType;
};

//template specialization that ends the recursion
//at this point the parameter pack Types should hold N copies of Type where N is
//the value passed to the Count parameter of TupleTypeGen
template<typename Type, typename ...Types>
struct TupleTypeGenRec<0, Type, Types...>
{
    typedef std::tuple<Types...> TupleType;
};

//this is the type that actually needs to be called to make the magic happen
template<int Count, typename Type>
struct TupleTypeGen
{
    typedef typename TupleTypeGenRec<Count-1, Type, Type>::TupleType TupleType;
};


/*
 * Obtain the correct push function, be it bounded push or simple push (ie spsc queue or queue from lockfree)
 */
//general case
template<typename QueueType, typename QueueContent>
struct PushSelect
{
    static bool push(QueueType& queue, QueueContent data);
};

//speciallization for spsc_queue
template<typename QueueContent, typename QueueParam>
struct PushSelect<boost::lockfree::spsc_queue<QueueContent, QueueParam>, QueueContent>
{
    static bool push(boost::lockfree::spsc_queue<QueueContent, QueueParam>& queue, QueueContent data)
    {return queue.push(data);}
};

//speciallization for queue
template<typename QueueContent, typename QueueParam>
struct PushSelect<boost::lockfree::queue<QueueContent, QueueParam>, QueueContent>
{
    static bool push(boost::lockfree::queue<QueueContent, QueueParam>& queue, QueueContent data)
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
struct GenSequence<0, Indices...>: Sequence<Indices...> {};

template<typename Type, typename FunctorType, int ...Indices>
void forEachDetail(Type&& t, FunctorType f, Sequence<Indices...>)
{
    auto l = { f(std::get<Indices>(t))... };
}

}

template<typename... TupleTypes, typename FunctorType>
void forEachInTuple(std::tuple<TupleTypes...>& t, FunctorType f)
{
    ForEachDetail::forEachDetail(t, f, ForEachDetail::GenSequence<sizeof...(TupleTypes)>());
}

/*
 * Functor to set atomic values
 */
template<typename AtomicType, AtomicType value>
struct SetAtomicFunctor
{
    int operator()(std::atomic<AtomicType>& toBeSet){toBeSet.store(value); return 0;}
};

/*
 * Functor to apply notify_all to all condition variables in tuple
 */
struct WakeAllConditionsFunctor
{
    int operator()(boost::condition_variable& condVar){condVar.notify_all(); return 0;}
};

}
#endif //ORCHID_SRC_INTERTHREADCOMM_MULTIQUEUETMP_H
