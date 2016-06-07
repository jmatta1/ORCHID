/***************************************************************************//**
********************************************************************************
**
** @file EventInterface.h
** @author James Till Matta
** @date 07 June, 2016
** @brief
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** @details Holds the defintion of the reentrant base class for events that are
** sent to the file output thread
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHID_SRC_EVENTS_EVENTINTERFACE_H
#define ORCHID_SRC_EVENTS_EVENTINTERFACE_H
// includes for C system headers
// includes for C++ system headers
// includes from other libraries
// includes from ORCHID

namespace Events
{

class EventInterface
{
public:
    virtual ~EventInterface(){}
    
    //functions to retrieve the binary representation
    virtual int getSizeOfBinaryRepresentation() = 0;
    //the buffer provided to this function will be at *least* the size
    //returned by getSizeOfBinaryRepresentation
    virtual void getBinaryRepresentation(char* buff) = 0;
    
    //functions to retrieve the ascii representation
    virtual int getSizeOfAsciiRepresentation() = 0;
    //the buffer provided to this function will be at *least* the size returned
    //by getSizeOfAsciiRepresentation
    virtual void getAsciiRepresentation() = 0;
};

}

#endif //ORCHID_SRC_EVENTS_EVENTINTERFACE_H
