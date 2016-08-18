/***************************************************************************//**
********************************************************************************
**
** @file RawBufferTriple.h
** @author James Till Matta
** @date 24 June, 2016
** @brief
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** @details Holds a little class to move data through the to processing queue
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHID_SRC_INTERTHREADCOMM_RAWBUFFER_TRIPLE_H
#define ORCHID_SRC_INTERTHREADCOMM_RAWBUFFER_TRIPLE_H

namespace InterThread
{

template<class BufferInfo>
struct RawBufferTriple
{
    int sizeOfData;
    int* dataBuffer;
    BufferInfo info;
};

}
#endif //ORCHID_SRC_INTERTHREADCOMM_RAWBUFFER_TRIPLE_H
