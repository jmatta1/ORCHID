/***************************************************************************//**
********************************************************************************
**
** @file RateData.h
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
** @details Holds the definition of the class that is used to accumulate trigger
** information from the digitizers, that in turn is used to calculate trigger
** rates by the ui
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHID_SRC_DIGILIB_RATEDATA_H
#define ORCHID_SRC_DIGILIB_RATEDATA_H
// includes for C system headers
// includes for C++ system headers
#include<atomic>
// includes from other libraries
// includes from ORCHID

namespace FastData
{

struct RateData
{
    RateData(int numDigiChan);
    ~RateData();
    void clearTrigs();
    void addTrigs(int chan, unsigned count);

    std::atomic_uint* triggers;

    int numDigitizerChannels;
};

}
#endif //ORCHID_SRC_DIGILIB_RATEDATA_H
