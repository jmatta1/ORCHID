/***************************************************************************//**
********************************************************************************
**
** @file SecantThreadInterface.h
** @author James Till Matta
** @date 16 Dec, 2016
** @brief
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** @details Definition file for the abstract interface to SECANT threads this is
** used to hide the details of the derived templated threads from the type system
** so that I do not need to go really crazy in templatizing the SECANT core
**
** \defgroup SecantThreads Secant Thread Classes
** 
********************************************************************************
*******************************************************************************/
#ifndef SECANT_SRC_THREADS_THREADINTERFACE_H
#define SECANT_SRC_THREADS_THREADINTERFACE_H


namespace Secant
{

namespace Threads
{

/*!
 * @class SecantThreadInterface
 * 
 * @ingroup SecantThreads
 * 
 * @brief This class defines the interface for thread functors
 * 
 * This class exists to prevent the need for massive templatization of the
 * SecantCore class. Instead of needing to store many separate templated
 * thread functor pointers, it merely needs to store the base class pointers
 * and pass those to the threads being created.
 * 
 * @author Author: James Till Matta
 */
class ThreadInterface
{
public:
    /** @brief Virtual Destructor
    * 
    * This function ensures that the derived class destructor is called. This is
    * important because the derived class takes ownership of the user provided
    * objects that are passed to it and so when the base class pointer is
    * deleted then the derived class needs to delete itself. Since it is only
    * called once in the thread functor's lifetime I deem this virtual call to
    * be an acceptable cost
    */
    virtual ~ThreadInterface(){}
    
    /** @brief Function Call Operator
    * 
    * This function is the entry point for the thread to call. Since it is only
    * called once at thread start this is a reasonable virtual call usage.
    */
    virtual void operator()() = 0;
};

}

}
#endif //SECANT_SRC_THREADS_SECANTTHREADINTERFACE_H
