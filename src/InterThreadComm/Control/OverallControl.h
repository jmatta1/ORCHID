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
    bool startAcquisition(const std::string& runTitle, int runNum);
private:
    //internal functions that encapsulate the logic needed for certain operations
    //regarding controlling threads
    void terminateAcquisition();
    void terminateSlowControls();
    void terminateProcessing();
    
    
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
    BOOST_LOG_SEV(this->lg, Information) << "Overall Control: Setting all SECANT threads to idle";
    this->stopAcquisition();
    this->setSlowControlsToPoll();
    this->stopProcessing();
    BOOST_LOG_SEV(this->lg, Information) << "Overall Control: Done setting all SECANT threads to idle";
}

//the private set sc to polling function
template<int NumProdConsGroups, int NumBuffersPerGroup>
void OverallControl<NumProdConsGroups, NumBuffersPerGroup>::setSlowControlsToPoll()
{
    BOOST_LOG_SEV(this->lg, Information) << "Overall Control: Setting SECANT slow controls thread to poll";
    //set acquisition to terminate
    this->slowControlsCtrl->setToPolling();
    
    //do a pause and check in this thread to wait until we see the slow controlls thread sleeping
    while(!(this->slowControlsCtrl->isSleeping()))
    {
        boost::this_thread::sleep_for(boost::chrono::microseconds(50));
    }
    BOOST_LOG_SEV(this->lg, Information) << "Overall Control: Done setting SECANT slow controls thread to poll";
}

//the private set to idle functions
template<int NumProdConsGroups, int NumBuffersPerGroup>
void OverallControl<NumProdConsGroups, NumBuffersPerGroup>::idleAcquisition()
{
    BOOST_LOG_SEV(this->lg, Information) << "Overall Control: Setting SECANT acquisition threads to stop";
    //set acquisition to terminate
    this->acquisitionCtrl->setToStopped();
    //force any acquisition threads waiting on the availability of an outgoing
    //queue to spin until they get rid of their data
    for(auto&& queue: dataQueues)
    {
        queue->forceWakeAll();
    }
    
    //do a pause and check in this thread to wait until all acquisition threads signal they are done
    while(!(this->acquisitionCtrl->getThreadsWaiting() == this->numAcqThreads))
    {
        boost::this_thread::sleep_for(boost::chrono::microseconds(100));
    }
    
    //clear the force wake condition from the queues
    for(auto&& queue: dataQueues)
    {
        queue->clearForce();
    }
    BOOST_LOG_SEV(this->lg, Information) << "Overall Control: Done setting SECANT acquisition threads to stop";
}

template<int NumProdConsGroups, int NumBuffersPerGroup>
void OverallControl<NumProdConsGroups, NumBuffersPerGroup>::stopSlowControls()
{
    BOOST_LOG_SEV(this->lg, Information) << "Overall Control: Setting SECANT slow controls thread to stop";
    //set acquisition to terminate
    this->slowControlsCtrl->setToStop();
    
    //do a pause and check in this thread to wait until the slow controls thread acknowledges shutdown
    while(!(this->slowControlsCtrl->isWaiting()))
    {
        boost::this_thread::sleep_for(boost::chrono::microseconds(100));
    }
    BOOST_LOG_SEV(this->lg, Information) << "Overall Control: Done setting SECANT slow controls thread to stop";
}

template<int NumProdConsGroups, int NumBuffersPerGroup>
void OverallControl<NumProdConsGroups, NumBuffersPerGroup>::stopProcessing()
{
    BOOST_LOG_SEV(this->lg, Information) << "Overall Control: Setting SECANT processing threads to stop";
    //set processing to terminate
    this->processingCtrl->setToStopped();
    //force any acquisition threads waiting on the availability of an outgoing
    //queue to spin until they get rid of their data
    for(auto&& queue: dataQueues)
    {
        queue->forceWakeAll();
    }
    
    //do a pause and check in this thread to wait until all acquisition threads signal they are done
    while(!(this->processingCtrl->getThreadsWaiting() == this->numProcessingThreads))
    {
        boost::this_thread::sleep_for(boost::chrono::microseconds(100));
    }
    
    //clear the force wake condition from the queues
    for(auto&& queue: dataQueues)
    {
        queue->clearForce();
    }
    BOOST_LOG_SEV(this->lg, Information) << "Overall Control: Done setting SECANT processing threads to stop";
}

//the private terminate functions
template<int NumProdConsGroups, int NumBuffersPerGroup>
void OverallControl<NumProdConsGroups, NumBuffersPerGroup>::terminateAcquisition()
{
    BOOST_LOG_SEV(this->lg, Information) << "Overall Control: Setting SECANT acquisition threads to terminate";
    //set acquisition to terminate
    this->acquisitionCtrl->setToTerminate();
    //force any acquisition threads waiting on the availability of an outgoing
    //queue to spin until they get rid of their data
    for(auto&& queue: dataQueues)
    {
        queue->forceWakeAll();
    }
    
    //do a pause and check in this thread to wait until all acquisition threads signal they are done
    while(!(this->acquisitionCtrl->getThreadsTerminated() == this->numAcqThreads))
    {
        boost::this_thread::sleep_for(boost::chrono::microseconds(100));
    }
    
    //clear the force wake condition from the queues
    for(auto&& queue: dataQueues)
    {
        queue->clearForce();
    }
    BOOST_LOG_SEV(this->lg, Information) << "Overall Control: Done setting SECANT acquisition threads to terminate";
}

template<int NumProdConsGroups, int NumBuffersPerGroup>
void OverallControl<NumProdConsGroups, NumBuffersPerGroup>::terminateSlowControls()
{
    BOOST_LOG_SEV(this->lg, Information) << "Overall Control: Setting SECANT slow controls thread to terminate";
    //set acquisition to terminate
    this->slowControlsCtrl->setToTerminate();
    
    //do a pause and check in this thread to wait until the slow controls thread acknowledges shutdown
    while(!(this->slowControlsCtrl->isDone()))
    {
        boost::this_thread::sleep_for(boost::chrono::microseconds(100));
    }
    BOOST_LOG_SEV(this->lg, Information) << "Overall Control: Done setting SECANT slow controls thread to terminate";
}

template<int NumProdConsGroups, int NumBuffersPerGroup>
void OverallControl<NumProdConsGroups, NumBuffersPerGroup>::terminateProcessing()
{
    BOOST_LOG_SEV(this->lg, Information) << "Overall Control: Setting SECANT processing threads to terminate";
    //set processing to terminate
    this->processingCtrl->setToTerminate();
    //force any acquisition threads waiting on the availability of an outgoing
    //queue to spin until they get rid of their data
    for(auto&& queue: dataQueues)
    {
        queue->forceWakeAll();
    }
    
    //do a pause and check in this thread to wait until all acquisition threads signal they are done
    while(!(this->processingCtrl->getThreadsTerminated() == this->numProcessingThreads))
    {
        boost::this_thread::sleep_for(boost::chrono::microseconds(100));
    }
    
    //clear the force wake condition from the queues
    for(auto&& queue: dataQueues)
    {
        queue->clearForce();
    }
    BOOST_LOG_SEV(this->lg, Information) << "Overall Control: Done setting SECANT processing threads to terminate";
}

}
