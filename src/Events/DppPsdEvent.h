/***************************************************************************//**
********************************************************************************
**
** @file DppPsdEvent.h
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
#ifndef ORCHID_SRC_EVENTS_DPPPSDEVENT_H
#define ORCHID_SRC_EVENTS_DPPPSDEVENT_H
// includes for C system headers
// includes for C++ system headers
// includes from other libraries
// includes from ORCHID
#include"EventInterface.h"

namespace Events
{

class DppPsdEvent : public EventInterface
{
public:
    DppPsdEvent():boardNumber(0), channelNumber(0), longIntegral(0),
        shortIntegral(0), extendedTimestamp(0), fineTimeStamp(0),
        baseline(0), flags(0) {}
    virtual ~DppPsdEvent(){}
    
    //functions to retrieve the binary representation
    virtual int getSizeOfBinaryRepresentation();
    //the buffer provided to this function will be at *least* the size
    //returned by getSizeOfBinaryRepresentation
    virtual void getBinaryRepresentation(char* buff);
    
    //setters for the event processing threads to dump data into these
    void setBoard(unsigned short board){boardNumber = board;}
    void setChannel(unsigned short chan){channelNumber = chan;}
    void setLongGate(unsigned short lGate){longIntegral = lGate;} 
    void setShortGate(unsigned short sGate){shortIntegral = sGate;}
    void setTimeStamp(unsigned long long tStamp){extendedTimestamp = tStamp;}
    void setFineTimeStamp(unsigned short ftStamp){fineTimeStamp = ftStamp;}
    void setBaseline(unsigned short bsl){baseline = bsl;}
    void setFlags(unsigned short flg){flags = flg;}
    
private:
    unsigned short boardNumber;
    unsigned short channelNumber;
    unsigned long long extendedTimestamp;
    unsigned short longIntegral;
    unsigned short shortIntegral;
    unsigned short fineTimeStamp;
    unsigned short baseline;//could be zero based on extras word options
    unsigned short flags;//bit[0] = PUR flag, bit[1] = over-range, bit[2] = trigger lost
};


}

#endif  //ORCHID_SRC_EVENTS_DPPPSDEVENT_H
