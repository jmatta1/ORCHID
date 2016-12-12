/***************************************************************************//**
********************************************************************************
**
** @file OverallControl.h
** @author James Till Matta
** @date 1 Dec, 2016
** @brief
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** @details Holds the definition for the OverallControl class which handles
**    coordinating all the thread controls
**
********************************************************************************
*******************************************************************************/
//includes from C libraries
//includes from C++ libraries
#include<string>
#include<array>
//includes from other libraries
//includes from ORCHID
//various controls from ORCHID
#include"SlowControlsThreadControl.h"
#include"AcquisitionThreadControl.h"
#include"ProcessingThreadControl.h"
#include"OutputControl.h"
//the queues between acquisition threads and processing threads
#include"InterThreadComm/QueuePair.h"
//a couple type defs for sanities sake
#include"Utility/CommonTypeDefs.h"

namespace InterThread
{

//a minor class that allows me to put a member function in a
template <typename CtrlClass, typename RetType>
class ValueChecker
{
public:
    ValueChecker(CtrlClass* ins, RetType (CtrlClass::* func)):instance(ins), function(func){}
    RetType operator()() const {return (instance->*function)();}
private:
    CtrlClass* instance;
    RetType (CtrlClass::* function)();
};

template<int NumProdConsGroups, int NumBuffersPerGroup>
class OverallControl
{
public:
    using InterThreadQueue = Utility::ToProcessingQueuePair<NumBuffersPerGroup>;
    OverallControl(Utility::LoggerType& log, int numAcqThrd, int numPrcThrd);
    ~OverallControl();
    
    //retrieves individual controllers for the threads that need them
    OutputControl* getFileDataCtrl(){return outputCtrl;}
    SlowControlsThreadControl* getSlowControlsCtrl(){return slowControlsCtrl;}
    ProcessingThreadControl* getProcessingThreadCtrl(){return processingCtrl;}
    AcquisitionThreadControl* getAcquisitionThreadCtrl(){return acquisitionCtrl;}
    
    //set and retrive the queues in the array below
    void setInterthreadQueuePtr(InterThreadQueue* ptr, int index){dataQueues[index] = ptr;}
    InterThreadQueue* getInterthreadQueuePtr(int index){return dataQueues[index];}
    
    //get the number of interthread queues
    constexpr int getNumInterthreadQueues(){return NumProdConsGroups;}
    constexpr int getNumBuffersPerGroup(){return NumBuffersPerGroup;}
    
    
    //sends terminate and waits for response to groups of threads in the following order:
    //first turn off the production of data
    //1) acqusition threads
    //2) slow controls thread - does production and writing
    //then shutdown the processing and writing of data
    //3) processing threads
    void terminate();
    //puts everyone into stop condition first acquisition threads, then slow ctrls threads,
    //then processing threads
    void idleEverything();
    //first calls idle everything to put it all in stop mode. It then puts the
    //slow controlls thread in polling mode
    void pollSlowControls();
    //sets the file data using the given runTitle and runNum, then puts the
    //slow controls thread into writing mode, puts the processing threads into
    //active mode, and finally puts the acquisition threads into acquiring mode
    //this is a dumb function, it just sets things to go, SECANT files should not
    //overwrite previously existing data, but they avoid this by incrementing the
    //sequence number, the user needs to check for runtitle and run number collisions
    //elsewhere
    void startRun(const std::string& runTitle, int runNum);
private:
    //internal functions that encapsulate the logic needed for certain operations
    //regarding controlling threads
    void terminateAcquisition();
    void terminateSlowControls();
    void terminateProcessing();
    
    void stopAcquisition();
    void stopSlowControls();
    void stopProcessing();
    
    void setSlowControlsToPoll();
    
    void startProcessing();
    void startSlowControls();
    void startAcquisition();
    
    void forceQueuesAwake();
    void clearQueuesWake();
    
    //convenience class to encapsulate my loop and recheck value idiom
    template<typename CtrlClass, typename RetType>
    void waitForValue(ValueChecker<CtrlClass, RetType>& valCheck, const RetType& targetValue, int waitTimeInMicroseconds) const;
    
    inline void logIndivModStart(const char* thrType, const char* setState)
    {
        BOOST_LOG_SEV(this->lg, Information) << "Overall Control: Setting SECANT "<<thrType<<" to "<<setState;
    }
    
    inline void logIndivModStop(const char* thrType, const char* setState)
    {
        BOOST_LOG_SEV(this->lg, Information) << "Overall Control: Done setting SECANT "<<thrType<<" to "<<setState;
    }
    
    AcquisitionThreadControl* acquisitionCtrl;
    ProcessingThreadControl* processingCtrl;
    SlowControlsThreadControl* slowControlsCtrl;
    OutputControl* outputCtrl;
    
    //to track when all threads have acknowledged a command
    int numAcquisitionThreads;
    int numProcessingThreads;
    
    //logger for logging various doings
    Utility::LoggerType& lg;
    
    //Array of acquisition to processing queues
    std::array<InterThreadQueue*, NumProdConsGroups> dataQueues;
};

/*******************************************************************************
** Public Member Functions
*******************************************************************************/

template<int NumProdConsGroups, int NumBuffersPerGroup>
OverallControl<NumProdConsGroups, NumBuffersPerGroup>::OverallControl(Utility::LoggerType& log, int numAcqThrd, int numPrcThrd):
    numAcquisitionThreads(numAcqThrd), numProcessingThreads(numPrcThrd), lg(log)
{
    //make the only control class whose threads do not also depend on output
    //control, the control for the acquisition thread, and set it to stopped
    this->acquisitionCtrl = new AcquisitionThreadControl;
    this->acquisitionCtrl->setToStopped();
    //make the output controller
    this->outputCtrl = new OutputControl();
    //make the controls for threads that depend on output control as well
    //make the processing thread control and set it to stopped
    this->processingCtrl = new ProcessingThreadControl;
    this->processingCtrl->setToStopped();
    //make the slow controls thread controls and set it to stopped
    this->slowControlsCtrl = new SlowControlsThreadControl;
    this->slowControlsCtrl->setToStop();
}

template<int NumProdConsGroups, int NumBuffersPerGroup>
OverallControl<NumProdConsGroups, NumBuffersPerGroup>::~OverallControl()
{
    delete slowControlsCtrl;
    delete processingCtrl;
    delete outputCtrl;
    delete acquisitionCtrl;
}

template<int NumProdConsGroups, int NumBuffersPerGroup>
void OverallControl<NumProdConsGroups, NumBuffersPerGroup>::terminate()
{
    BOOST_LOG_SEV(this->lg, Information) << "Overall Control: Setting all SECANT threads to terminate";
    this->terminateAcquisition();
    this->terminateSlowControls();
    this->terminateProcessing();
    BOOST_LOG_SEV(this->lg, Information) << "Overall Control: Done setting all SECANT threads to terminate";
}

template<int NumProdConsGroups, int NumBuffersPerGroup>
void OverallControl<NumProdConsGroups, NumBuffersPerGroup>::idleEverything()
{
    BOOST_LOG_SEV(this->lg, Information) << "Overall Control: Setting all SECANT threads to idle";
    this->stopAcquisition();
    this->stopSlowControls();
    this->stopProcessing();
    BOOST_LOG_SEV(this->lg, Information) << "Overall Control: Done setting all SECANT threads to idle";
}

template<int NumProdConsGroups, int NumBuffersPerGroup>
void OverallControl<NumProdConsGroups, NumBuffersPerGroup>::pollSlowControls()
{
    BOOST_LOG_SEV(this->lg, Information) << "Overall Control: Setting all SECANT threads to poll Slow Controls";
    this->stopAcquisition();
    this->setSlowControlsToPoll();
    this->stopProcessing();
    BOOST_LOG_SEV(this->lg, Information) << "Overall Control: Done setting all SECANT threads to poll Slow Controls";
}

template<int NumProdConsGroups, int NumBuffersPerGroup>
void OverallControl<NumProdConsGroups, NumBuffersPerGroup>::startRun(const std::string& runTitle, int runNum)
{
    BOOST_LOG_SEV(this->lg, Information) << "Overall Control: Passing run title and number to output control";
    this->outputCtrl->setRunTitle(runTitle);
    this->outputCtrl->setRunNumber(runNum);
    BOOST_LOG_SEV(this->lg, Information) << "Overall Control: Setting all SECANT threads to running/writing";
    this->startProcessing();//start the processing threads
    this->startSlowControls();//start the 
    this->startAcquisition();
    BOOST_LOG_SEV(this->lg, Information) << "Overall Control: Done setting all SECANT threads to running/writing";
}

/*******************************************************************************
** Private Member Functions
*******************************************************************************/
//private set to running functions
template<int NumProdConsGroups, int NumBuffersPerGroup>
void OverallControl<NumProdConsGroups, NumBuffersPerGroup>::startAcquisition()
{
    this->logIndivModStart("Acquisition Threads", "Running");
    //set acquisition to terminate
    this->acquisitionCtrl->setToAcquiring();
    //do a pause and check in this thread to wait until all acquisition threads signal they are done
    ValueChecker<AcquisitionThreadControl, int> checker(acquisitionCtrl, &(AcquisitionThreadControl::getThreadsWaiting));
    this->waitForValue(checker, 0, 100);
    this->logIndivModStop("Acquisition Threads", "Running");
}

template<int NumProdConsGroups, int NumBuffersPerGroup>
void OverallControl<NumProdConsGroups, NumBuffersPerGroup>::startSlowControls()
{
    this->logIndivModStart("SlowControls Thread", "Writing");
    //set acquisition to terminate
    this->slowControlsCtrl->setToWriting();
    
    //do a pause and check in this thread to wait until the slow controls thread acknowledges shutdown
    ValueChecker<SlowControlsThreadControl, bool> checker(slowControlsCtrl, &(SlowControlsThreadControl::isSleeping));
    this->waitForValue(checker, true, 50);
    this->logIndivModStop("SlowControls Thread", "Writing");
}

template<int NumProdConsGroups, int NumBuffersPerGroup>
void OverallControl<NumProdConsGroups, NumBuffersPerGroup>::startProcessing()
{
    this->logIndivModStart("Processing Threads", "Running");
    //set processing to run
    this->processingCtrl->setToRunning();
    
    //do a pause and check in this thread to wait until all acquisition threads signal they are done
    ValueChecker<ProcessingThreadControl, int> checker(processingCtrl, &(ProcessingThreadControl::getThreadsWaiting));
    this->waitForValue(checker, 0, 100);
    
    this->logIndivModStop("Processing Threads", "Running");
}

//the private set sc to polling function
template<int NumProdConsGroups, int NumBuffersPerGroup>
void OverallControl<NumProdConsGroups, NumBuffersPerGroup>::setSlowControlsToPoll()
{
    this->logIndivModStart("SlowControls Thread", "polling");
    //set acquisition to poll
    this->slowControlsCtrl->setToPolling();
    
    //do a pause and check in this thread to wait until the slow controls thread acknowledges shutdown
    ValueChecker<SlowControlsThreadControl, bool> checker(slowControlsCtrl, &(SlowControlsThreadControl::isSleeping));
    this->waitForValue(checker, true, 50);
    this->logIndivModStop("SlowControls Thread", "polling");
}

//the private set to idle functions
template<int NumProdConsGroups, int NumBuffersPerGroup>
void OverallControl<NumProdConsGroups, NumBuffersPerGroup>::stopAcquisition()
{
    this->logIndivModStart("Acquisition Threads", "stop");
    //set acquisition to stop
    this->acquisitionCtrl->setToStopped();
    this->forceQueuesAwake();
    
    //do a pause and check in this thread to wait until all acquisition threads signal they are done
    ValueChecker<AcquisitionThreadControl, int> checker(acquisitionCtrl, &(AcquisitionThreadControl::getThreadsWaiting));
    this->waitForValue(checker, this->numAcqThreads, 100);
    
    this->clearQueuesWake();
    this->logIndivModStop("Acquisition Threads", "stop");
}

template<int NumProdConsGroups, int NumBuffersPerGroup>
void OverallControl<NumProdConsGroups, NumBuffersPerGroup>::stopSlowControls()
{
    this->logIndivModStart("SlowControls Thread", "stop");
    //set acquisition to stop
    this->slowControlsCtrl->setToStop();
    
    //do a pause and check in this thread to wait until the slow controls thread acknowledges shutdown
    ValueChecker<SlowControlsThreadControl, bool> checker(slowControlsCtrl, &(SlowControlsThreadControl::isWaiting));
    this->waitForValue(checker, true, 100);
    this->logIndivModStop("SlowControls Thread", "stop");
}

template<int NumProdConsGroups, int NumBuffersPerGroup>
void OverallControl<NumProdConsGroups, NumBuffersPerGroup>::stopProcessing()
{
    this->logIndivModStart("Processing Threads", "stop");
    //set processing to stop
    this->processingCtrl->setToStopped();
    //force any acquisition threads waiting on the availability of an outgoing
    //queue to spin until they get rid of their data
    this->forceQueuesAwake();
    
    //do a pause and check in this thread to wait until all acquisition threads signal they are done
    ValueChecker<ProcessingThreadControl, int> checker(processingCtrl, &(ProcessingThreadControl::getThreadsWaiting));
    this->waitForValue(checker, this->numProcessingThreads, 100);
    
    //clear the force wake condition from the queues
    this->clearQueuesWake();
    this->logIndivModStop("Processing Threads", "stop");
}

//the private terminate functions
template<int NumProdConsGroups, int NumBuffersPerGroup>
void OverallControl<NumProdConsGroups, NumBuffersPerGroup>::terminateAcquisition()
{
    this->logIndivModStart("Acquisition Threads", "terminate");
    //set acquisition to terminate
    this->acquisitionCtrl->setToTerminate();
    //force any acquisition threads waiting on the availability of an outgoing
    //queue to spin until they get rid of their data
    this->forceQueuesAwake();
    
    //do a pause and check in this thread to wait until all acquisition threads signal they are done
    ValueChecker<AcquisitionThreadControl, int> checker(acquisitionCtrl, &(AcquisitionThreadControl::getThreadsTerminated));
    this->waitForValue(checker, this->numAcqThreads, 100);
    
    //clear the force wake condition from the queues
    this->clearQueuesWake();
    this->logIndivModStop("Acquisition Threads", "terminate");
}

template<int NumProdConsGroups, int NumBuffersPerGroup>
void OverallControl<NumProdConsGroups, NumBuffersPerGroup>::terminateSlowControls()
{
    this->logIndivModStart("SlowControls Thread", "terminate");
    //set acquisition to terminate
    this->slowControlsCtrl->setToTerminate();
    
    //do a pause and check in this thread to wait until the slow controls thread acknowledges shutdown
    ValueChecker<SlowControlsThreadControl, bool> checker(slowControlsCtrl, &(SlowControlsThreadControl::isDone));
    this->waitForValue(checker, true, 100);
    this->logIndivModStop("SlowControls Thread", "terminate");
}

template<int NumProdConsGroups, int NumBuffersPerGroup>
void OverallControl<NumProdConsGroups, NumBuffersPerGroup>::terminateProcessing()
{
    this->logIndivModStart("Processing Threads", "terminate");
    //set processing to terminate
    this->processingCtrl->setToTerminate();
    //force any acquisition threads waiting on the availability of an outgoing
    //queue to spin until they get rid of their data
    this->forceQueuesAwake();
    
    //do a pause and check in this thread to wait until all acquisition threads signal they are done
    ValueChecker<ProcessingThreadControl, int> checker(processingCtrl, &(ProcessingThreadControl::getThreadsTerminated));
    this->waitForValue(checker, this->numProcessingThreads, 100);
    
    this->clearQueuesWake();
    this->logIndivModStop("Processing Threads", "terminate");
}

template<int NumProdConsGroups, int NumBuffersPerGroup>
void OverallControl<NumProdConsGroups, NumBuffersPerGroup>::forceQueuesAwake()
{
    //force any acquisition threads waiting on the availability of an outgoing
    //queue to spin until they get rid of their data
    for(auto&& queue: dataQueues)
    {
        queue->forceWakeAll();
    }
}

template<int NumProdConsGroups, int NumBuffersPerGroup>
void OverallControl<NumProdConsGroups, NumBuffersPerGroup>::clearQueuesWake()
{
    //clear the force wake condition from the queues
    for(auto&& queue: dataQueues)
    {
        queue->clearForce();
    }
}

template<int NumProdConsGroups, int NumBuffersPerGroup>
template<typename CtrlClass, typename RetType>
void OverallControl<NumProdConsGroups, NumBuffersPerGroup>::waitForValue<CtrlClass, RetType>(
        ValueChecker<CtrlClass, RetType>& valCheck, const RetType& targetValue, int waitTimeInMicroseconds)
{
    while( valCheck() != targetValue )
    {
        boost::this_thread::sleep_for(boost::chrono::microseconds(waitTimeInMicroseconds));
    }
}

}
