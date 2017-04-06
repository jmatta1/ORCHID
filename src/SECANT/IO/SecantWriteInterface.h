/***************************************************************************//**
********************************************************************************
**
** @file SecantFileWriter.h
** @author James Till Matta
** @date 06 April, 2017
** @brief
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** @details Holds the definition of a very simple class that is passed to
** code from users of SECANT. In this fashion they can directly access the
** writing capabilities of the underlying SecantFileWriter, but cannot do things
** like change the run or close the file, etc. i.e. this allows me to let them
** write directly to the file without some sort of weird buffer passing, while
** simultaneously preventing access to functions that really should only be used
** by the processing threads
**
********************************************************************************
*******************************************************************************/
#ifndef SECANT_SRC_IO_SECANTFILEINTERFACE_H
#define SECANT_SRC_IO_SECANTFILEINTERFACE_H

// includes for C system headers
// includes for C++ system headers
// includes from other libraries
// includes from ORCHID
#include"SECANT/IO/SecantFileWriter.h"
namespace Secant
{

namespace IO
{

class SecantWriteInterface
{
public:
    SecantWriteInterface(SecantFileWriter& writer):fileWriter(writer){}
    ~SecantWriteInterface(){}
    //used by the outputting thread to write events to the file
    inline void writeEvent(char* event, int size){fileWriter.writeEvent(event, size);}

private:
    SecantFileWriter& fileWriter;
};

}
}
#endif //SECANT_SRC_IO_SECANTFILEINTERFACE_H
