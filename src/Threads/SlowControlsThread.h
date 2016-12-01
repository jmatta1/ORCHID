/***************************************************************************//**
********************************************************************************
**
** @file SlowControlsThread.h
** @author James Till Matta
** @date 01 June, 2016
** @brief
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** @details Holds the definition of the slow controls thread class used to poll
** the slow controls like voltage and temperature
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHID_SRC_THREADS_SLOWCONTROLSTHREAD_H
#define ORCHID_SRC_THREADS_SLOWCONTROLSTHREAD_H

// includes for C system headers
// includes for C++ system headers
// includes from other libraries
#include<boost/chrono.hpp>
// includes from ORCHID
#include"IO/SecantFileWriter.h"
#include"Hardware/HVLib/MpodReader.h"
#include"InterThreadComm/Data/SlowData.h"
#include"InterThreadComm/Control/SlowControlsThreadController.h"
#include"Utility/OrchidLogger.h"
#include"Events/SlowControlsEvent.h"
#include"InterThreadComm/Data/RunData.h"

namespace Threads
{

typedef boost::log::sources::severity_logger_mt<LogSeverity> LoggerType;

class SlowControlsThread
{
public:
    //construction and destruction
    SlowControlsThread(SlowControls::MpodReader* mRead, InterThread::SlowData* slDat,
                       InterThread::SlowControlsThreadController* sctCtrl,
                       int refreshRate, InterThread::RunData* runDat, 
                       InterThread::FileData* fileDat, int thrdNum,
                       const std::string& baseOutputDirectory, LoggerType& log);
    ~SlowControlsThread(){delete[] eventBuffer;}
    
    // the function that makes this object callable which actually executes the thread
    void operator() ();
private:
    inline void publishInternalData()
    {
        slowData->readVoltageData(this->mpodReader->voltageData);
    }

    inline void writeScEventToFile()
    {
        //put the data into the event
        scEvent.ReadVoltageData(this->mpodReader->voltageData);
        //put the event into a write buffer
        scEvent.getBinaryRepresentation(this->eventBuffer);
        //put the event buffer into the file
        outputFile.writeEvent(this->eventBuffer, this->eventSize);
    }
    
    void doWritingLoop();
    void doPollingLoop();
    
    SlowControls::MpodReader* mpodReader;
    InterThread::SlowData* slowData;
    InterThread::SlowControlsThreadController* sctControl;
    boost::chrono::nanoseconds refreshPeriod;
    
    IO::SecantFileWriter outFile;
    
    bool notTerminated;
    
    //stuff for writing to file
    Events::SlowControlsEvent scEvent;
    char* eventBuffer;
    int eventSize;
    InterThread::RunData* runData;
    int runNumber;
    std::string runTitle;
    
    //the actual output file
    IO::SecantFileWriter outputFile;
    
    //logger
    LoggerType& lg;
};

}

#endif //ORCHID_SRC_THREADS_SLOWCONTROLSTHREAD_H
