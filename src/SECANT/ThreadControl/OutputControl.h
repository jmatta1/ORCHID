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

namespace Secant
{

namespace InterThreadCtrl
{

class OutputControl
{
public:
    OutputControl():runTitle(""), runNumber(0){}
    ~OutputControl(){}
    //setters
    void setRunTitle(const std::string& rTitle);
    void setRunNumber(int rn){runNumber.store(rn);}
    
    //getters
    int getRunNumber(){return runNumber.load();}
    void getRunTitle(std::string& rTitle);
    
private:
    //access guard for strings since they are not atomic
    boost::shared_mutex nonAtomicAccess;
    //string data
    std::string runTitle;
    //atomic integers
    std::atomic_int runNumber;
};

}
}
#endif //ORCHID_SRC_FILE_RUNDATA_H
