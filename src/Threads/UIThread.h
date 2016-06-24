/***************************************************************************//**
********************************************************************************
**
** @file UIThread.h
** @author James Till Matta
** @date 27 Apr, 2016
** @brief
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** @details Holds the definition of the class that is run by main to handle the
** user interface
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHID_SRC_THREADS_UITHREAD_H
#define ORCHID_SRC_THREADS_UITHREAD_H

// includes for C system headers
// includes for C++ system headers
#include<string>
// includes from other libraries
#include<boost/chrono.hpp>
#define NCURSES_MOUSE_VERSION 1
#include<ncurses.h>
// includes from ORCHID
#include"Utility/MpodMapper.h"
#include"SlowControls/HVLib/MpodController.h"
#include"InterThreadComm/Data/SlowData.h"
#include"InterThreadComm/Data/RateData.h"
#include"InterThreadComm/Data/FileData.h"
#include"InterThreadComm/Control/SlowControlsThreadController.h"
#include"InterThreadComm/Control/FileOutputThreadController.h"
#include"Utility/OrchidLogger.h"
#include"Utility/CommonTypeDefs.h"

namespace Threads
{
//An enumeration for the modes the display might be in
enum class UIMode : char {Init, Idle, Running};

class UIThread
{
public:
    UIThread(InterThread::SlowData* slDat, InterThread::RateData* rtDat,
             InterThread::FileData* fiDat, Utility::MpodMapper* mpodMap,
             InterThread::SlowControlsThreadController* sctCtrl,
             InterThread::FileOutputThreadController* fileCtrl,
             Utility::ToFileMultiQueue* fileDataQueue,
             SlowControls::MpodController* mpCtrl,
             int refreshFrequency, int pollingRate);
    ~UIThread(){}

    //this is the function that is called by boost::thread when making a thread
    void operator() ();
private:
    /*
    ** Private Member Functions
    */
    /** Drawing and standard UI functions **/
    //This function handles initial setup of the window
    void initScreen();
    //This function is used to handle key presses
    void handleKeyPress(int inChar);
    //this function is used to handle key presses in set run number
    void handleSetRunNumKeyPress(int inChar);
    //this function is used to handle key presses in set run title
    void handleSetRunTitleKeyPress(int inChar);
    //This function handles commands when the user presses enter
    void handleCommand();
    //this function selects the appropriate draw function based on mode
    void drawScreen();
    //This function draws the main options screen
    void drawInitScreen();
    //This function draws the main options screen
    void drawIdleScreen();
    //This function draws the screen while the system is taking data
    void drawRunningScreen();
    //This function draws the file information across the top of the text window
    void drawFileInfo();
    //This function draws the overall digitizer information on the second line of the text window
    //void drawDigitizerInfo();
    //This function draws the global slow controls information on the third line of the text window
    void drawGlobalSlowControlsInformation();
    //this function draws the slow controls information tables
    void drawSlowControlsGrid();
    //This function handles drawing the command in progress in the message window
    void drawCommandInProgress();
    //This function handles drawing any persistent message that may need to
    //be drawn in the message window
    void drawPersistentMessage();
    //This function does screen sizing stuff and figures out how to position
    //things, or how to yell at the user to fix it if the screen is too small
    void handleScreenResize();
    //This loop handles waiting for the user to fix their mistake in making the
    //window too small
    void waitForResize();
    
    //** Wait For Other Threads Functions **/
    void waitForAllTerminations();
    void waitForSlowControlsThreadTermination();
    void waitForFileThreadTermination();
    
    /** Command functions **/
    //shuts down orchid, disconnecting from digitizer and ramping down voltages
    //if it has not already been done
    void runGracefulShutdown();
    //connects to the digitizer, turns on the mpod and ramps up voltages
    void turnOn();
    //disconnects from the digitizer, ramps down voltages and turns off the mpod
    void turnOff();
    //starts the digitizer running and data processing
    void startDataTaking();
    //stops the digitizer running, and finishes handling the data in the system
    void stopDataTaking();
    // sets the run number
    void setRunNumber();
    // sets the run name
    void setRunName();
    // sets the run name, number and description
    void setRunParams();
    
    /*
    ** Private Member Variables
    */
    /**  Variables used to aggregate trigger data and the like*/
    //we may have a pointer to these data but we do not own it so: No deletions!
    // *vader says shaking his finger at Boba Fett*
    InterThread::SlowData* slowData;
    InterThread::RateData* rateData;
    InterThread::FileData* fileData;
    InterThread::SlowControlsThreadController* sctControl;
    InterThread::FileOutputThreadController* fileControl;
    SlowControls::MpodController* mpodController;
    Utility::MpodMapper* mpodMapper;
    Utility::ToFileMultiQueue* fileMultiQueue;
    //multiplier for calculating rate from number of triggers
    float rateMultiplier;
    
    /**  Variables for managing the display output**/
    //the actual refresh rate in Hz good for calculations of persist count
    //merely multiply this by the number of seconds you wish to persist and
    //set persistCount to that value
    int refreshRate;
    //string to hold persistent messages in
    std::string persistentMessage;
    //integer to store persistent message color in
    int persistColor;
    //counter for number of refreshes persistent messages persist for
    long persistCount;
    
    /**Variables for displaying file info*/
    std::string fileName;
    std::string runTitle;
    int runNumber;
    int sequenceNumber;
    //variable to store the size of the file when we check
    long long lastFileSize;
    
    //Variables to store temporary data when loading new run parameters
    std::string tempRunTitle;
    int tempRunNumber;
    
    /**  Variables for managing the user interface**/
    //amount of time to sleep for
    boost::chrono::nanoseconds refreshPeriod;
    //amount of time to sleep for to make certain that the slow controls thread
    //has gotten its act together
    boost::chrono::nanoseconds slowControlsPollingWaitPeriod;
    //string to hold commands in
    std::string command;
    //flag to continue or kill loop
    bool runLoop;
    //flag to continue or kill a subloop
    bool runSubLoop;
    //enum to hold what mode we are in
    UIMode mode;
    
    /**  Variables for managing basic screen drawing**/
    int numRows;
    int numCols;
    WINDOW* textWindow;
    WINDOW* messageWindow;
    boost::log::sources::severity_logger_mt<LogSeverity>& lg;
    //int startLine;
    //int sizeDiff;
};

}
#endif //ORCHID_SRC_THREADS_UITHREAD_H
