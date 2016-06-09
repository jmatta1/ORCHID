/***************************************************************************//**
********************************************************************************
**
** @file MpodMapper.h
** @author James Till Matta
** @date 09 June, 2016
** @brief
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** @details Simple header only class to allow a mapping between absolute channel
** number (ie channel 0 is the first active channel, channel 1 is the second
** active channel, etc) and the module/channel system in the mpod
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHID_SRC_UTILITY_MPODMAPPER_H
#define ORCHID_SRC_UTILITY_MPODMAPPER_H

// includes for C system headers
// includes for C++ system headers
#include<vector>
// includes from other libraries
// includes from ORCHID
#include"InputLib/Blocks/MpodChannelData.h"

namespace Utility
{

class MpodMapper
{
public:
    MpodMapper(){}
    ~MpodMapper(){}
    
    void loadFromData(InputParser::MpodChannelData* chanData)
    {
        for(int i=0; i<chanData->board.size(); ++i)
        {
            if(chanData->online[i])
            {
                moduleNum.push_back(chanData->board[i]);
                channelNum.push_back(chanData->channel[i]);
            }
        }
    }
    
    std::vector<int> moduleNum;
    std::vector<int> channelNum;
};
}
#endif //ORCHID_SRC_UTILITY_MPODMAPPER_H
