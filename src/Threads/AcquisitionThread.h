/***************************************************************************//**
********************************************************************************
**
** @file AcquisitionThread.h
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
** @details Definition file for the AcquisitionThread callable
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHID_SRC_THREADS_ACQUISITIONTHREAD_H
#define ORCHID_SRC_THREADS_ACQUISITIONTHREAD_H
// includes for C system headers
// includes for C++ system headers
// includes from other libraries
// includes from ORCHID
#include"Hardware/Digitizer/Vx1730Digitizer.h"
#include"InterThreadComm/Control/AcquisitionThreadControl.h"
#include"Utility/CommonTypeDefs.h"
#include"Utility/OrchidLogger.h"

namespace Threads
{

class AcquisitionThread
{
public:
    AcquisitionThread(Digitizer::Vx1730Digitizer* digi, InterThread::AcquisitionThreadControl* acCtrl,
                      Utility::ToProcessingQueuePair* procQueue):digitizer(digi),
        controller(), dataOutputQueue(procQueue), notTerminated(true),
        lg(OrchidLog::get()) { firstChannel = digi->getModuleStartChannel();
                               modNumber = digi->getModuleNumber();}
    ~AcquisitionThread(){}//delete nothing since we own nothing
    
    void operator()();
    
private:
    void doAcquisitionLoop();
    void startAcquisition();
    void endAcquisition();
    void doFinalRead();
    
    Digitizer::Vx1730Digitizer* digitizer;
    InterThread::AcquisitionThreadControl* controller;
    Utility::ToProcessingQueuePair* dataOutputQueue;
    
    Utility::ToProcessingBuffer* currentBuffer;
    int firstChannel;
    int modNumber;
    
    bool notTerminated;
    
    //logger
    boost::log::sources::severity_logger_mt<LogSeverity>& lg;
};

}

#endif //ORCHID_SRC_THREADS_ACQUISITIONTHREAD_H
