/****************************************************************************
 *
 * File OverallControl.h
 *
 * Holds the definition for the OverallControl class which handles coordinating
 *    all the thread controls
 *****************************************************************************/
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
    OverallControl(AcquisitionThreadControl* acqCtrl, ProcessingThreadControl* prcCtrl,
                   SlowControlsThreadController* slwCtrl, FileData* filDat);
    ~OverallControl(){}
    
    //sends terminate and waits for response to groups of threads in the following order:
    //first turn off the production of data
    //1) acqusition threads
    //2) slow controls thread
    //then shutdown the processing and writing of data
    //3) processing threads
    void shutDown();
    //puts everyone into stop condition
    void idleEverything();
    void pollSlowControls();
    void startAcquisition(const std::string& runTitle, int runNum);
private:
    AcquisitionThreadControl* acquisitionControl;
    ProcessingThreadControl* processingControl;
    SlowControlsThreadController* slowcontrolsControl;
    FileData* fileData;

};
}
