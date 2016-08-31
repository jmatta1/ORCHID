#include"byteutil.h"

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

