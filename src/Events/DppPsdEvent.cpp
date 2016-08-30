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

void DppPsdEvent::getBinaryRepresentation(char* buff)
{
    int index = 0;
    *(reinterpret_cast<unsigned char*>(&(buff[index])))     = this->binarySize;
    index += sizeof(unsigned char);
    *(reinterpret_cast<unsigned char*>(&(buff[index])))     = Codes::DigitizerPsdEventCode;
    index += sizeof(unsigned char);
    *(reinterpret_cast<unsigned char*>(&(buff[index])))     = boardNumber;
    index += sizeof(unsigned char);
    *(reinterpret_cast<unsigned char*>(&(buff[index])))     = channelNumber;
    index += sizeof(unsigned char);
    *(reinterpret_cast<unsigned int*>(&(buff[index])))      = timeStamp;
    index += sizeof(unsigned int);
    *(reinterpret_cast<unsigned short*>(&(buff[index])))    = extraTimeStamp;
    index += sizeof(unsigned short);
    *(reinterpret_cast<unsigned short*>(&(buff[index])))    = longIntegral;
    index += sizeof(unsigned short);
    *(reinterpret_cast<unsigned short*>(&(buff[index])))    = shortIntegral;
    index += sizeof(unsigned short);
    *(reinterpret_cast<unsigned char*>(&(buff[index])))     = flags;
    index += sizeof(unsigned char);
}

}
