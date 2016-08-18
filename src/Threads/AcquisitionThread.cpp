/***************************************************************************//**
********************************************************************************
**
** @file DigitizerThread.cpp
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
** @details Implementation file for the DigitizerThread callable
**
********************************************************************************
*******************************************************************************/

#include"AcquisitionThread.h"
// includes for C system headers
// includes for C++ system headers
// includes from other libraries
// includes from ORCHID

namespace Threads
{

void AcquisitionThread::operator ()()
{
    //poll the state of the controller and loop if it is not terminate
    while(this->controller->getCurrentState() != InterThread::AcquisitionThreadState::Terminate)
    {
        
    }
}

}
