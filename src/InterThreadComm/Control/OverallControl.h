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
//includes from other libraries
//includes from ORCHID
#include"SlowControlsThreadController.h"
#include"AcquisitionThreadControl.h"
#include"ProcessingThreadControl.h"
#include"InterThreadComm/Data/FileData.h"

namespace InterThread
{

class OverallControl
{
public:
    OverallControl();
    ~OverallControl(){}
    
    //retrieves individual controllers for the threads
    FileData* getFileDataCtrl(){return fileData;}
    SlowControlsThreadController* getSlowControlsCtrl(){return slowControlsControl;}
    ProcessingThreadControl* getProcessingThreadCtrl(){return processingControl;}
    AcquisitionThreadControl* getAcquisitionThreadCtrl(){return acquistionControl;}
    
    
    //sends terminate and waits for response to groups of threads in the following order:
    //first turn off the production of data
    //1) acqusition threads
    //2) slow controls thread
    //then shutdown the processing and writing of data
    //3) processing threads
    void shutDown();
    //puts everyone into stop condition first acquisition threads, then slow ctrls threads,
    //then processing threads
    void idleEverything();
    //first calls idle everything to put it all in stop mode. It then puts the
    //slow controlls thread in polling mode
    void pollSlowControls();
    //sets the file data using the given runTitle and runNum, then puts the
    //slow controls thread into writing mode, puts the processing threads into
    //active mode, and finally puts the acquisition threads into acquiring mode
    void startAcquisition(const std::string& runTitle, int runNum);
private:
    AcquisitionThreadControl* acquisitionControl;
    ProcessingThreadControl* processingControl;
    SlowControlsThreadController* slowControlsControl;
    FileData* fileData;

};
}
