/***************************************************************************//**
********************************************************************************
**
** @file ProcessingThread.h
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
** @details Definition file for the ProcessingThread callable
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHID_SRC_THREADS_PROCESSINGTHREAD_H
#define ORCHID_SRC_THREADS_PROCESSINGTHREAD_H
// includes for C system headers
// includes for C++ system headers
#include<string>
// includes from other libraries
// includes from ORCHID
#include"InterThreadComm/Control/ProcessingThreadControl.h"
#include"InterThreadComm/Data/AcquisitionData.h"
#include"InterThreadComm/Data/RunData.h"
#include"InterThreadComm/Data/FileData.h"
#include"IO/SecantFileWriter.h"
#include"Utility/CommonTypeDefs.h"
#include"Utility/OrchidLogger.h"

namespace Threads
{

struct EventBuffer
{
public:
    inline EventBuffer(int maxSize):size(maxSize), buff(new char[maxSize]){}
    inline ~EventBuffer(){delete[] buff;}
    
    inline void setEventSize(unsigned char sz){reinterpret_cast<unsigned char*>(&(buff[0]))[0] = sz;}
    inline void setEventID(unsigned char id){reinterpret_cast<unsigned char*>(&(buff[1]))[0] = id;}
    inline void setBoard(unsigned char board){reinterpret_cast<unsigned char*>(&(buff[2]))[0] = board;}
    inline void setChannel(unsigned char chan){reinterpret_cast<unsigned char*>(&(buff[3]))[0] = chan;}
    inline void setTimeStamp(unsigned int tStamp){reinterpret_cast<unsigned int*>(&(buff[4]))[0] = tStamp;}
    inline void setExtraTimeStamp(unsigned short exTStamp){reinterpret_cast<unsigned short*>(&(buff[8]))[0] = exTStamp;}
    inline void setLongGate(unsigned short lGate){reinterpret_cast<unsigned short*>(&(buff[10]))[0] = lGate;} 
    inline void setShortGate(unsigned short sGate){reinterpret_cast<unsigned short*>(&(buff[12]))[0] = sGate;}
    inline void setFlags(unsigned char flg){reinterpret_cast<unsigned char*>(&(buff[14]))[0] = flg;}
    inline char* getBuffer(){return buff;}
private:
    int size;
    char* buff;
};

class ProcessingThread
{
public:
    ProcessingThread(InterThread::ProcessingThreadControl* prCtrl,
                     Utility::ToProcessingQueuePair* procQueue,
                     InterThread::AcquisitionData* acqDat,
                     InterThread::RunData* runDat, InterThread::FileData* fileDat, 
                     int thrdNum, const std::string& baseOutputDirectory):
        controller(prCtrl), dataInputQueue(procQueue), acqData(acqDat), runData(runDat),
        notTerminated(true), threadNumber(thrdNum), evBuff(15), lg(OrchidLog::get()),
        outputFile(fileDat, OrchidLog::get(), thrdNum, baseOutputDirectory)
    {evBuff.setEventSize(15); evBuff.setEventID(1);}
    ~ProcessingThread(){}//delete nothing since we own nothing
    
    void operator()();
    
private:
    void doProcessingLoop();
    void emptyProcessingBuffer();
    void processDataBuffer(Utility::ToProcessingBuffer* buffer);
    int processBoardAggregate(Utility::ToProcessingBuffer* buffer, int startOffset);
    int processChannelAggregate(Utility::ToProcessingBuffer* buffer, int startOffset, int baseChan);
    int processEventsWithExtras1(unsigned int* rawBuffer, int startOffset, int stopOffset, int baseChan, int boardNum, int skip);
    int processEventsWithExtras2(unsigned int* rawBuffer, int startOffset, int stopOffset, int baseChan, int boardNum, int skip);
    int processEventsWithExtras3(unsigned int* rawBuffer, int startOffset, int stopOffset, int baseChan, int boardNum, int skip);
    int processEventsWithoutExtras(unsigned int* rawBuffer, int startOffset, int stopOffset, int baseChan, int boardNum, int skip);
    
    
    InterThread::ProcessingThreadControl* controller;
    Utility::ToProcessingQueuePair* dataInputQueue;
    InterThread::AcquisitionData* acqData;
    InterThread::RunData* runData;
    
    bool notTerminated;
    int threadNumber;
    long long counters[16];
    
    //logger
    boost::log::sources::severity_logger_mt<LogSeverity>& lg;
    
    //a temporary buffer of character that holds the event to write to the file
    EventBuffer evBuff;
    
    //the actual output file
    IO::SecantFileWriter outputFile;
};

}

#endif //ORCHID_SRC_THREADS_PROCESSINGTHREAD_H
