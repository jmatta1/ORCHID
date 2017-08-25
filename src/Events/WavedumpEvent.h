/***************************************************************************//**
********************************************************************************
**
** @file WavedumpEvent.h
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
#ifndef ORCHID_SRC_EVENTS_WAVEDUMPEVENT_H
#define ORCHID_SRC_EVENTS_WAVEDUMPEVENT_H
// includes for C system headers
// includes for C++ system headers
// includes from other libraries
// includes from ORCHID
#include"EventInterface.h"

namespace Events
{

class WavedumpEvent : public EventInterface
{
public:
    WavedumpEvent(int nSamples):
        //binary size + event code + extra timestamp + numSamples + timestamp + event number + board number +
        //eventNumber + data array
        binarySize((2+1)*sizeof(unsigned short)+3*sizeof(unsigned int)+2*sizeof(unsigned char)+nSamples*sizeof(unsigned short)),
        numSamples(nSamples), numSamplePairs(nSamples/2), boardNumber(0), channelNumber(0), timeStamp(0),
        extraTimeStamp(0), eventNumber(0), dataArray(new unsigned short[nSamples]){}
    virtual ~WavedumpEvent(){}
    
    //functions to retrieve the binary representation
    virtual int getSizeOfBinaryRepresentation();
    //the buffer provided to this function will be at *least* the size
    //returned by getSizeOfBinaryRepresentation
    virtual void getBinaryRepresentation(char* buff);
    
    //setters for the event processing threads to dump data into these
    void setBoard(unsigned char board){boardNumber = board;}
    void setChannel(unsigned char chan){channelNumber = chan;}
    void setTimeStamp(unsigned int tStamp){timeStamp = tStamp;}
    void setExtraTimeStamp(unsigned short exTStamp){extraTimeStamp = exTStamp;}
    void setEventNumber(unsigned int evNum){eventNumber = evNum;}
    void setDataArray(unsigned int* dArr);
    
private:
    unsigned short binarySize;
    unsigned int numSamples;
    unsigned int numSamplePairs;
    unsigned char boardNumber;
    unsigned char channelNumber;
    unsigned int timeStamp;
    unsigned short extraTimeStamp;
    unsigned int eventNumber;
    unsigned short* dataArray;
};


}

#endif  //ORCHID_SRC_EVENTS_WAVEDUMPEVENT_H
