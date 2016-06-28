/***************************************************************************//**
********************************************************************************
**
** @file FileOutputThread.h
** @author James Till Matta
** @date 27 June, 2016
** @brief
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** @details Holds a callable class that allows us to get around the restriction
** that a boost thread needs a copyable object passed to it.
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHID_SRC_THREADS_FILEOUTPUTSHELL_H
#define ORCHID_SRC_THREADS_FILEOUTPUTSHELL_H

// includes for C system headers
// includes for C++ system headers
// includes from other libraries
// includes from ORCHID
#include"FileOutputThread.h"

namespace Threads
{

template <typename ThreadCallable>
class ThreadWrapper
{
public:
    ThreadWrapper(ThreadCallable* callable):threadCallable(callable){}
    ~ThreadWrapper(){}
    void operator()(){(*threadCallable)();}
    
private:
    ThreadCallable* threadCallable;
};

}
#endif //ORCHID_SRC_THREADS_FILEOUTPUTSHELL_H
