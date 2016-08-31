/***************************************************************************//**
********************************************************************************
**
** @file byteutil.cpp
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
** @details Holds implementations of functions for byte swapping etc
**
********************************************************************************
*******************************************************************************/

#include"byteutil.h"
// includes for C system headers
// includes for C++ system headers
// includes from other libraries
// includes from ORCHID

namespace Utility
{

namespace RdwrSpe
{

void swapFourByte(int32* array, int numChunks)
{
	for(int i=0; i<numChunks; ++i)
	{
		asm("bswapl %1"	: "+r" (array[i]) );
	}
}

void swapTwoByte(int16* array, int numChunks)
{
	for(int i= 0; i<numChunks; ++i)
	{
		asm("rorw $8, %1;" : "+r" (array[i]) );
	}
}

}
}
