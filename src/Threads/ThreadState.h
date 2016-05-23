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
** @details Holds the definition of the class that is run by main to handle the
** user interface
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

enum class TState  : char {Running, Waiting, Stopping};
typedef atomic<TState> ThreadState;

enum class DigMode : char {PSDWaveforms, PSDIntegrals, PSDBoth, Waveforms};
typedef atomic<DigMode> DigitizerMode;

#endif //ORCHID_SRC_THREADS_THREADSTATE_H
