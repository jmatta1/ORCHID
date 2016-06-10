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

enum class QueueSizes : int 
{
    SlowControlToFile       = 50,
    DigitizerToProcessing   = 5000,
    ProcessingToFile        = 20000
};

//here is a quick template meta program to cast the strongly typed enums to ints
//for purposes of accessing the actual values
template<typename Enum>
constexpr typename std::underlying_type<Enum>::type getEnumVal(Enum val)
{
    return static_cast<typename std::underlying_type<Enum>::type>(val);
}

}

#endif //ORCHID_SRC_INTERTHREADCOMM_INTERTHREADQUEUESIZES_H
