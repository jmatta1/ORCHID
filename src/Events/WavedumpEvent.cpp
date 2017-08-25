/***************************************************************************//**
********************************************************************************
**
** @file WavedumpEvent.cpp
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
** @details Holds definition of the wavedump event extension of an event interface
**
********************************************************************************
*******************************************************************************/
#include"WavedumpEvent.h"
// includes for C system headers
// includes for C++ system headers
// includes from other libraries
// includes from ORCHID
#include"EventCodes.h"

namespace Events
{


int WavedumpEvent::getSizeOfBinaryRepresentation()
{
    return binarySize;
}

void WavedumpEvent::getBinaryRepresentation(char* buff)
{
    int index = 0;
    *(reinterpret_cast<unsigned short*>(&(buff[index])))    = this->binarySize;
    index += sizeof(unsigned short);
    *(reinterpret_cast<unsigned short*>(&(buff[index])))    = Codes::DigitizerWavedumpCode;
    index += sizeof(unsigned short);
    *(reinterpret_cast<unsigned char*>(&(buff[index])))     = boardNumber;
    index += sizeof(unsigned char);
    *(reinterpret_cast<unsigned char*>(&(buff[index])))     = channelNumber;
    index += sizeof(unsigned char);
    *(reinterpret_cast<unsigned int*>(&(buff[index])))      = numSamples;
    index += sizeof(unsigned int);
    *(reinterpret_cast<unsigned int*>(&(buff[index])))      = timeStamp;
    index += sizeof(unsigned int);
    *(reinterpret_cast<unsigned short*>(&(buff[index])))    = extraTimeStamp;
    index += sizeof(unsigned short);
    *(reinterpret_cast<unsigned int*>(&(buff[index])))      = eventNumber;
    index += sizeof(unsigned int);
    for(int i=0; i<numSamples; ++i)
    {
        *(reinterpret_cast<unsigned short*>(&(buff[index])))= dataArray[2*i];
        index += sizeof(unsigned short);
    }
}

void WavedumpEvent::setDataArray(unsigned int* dArr)
{
    for(int i=0; i<numSamplePairs; ++i)
    {
        dataArray[2*i] = (dArr[i]&0xFFFFUL);
        dataArray[2*i+1] = ((dArr[i]&0xFFFF0000UL)>>16);
    }
}

}
