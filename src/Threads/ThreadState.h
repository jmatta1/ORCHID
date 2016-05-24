/***************************************************************************//**
********************************************************************************
**
** @file ThreadState.h
** @author James Till Matta
** @date 27 Apr, 2016
** @brief
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** @details Holds the definition of the enums used to control threads also
** contains the typedefs that wrap them in atomic structures
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHID_SRC_THREADS_THREADSTATE_H
#define ORCHID_SRC_THREADS_THREADSTATE_H

// includes for C system headers
// includes for C++ system headers
#include<atomic>
using std::atomic;
// includes from other libraries
// includes from ORCHID

namespace Threads
{

enum class TState  : char {Running, Waiting, Stopping};
typedef atomic<TState> ThreadState;

}
#endif //ORCHID_SRC_THREADS_THREADSTATE_H
