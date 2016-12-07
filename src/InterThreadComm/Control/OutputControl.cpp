/***************************************************************************//**
********************************************************************************
**
** @file RunData.cpp
** @author James Till Matta
** @date 26 May, 2016
** @brief
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** @details Holds the implementation of the class that is used to send run
** title and number to the processing thread classes
**
********************************************************************************
*******************************************************************************/

#include"OutputControl.h"
// includes for C system headers
// includes for C++ system headers
// includes from other libraries
// includes from ORCHID

namespace InterThread
{

OutputControl::OutputControl():runTitle(""), runNumber(0), runTitleTest(true),
    runNumberTest(true)
{}

//setters
void OutputControl::setRunTitle(const std::string& rTitle)
{
    boost::upgrade_lock<boost::shared_mutex> lock(nonAtomicAccess);
    boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock(lock);
    //now we have exclusive, a.k.a. writers, access
    runTitle = rTitle;
    runTitleTest.store(true);
    //when the locks go out of scope they unlock
}


void OutputControl::getRunTitle(std::string& rTitle)
{
    boost::shared_lock<boost::shared_mutex> lock(nonAtomicAccess);
    rTitle = runTitle;
    runTitleTest.store(false);
}

}
