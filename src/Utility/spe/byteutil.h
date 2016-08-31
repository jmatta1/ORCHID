#ifndef BYTEUTIL_H
#define BYTEUTIL_H
#include"globaldefs.h"

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

#endif // BYTEUTIL_H
