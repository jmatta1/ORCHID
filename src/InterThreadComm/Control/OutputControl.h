/***************************************************************************//**
********************************************************************************
**
** @file RunData.h
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
** @details Holds the definition of the class that is used to send run
** title and number to the processing thread classes
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHID_SRC_FILE_RUNDATA_H
#define ORCHID_SRC_FILE_RUNDATA_H

// includes for C system headers
// includes for C++ system headers
#include<string>
#include<atomic>
// includes from other libraries
#include<boost/thread.hpp>
// includes from ORCHID

namespace InterThread
{

class OutputControl
{
public:
    OutputControl();
    ~OutputControl(){}
    //setters
    void setRunTitle(const std::string& rTitle);
    void setRunNumber(int rn){runNumber.store(rn); runNumberTest.store(true);}
    void incrementRunNumber(){++runNumber; runNumberTest.store(true);}
    void increaseRunNumber(int diff){runNumber.fetch_add(diff); runNumberTest.store(true);}
    
    //getters
    int getRunNumber(){runNumberTest.store(false); return runNumber.load();}
    void getRunTitle(std::string& rTitle);
    //tester functions
    //why repeatedly lock and unlock mutexes when we can simply check
    //an atomic boolean and see if it is true or not
    //these should not change often so testing the boolean is effecient
    bool runTitleChangeSinceLastGet(){return runTitleTest.load();}
    bool runNumberChangeSinceLastGet(){return runNumberTest.load();}
    
private:
    //access guard for strings since they are not atomic
    boost::shared_mutex nonAtomicAccess;
    //string data
    std::string runTitle;
    //atomic integers
    std::atomic_int runNumber;
    
    //atomic boolean test
    std::atomic_bool runTitleTest;
    std::atomic_bool runNumberTest;
};

}
#endif //ORCHID_SRC_FILE_RUNDATA_H
