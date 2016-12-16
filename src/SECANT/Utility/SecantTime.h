/***************************************************************************//**
********************************************************************************
**
** @file SecantTime.h
** @author James Till Matta
** @date 03 June, 2016
** @brief
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** @details Holds the code to use boost::chrono and the like to measure us since
** the epoch
**
********************************************************************************
*******************************************************************************/
#ifndef SECANT_SRC_UTILITY_SECANTTIME_H
#define SECANT_SRC_UTILITY_SECANTTIME_H
// includes for C system headers
// includes for C++ system headers
// includes from other libraries
#include<boost/date_time/posix_time/posix_time.hpp>
// includes from SECANT

namespace Secant
{
namespace Time
{

unsigned long long getTimeSinceEpoch()
{
    static const boost::posix_time::ptime Epoch(boost::posix_time::time_from_string("1970-01-01 00:00:00.000"));
    return (boost::posix_time::microsec_clock::universal_time() - Epoch).total_microseconds();
}

}
}

#endif //SECANT_SRC_UTILITY_SECANTTIME_H
