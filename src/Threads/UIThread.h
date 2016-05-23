/***************************************************************************//**
********************************************************************************
**
** @file UIThread.h
** @author James Till Matta
** @date 27 Apr, 2016
** @brief
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** @details Holds the definition of the class that is run by main to handle the
** user interface
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHID_SRC_THREADS_UITHREAD_H
#define ORCHID_SRC_THREADS_UITHREAD_H

// includes for C system headers
// includes for C++ system headers
// includes from other libraries
// includes from ORCHID

namespace Threads
{
class UIThread
{
public:
    UIThread(){}
    ~UIThread(){}

    //this is the function that is called by boost::thread when making a thread
    void operator() ();
private:

};

}
#endif //ORCHID_SRC_THREADS_UITHREAD_H
