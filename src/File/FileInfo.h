/***************************************************************************//**
********************************************************************************
**
** @file FileInfo.h
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
** @details Holds the definition of the class that is used to show file data to
** the UI thread
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHID_SRC_FILE_FILEINFO_H
#define ORCHID_SRC_FILE_FILEINFO_H

// includes for C system headers
// includes for C++ system headers
#include<string>
#include<atomic>
// includes from other libraries
#include<boost/thread.hpp>
// includes from ORCHID

namespace FileOutput
{

struct FileInfo
{
    FileInfo();
    ~FileInfo(){}
    //setters
    void setFileNameAndRunTitle(const std::string& fName, const std::string& rTitle);
    void setFileName(const std::string& fName);
    void setRunTitle(const std::string& rTitle);
    void setRunNumber(int rn){runNumber.store(rn); runNumberTest.store(true);}
    void setSequenceNumber(int sn){sequenceNumber.store(sn); sequenceNumberTest.store(true);}
    void setSize(int s){size.store(sn);}
    void incrementRunNumber(){++runNumber; sequenceNumberTest.store(true);}
    void incrementSequenceNumber(){++sequenceNumber; sequenceNumberTest.store(true);}
    void increaseRunNumber(int diff){runNumber.fetch_add(diff); runNumberTest.store(true);}
    void increaseSequenceNumber(int diff){sequenceNumber.fetch_add(diff); sequenceNumberTest.store(true);}
    void increaseSize(int diff){size.fetch_add(diff);}
    //getters
    int getRunNumber(){return runNumber.load(); runNumberTest.store(false);}
    int getSequencNumber(){return sequenceNumber.load(); sequenceNumberTest.store(false);}
    int getSize(){return size.load();}
    void getFileName(std::string& fName);
    void getRunTitle(std::string& rTitle);
    void getFileNameAndRunTitle(std::string& fName, std::string& rTitle);
    //tester functions
    //why repeatedly lock and unlock mutexes when we can simply check
    //an atomic boolean and see if it is true or not
    //these should not change often so testing the boolean is effecient
    bool fileNameChangeSinceLastGet(){return fileNameTest.load();}
    bool runTitleChangeSinceLastGet(){return runTitleTest.load();}
    bool runNumberChangeSinceLastGet(){return runNumberTest.load();}
    bool sequenceNumberChangeSinceLastGet(){return sequenceNumberTest.load();}
    
private:
    //access guard for strings since they are not atomic
    boost::shared_mutex nonAtomicAccess;
    //string data
    std::string fileName;
    std::string runTitle;
    //atomic integers
    std::atomic_int runNumber;
    std::atomic_int sequenceNumber;
    std::atomic_int size;
    
    //atomic boolean test
    std::atomic_bool fileNameTest;
    std::atomic_bool runTitleTest;
    std::atomic_bool runNumberTest;
    std::atomic_bool sequenceNumberTest;
};

}
#endif //ORCHID_SRC_FILE_FILEINFO_H
