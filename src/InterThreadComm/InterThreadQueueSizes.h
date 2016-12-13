/***************************************************************************//**
********************************************************************************
**
** @file InterThreadQueueSizes.h
** @author James Till Matta
** @date 08 June, 2016
** @brief
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** @details Holds the various constants governing sizes of interthread queues
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHID_SRC_INTERTHREADCOMM_INTERTHREADQUEUESIZES_H
#define ORCHID_SRC_INTERTHREADCOMM_INTERTHREADQUEUESIZES_H

// includes for C system headers
// includes for C++ system headers
#include<type_traits>
// includes from other libraries
// includes from ORCHID

namespace InterThread
{

namespace QueueSizes
{

static const int AcqToProc = 80;
//Digitizer to processing can be *REALLY* big, for 0 trace samples
//  (12 bytes per event) and the maximum of 1023 events per channel aggregate and
//  255 board aggregates per BLT one gets a little under 24MB per buffer
}

}

#endif //ORCHID_SRC_INTERTHREADCOMM_INTERTHREADQUEUESIZES_H
