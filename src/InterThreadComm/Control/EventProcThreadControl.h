/***************************************************************************//**
********************************************************************************
**
** @file ProcessingThreadControl.h
** @author James Till Matta
** @date 27 June, 2016
** @brief
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** @details Definition file for the ProcessingThreadControl class
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHID_SRC_INTERTHREADCOMM_CONTROL_PROCESSINGTHREADCONTROL_H
#define ORCHID_SRC_INTERTHREADCOMM_CONTROL_PROCESSINGTHREADCONTROL_H
// includes for C system headers
// includes for C++ system headers
#include<atomic>
// includes from other libraries
#include<boost/thread.hpp>
// includes from ORCHID

namespace InterThread
{

enum class ProcessingThreadState : char {Running, Stopped, Terminate};

class ProcessingThreadControl
{
    
};

}
#endif //ORCHID_SRC_INTERTHREADCOMM_CONTROL_PROCESSINGTHREADCONTROL_H
