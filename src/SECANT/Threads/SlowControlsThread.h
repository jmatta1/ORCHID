/***************************************************************************//**
********************************************************************************
**
** @file SecantSlowControlsThread.h
** @author James Till Matta
** @date 16 Dec, 2016
** @brief Contains the SECANT slow controls thread functor
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** @details Definition file for the SecantSlowControlsThread functor class
** This class is the one thread class that is not derived from SecantThreadInterface
** the reason is that because this class is much less time critical it can afford
** to use virtual functions in registered base classes to poll the relevant
** hardware therefore it has no need for template madness and thus no need for
** there to be an abstract base class to wipe away the issues of storing a large
** set of pointers to heavily and variably templated classes
** 
********************************************************************************
*******************************************************************************/
#ifndef SECANT_SRC_THREADS_SECANTSLOWCONTROLSTHREAD_H
#define SECANT_SRC_THREADS_SECANTSLOWCONTROLSTHREAD_H

// includes for C system headers
// includes for C++ system headers
#include<memory>
#include<vector>
// includes from other libraries
// includes from SECANT
#include"SECANT/UserInterfaces/SlowControlsInterface.h"
#include"SECANT/IO/SecantFileWriter.h"

namespace Secant
{

namespace Threads
{

/*!
 * @class SecantSlowControlsThread
 * 
 * @ingroup SecantThreads
 * 
 * @brief This class is the functor for the SECANT slow controls thread
 * 
 * This class takes a list of registered slow controls interfaces provided by
 * the user and probes each of those interfaces at regular intervals
 * 
 * @author Author: James Till Matta
 */
class SlowControlsThread
{
    using HardwareList = std::vector<std::unique_ptr<Interfaces::SlowControlsInterface> >;
public:
    /**
     * @brief SlowControlsThread Default Contructor
     * 
     * Builds a new slow controls thread functor.
     */
    SlowControlsThread();
    
    /**
     * @brief SlowControlsThread Destructor
     * 
     * Deletes the current slow controls thread functor.
     */
    SlowControlsThread();
    
    /** @brief Register Slow Controls Hardware Object
     * 
     * @param newHardware unique pointer to an object that inherits the SlowControlsInterface
     * 
     * @return True if the registration occurred before this thread was started, False otherwise
     * 
     * This function appends the given slow controls interface pointer to the
     * vector of pointers that are to be checked every loop. It will not do this
     * if the thread is actually running as opposed to sitting idle before starting
     */
    bool registerSlowControlsObject(std::unique_ptr<Interfaces::SlowControlsInterface>&& newHardware);
    
    /** @brief operator()
    * 
    * This function is the entry point for the thread to call. Within this
    * function the current state of the thread is examined and appropriate
    * actions are taken based on that
    */
    void operator()();
private:
    //the actual list of slow controls devices to be probed
    /**
     * @brief hardwareList
     * 
     * This vector stores the list of unique pointers to the slow controls hardware
     */
    HardwareList hardwareList;

    //the file to hold the output of the slow controls thread
    /**
     * @brief outputFile
     * 
     * The file that is used to write slow controls data to disk
     */
    IO::SecantFileWriter outputFile;
    
    /**
     * @brief outputArray
     * 
     * This holds the character array that slow controls dumps data to for event
     * outputting
     */
    std::unique_ptr<char*> outputArray;
    
    /**
     * @brief outputArraySize
     * 
     * Contains the size of the array that slow controls dumps data to
     */
    std::size_t outputArraySize;
};

}

}
#endif //SECANT_SRC_THREADS_SECANTTHREADINTERFACE_H
