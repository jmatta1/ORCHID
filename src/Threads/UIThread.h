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
// includes from ORCHID
#include"SlowControls/SlowData.h"
#include"DigiLib/RateData.h"
namespace Threads
{

//An enumeration for the modes the display might be in
enum class UIMode : char {Init, Idle, Running};

class UIThread
{
public:
    UIThread(SlowControls::SlowData* slDat, FastData::RateData* rtDat, int refreshFrequency);
    ~UIThread(){}

    //this is the function that is called by boost::thread when making a thread
    void operator() ();
private:
    /*
    ** Private Member Functions
    */
    /** Drawing and standard UI functions **/
    //This function is used to handle key presses
    void handleKeyPress(int inChar);
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
    //This function handles drawing the command in progress
    void drawCommandInProgress();
    //This function handles drawing any persistent message that may need to
    //be drawn
    void drawPersistentMessage();
    //This function does screen sizing stuff and figures out how to position
    //things, or how to yell at the user to fix it if the screen is too small
    void handleScreenResize();
    
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
    //momentarily stops the digitizer, flushes the file, changes the run number
    // and then restarts data taking
    void incrementRunNumber();
    // sets the run number
    void setRunNumber();
    // sets the run name
    void setRunName();
    
    /*
    ** Private Member Variables
    */
    /**  Variables used to aggregate trigger data and the like*/
    //we may have a pointer to these data but we do not own it so: No deletions!
    // *vader says shaking his finger at Boba Fett*
    SlowControls::SlowData* slowData;
    FastData::RateData* rateData;
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
    
    /**  Variables for managing the user interface**/
    //amount of time to sleep for
    boost::chrono::nanoseconds refreshPeriod;
    //string to hold commands in
    std::string command;
    //flag to continue or kill loop
    bool runLoop;
    //enum to hold what mode we are in
    UIMode mode;
    
    /**  Variables for managing basic screen drawing**/
    int numRows;
    int numCols;
    int messageRow;
    int commandRow;
    
};

}
#endif //ORCHID_SRC_THREADS_UITHREAD_H
