/***************************************************************************//**
********************************************************************************
**
** @file byteutil.h
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
** @details Holds definitions of functions for byte swapping etc
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHID_SRC_UTILITY_SPE_BYTEUTIL_H
#define ORCHID_SRC_UTILITY_SPE_BYTEUTIL_H
// includes for C system headers
// includes for C++ system headers
// includes from other libraries
// includes from ORCHID
#include"globaldefs.h"

namespace Utility
{

namespace RdwrSpe
{

//takes an array as an input and swaps it in 4 byte chunks
void swapFourByte(int32* array, int numChunks=1);

//takes an array as an input and swaps it in 2 byte chunks
void swapTwoByte(int16* array, int numChunks=1);

inline void swapInt16(int16 &num)
{
	asm("ror $8, %1;" : "+r" ( num ) );
}

inline void swapInt32(int32 &num)
{
	asm("bswap %1"	: "+r" (num) );
}

inline void swapInt64(int64 &num) //buggy in gcc
{
	asm("bswap %1" : "+r" (num) );
}

inline void swapUint16(uint16 &num)
{
	asm("ror $8, %1;" : "+r" (num) );
}

inline void swapUint32(uint32 &num)
{
	asm("bswap %1"	: "+r" (num) );
}

inline void swapUint64(uint64 &num)//buggy in gcc
{
	asm("bswap %1" : "+r" (num) );
}

}
}

#endif // ORCHID_SRC_UTILITY_SPE_BYTEUTIL_H
