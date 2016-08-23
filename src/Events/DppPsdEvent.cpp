/***************************************************************************//**
********************************************************************************
**
** @file DppPsdEvent.cpp
** @author James Till Matta
** @date 21 June, 2016
** @brief
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** @details Holds definition of the dpp psd event extension of an event interface
**
********************************************************************************
*******************************************************************************/
#include"DppPsdEvent.h"
// includes for C system headers
// includes for C++ system headers
// includes from other libraries
// includes from ORCHID
#include"EventCodes.h"

namespace Events
{


int DppPsdEvent::getSizeOfBinaryRepresentation()
{
    return binarySize;
}

unsigned short boardNumber;
unsigned short channelNumber;
unsigned long long extendedTimestamp;
unsigned short longIntegral;
unsigned short shortIntegral;
//could be zero based on extras word options
unsigned short fineTimeStamp;
unsigned short baseline;
unsigned short flags;//bit[0] = PUR flag, bit[1] = over-range, bit[2] = trigger lost

void DppPsdEvent::getBinaryRepresentation(char* buff)
{
    int index = 0;
    *(reinterpret_cast<int*>(&(buff[index]))) = this->binarySize;
    index += sizeof(int);
    *(reinterpret_cast<int*>(&(buff[index]))) = Codes::DigitizerPsdEventCode;
    index += sizeof(int);
    *(reinterpret_cast<unsigned short*>(&(buff[index]))) = boardNumber;
    index += sizeof(unsigned short);
    *(reinterpret_cast<unsigned short*>(&(buff[index]))) = channelNumber;
    index += sizeof(unsigned short);
    *(reinterpret_cast<unsigned long long*>(&(buff[index]))) = extendedTimestamp;
    index += sizeof(unsigned long long);
    *(reinterpret_cast<unsigned short*>(&(buff[index]))) = longIntegral;
    index += sizeof(unsigned short);
    *(reinterpret_cast<unsigned short*>(&(buff[index]))) = shortIntegral;
    index += sizeof(unsigned short);
    *(reinterpret_cast<unsigned short*>(&(buff[index]))) = fineTimeStamp;
    index += sizeof(unsigned short);
    *(reinterpret_cast<unsigned short*>(&(buff[index]))) = baseline;
    index += sizeof(unsigned short);
    *(reinterpret_cast<unsigned short*>(&(buff[index]))) = flags;
    index += sizeof(unsigned short);
}

}
