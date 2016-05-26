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

#include"FileInfo.h"
// includes for C system headers
// includes for C++ system headers
// includes from other libraries
// includes from ORCHID

namespace FileOutput
{

FileInfo::FileInfo():fileName(""), runTitle(""), runNumber(0),
    sequenceNumber(0), size(0),fileNameTest(true), runTitleTest(true),
    runNumberTest(true), sequenceNumberTest(true)
{}
//setters
void FileInfo::setFileNameAndRunTitle(const std::string& fName, const std::string& rTitle)
{
    boost::upgrade_lock<boost::shared_mutex> lock(nonAtomicAccess);
    boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock(lock);
    //now we have exclusive, writers, access
    fileName = fName;
    runTitle = rTitle;
    fileNameTest.store(true);
    runTitleTest.store(true);
    //when the locks go out of scope they unlock
}

void FileInfo::setFileName(const std::string& fName)
{
    boost::upgrade_lock<boost::shared_mutex> lock(nonAtomicAccess);
    boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock(lock);
    //now we have exclusive, writers, access
    fileName = fName;
    fileNameTest.store(true);
    //when the locks go out of scope they unlock
}

void FileInfo::setRunTitle(const std::string& rTitle)
{
    boost::upgrade_lock<boost::shared_mutex> lock(nonAtomicAccess);
    boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock(lock);
    //now we have exclusive, writers, access
    runTitle = rTitle;
    runTitleTest.store(true);
    //when the locks go out of scope they unlock
}

void FileInfo::getFileName(std::string& fName)
{
    boost::shared_lock<boost::shared_mutex> lock(nonAtomicAccess);
    fName = fileName;
    fileNameTest.store(false);
}

void FileInfo::getRunTitle(std::string& rTitle)
{
    boost::shared_lock<boost::shared_mutex> lock(nonAtomicAccess);
    rTitle = runTitle;
    runTitleTest.store(false);
}

void FileInfo::getFileNameAndRunTitle(std::string& fName, std::string& rTitle)
{
    boost::shared_lock<boost::shared_mutex> lock(nonAtomicAccess);
    fName = fileName;
    rTitle = runTitle;
    fileNameTest.store(false);
    runTitleTest.store(false);
}

}
