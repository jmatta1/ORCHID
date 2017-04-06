/***************************************************************************//**
********************************************************************************
**
** @file FileInfo.cpp
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
** @details Holds the implementation of the class that is used to show file
** data to the UI thread
**
********************************************************************************
*******************************************************************************/

#include"FileData.h"
// includes for C system headers
// includes for C++ system headers
// includes from other libraries
// includes from ORCHID

namespace Secant
{

namespace OperationData
{

FileData::FileData():fileName(""), sequenceNumber(0), size(0),
    fileNameTest(true), sequenceNumberTest(true){}

//setters
void FileData::setFileName(const std::string& fName)
{
    boost::upgrade_lock<boost::shared_mutex> lock(nonAtomicAccess);
    boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock(lock);
    //now we have exclusive, writers, access
    fileName = fName;
    fileNameTest.store(true);
    //when the locks go out of scope they unlock
}

void FileData::getFileName(std::string& fName)
{
    boost::shared_lock<boost::shared_mutex> lock(nonAtomicAccess);
    fName = fileName;
    fileNameTest.store(false);
}

}
}
