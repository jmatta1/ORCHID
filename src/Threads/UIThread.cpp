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
** @details Holds the implementation of the class that is run by main to handle
** the user interface
**
********************************************************************************
*******************************************************************************/

#include"UIThread.h"
// includes for C system headers
// includes for C++ system headers
#include<string>
#include<sstream>
#include<iomanip>
// includes from other libraries
#include<boost/thread.hpp>
#include<boost/algorithm/string.hpp>
#include <boost/spirit/include/qi.hpp>
// includes from ORCHID
#include"UICommandTable.h"
#include"Utility/OrchidLogger.h"

namespace Threads
{

static const int errorColor = 1;
static const int goodColor = 2;
static const int gridStartLine = 5;
static const int tempStartCol = 20;
static const int volStartCol = 36;

UIThread::UIThread(InterThread::SlowData* slDat, InterThread::RateData* rtDat,
                   InterThread::FileData* fiDat, Utility::MpodMapper* mpodMap,
                   InterThread::SlowControlsThreadController* sctCtrl,
                   SlowControls::MpodController* mpCtrl, int refreshFrequency,
                   int pollingRate):
    slowData(slDat), rateData(rtDat), fileData(fiDat), mpodMapper(mpodMap), sctControl(sctCtrl),
    mpodController(mpCtrl), persistCount(-1), lastFileSize(0), command(""),
    persistentMessage(""), runLoop(true), refreshRate(refreshFrequency),
    mode(UIMode::Init), textWindow(nullptr), messageWindow(nullptr), lg(OrchidLog::get())
{
    //calculate the refresh period in seconds then multiply by one billion to get
    //nanoseconds, which is what boost thread takes
    long long int refPeriod = (1000000000/refreshFrequency);
    this->rateMultiplier = static_cast<float>(refreshFrequency);
    this->refreshPeriod = boost::chrono::nanoseconds(refPeriod);
    
    long long int pollPeriod = static_cast<long long int>(1.5*static_cast<float>(1000000000/pollingRate));
    this->slowControlsPollingWaitPeriod = boost::chrono::nanoseconds(pollPeriod);
}

void UIThread::operator() ()
{
    //do initial set up of ncurses
    this->initScreen();
    //prep for initial drawing
    
    //holds characters that were input during the loop
    int inChar;
    //start the event loop
    while(runLoop)
    {
        //here we draw the screen
        drawScreen();
        //here we check if a key was pressed
        if((inChar = getch()) != ERR) //if we get ERR then no key was pressed in the period
        {
            this->handleKeyPress(inChar);
        }
        //here we sleep a bit
        boost::this_thread::sleep_for(this->refreshPeriod);
    }
    this->waitForAllTerminations();
    delwin(this->textWindow);
    delwin(this->messageWindow);
    endwin();
}

void UIThread::initScreen()
{
    initscr();
    keypad(stdscr, true);
    nodelay(stdscr, true);
    cbreak();
    noecho();
    clear();
    
    //get the terminal size, if it is not big enough force the user to resize
    //the terminal until we get at least the minimum size
    this->numRows = LINES; this->numCols = COLS;
    while((this->numRows < 14) || (this->numCols < 72))
    {
        wclear(this->textWindow);
        wclear(this->messageWindow);
        clear();
        wmove(this->textWindow, 0, 0);
        printw("Too Small! Make Window Bigger");
        this->waitForResize();
        this->numRows = LINES; this->numCols = COLS;
    }
    clear();
    
    //make the sub windows
    this->textWindow = newwin(this->numRows - 4, this->numCols, 0, 0);
    this->messageWindow = newwin(4, this->numCols, this->numRows - 4, 0);
    scrollok(this->textWindow, true);
    //find out if we can use color to draw things
    if(has_colors())
    {  
        start_color();
        use_default_colors();
    }
    //init some colors for usage
    init_color(COLOR_BLACK, 0, 0, 0);
    init_color(COLOR_RED, 1000, 0, 0);
    init_color(COLOR_GREEN, 0, 1000, 0);
    init_color(COLOR_GLUE, 0, 0, 1000);
    init_color(COLOR_WHITE, 1000, 1000, 1000);
    init_pair(errorColor, COLOR_RED, COLOR_BLACK);
    init_pair(goodColor, COLOR_GREEN, COLOR_BLACK);
}

void UIThread::drawScreen()
{
    switch(mode)
    {
    case UIMode::Init:
        this->drawInitScreen();
        break;
    case UIMode::Idle:
        this->drawIdleScreen();
        break;
    case UIMode::Running:
        this->drawRunningScreen();
        break;
    }
    refresh();
    wrefresh(this->textWindow);
    wrefresh(this->messageWindow);
}

void UIThread::drawInitScreen()
{
    //this->sizeDiff = 0;
    mvwprintw(this->textWindow, 0, 0,  "Status: Not Ready");
    mvwprintw(this->textWindow, 1, 0,  "Commands Available");
    mvwprintw(this->textWindow, 2, 4,  "turnon");
    mvwprintw(this->textWindow, 2, 16, "- Connects to digitizer and activates MPOD");
    mvwprintw(this->textWindow, 3, 4,  "quit/exit");
    mvwprintw(this->textWindow, 3, 16, "- Exit ORCHID");
    
    this->drawPersistentMessage();
    this->drawCommandInProgress();
}

void UIThread::drawIdleScreen()
{
    //this->sizeDiff = 0;
    mvwprintw(this->textWindow, 0, 0, "Status: Idle");
    mvwprintw(this->textWindow, 1, 0,  "Commands Available");
    mvwprintw(this->textWindow, 2, 4,  "start");
    mvwprintw(this->textWindow, 2, 16, "- Start taking data");
    mvwprintw(this->textWindow, 3, 4,  "name");
    mvwprintw(this->textWindow, 3, 16, "- Set run name");
    mvwprintw(this->textWindow, 4, 4,  "number");
    mvwprintw(this->textWindow, 4, 16, "- Set run number");
    mvwprintw(this->textWindow, 5, 4,  "turnoff");
    mvwprintw(this->textWindow, 5, 16, "- Disconnect from digitizer and shutdown MPOD");
    mvwprintw(this->textWindow, 6, 4,  "quit/exit");
    mvwprintw(this->textWindow, 6, 16, "- Exit ORCHID");
    
    this->drawPersistentMessage();
    this->drawCommandInProgress();
}

void UIThread::drawFileInfo()
{
    //check if we need to get new values
    if(fileData->fileNameChangeSinceLastGet())
    {
        fileData->getFileName(this->fileName);
    }
    if(fileData->runTitleChangeSinceLastGet())
    {
        fileData->getRunTitle(this->runTitle);
    }
    if(fileData->runNumberChangeSinceLastGet())
    {
        this->runNumber = fileData->getRunNumber();
    }
    if(fileData->sequenceNumberChangeSinceLastGet())
    {
        this->sequenceNumber = fileData->getSequenceNumber();
    }
    //Generate the file info string
    std::ostringstream builder;
    builder << "Run Title: " << this->runTitle << " | Run #: " << this->runNumber;
    builder << " | File #: " << this->sequenceNumber << " | Rate: ";
    //get the file size and calculate the current file write rate
    long long tempFileSize = this->fileData->getSize();
    long long rate = (tempFileSize - this->lastFileSize) * this->rateMultiplier;
    this->lastFileSize = tempFileSize;
    //calculate if the file write rate is in thousands or millions etc
    if(rate > 1000000)
    {
        builder << std::setw(4) << std::setfill(' ') << std::setprecision(3) << (static_cast<float>(rate)/1048576.0) << "M";
    }
    else if(rate > 1000)
    {
        builder << std::setw(4) << std::setfill(' ') << std::setprecision(3) << (static_cast<float>(rate)/1024.0) << "k";
    }
    else
    {
        builder << std::setw(4) << std::setfill(' ') << rate;
    }
    builder << "B/s | File: " << this->fileName;
    mvwprintw(this->textWindow, 0, 0, builder.str().c_str());
}

void UIThread::drawGlobalSlowControlsInformation()
{
    std::ostringstream builder;
    //since we do not want to constantly regenerate and resize this variable
    //plus we want to avoid having too many class members (there are a crapton already)
    static std::string mpodCrateStatus = "Status Not Updating";
    this->slowData->genCrateInfoString(mpodCrateStatus);
    builder << "MPOD Crate Status: " << mpodCrateStatus;
    static std::string temperatureReaderStatus = "Not Updating";
    //this->slowData->genTempReaderInfoString(temperatureReaderStatus);
    builder << " | Temperature Reader Status: " << temperatureReaderStatus;
    mvwprintw(this->textWindow, 3, 0, builder.str().c_str());
}

void UIThread::drawSlowControlsGrid()
{
    int currentRow = gridStartLine;
    //now draw the topmost separators
    mvwprintw(this->textWindow, currentRow, tempStartCol, "--------------");
    mvwprintw(this->textWindow, currentRow, volStartCol, "---------------------------------------");
    ++currentRow;
    //now draw the column headers
    mvwprintw(this->textWindow, currentRow, tempStartCol, "|Chan|  Temp |");
    mvwprintw(this->textWindow, currentRow, volStartCol, "| Chan | TerVol | Current | Status");
    ++currentRow;
    //more separators
    mvwprintw(this->textWindow, currentRow, tempStartCol, "==============");
    mvwprintw(this->textWindow, currentRow, volStartCol, "=======================================");
    ++currentRow;
    //now loop through the temp sensors
    int currDataLine = currentRow;
    int stopLine = currentRow + 2*this->slowData->numTemperatureChannels;
    int chanInd = 0;
    while(currDataLine < stopLine)
    {
        mvwprintw(this->textWindow, currDataLine, tempStartCol, "|    |       |");
        //TODO: write out temp information
        ++currDataLine;
    }
    mvwprintw(this->textWindow, currDataLine, tempStartCol, "================");
    //now loop through the voltage sensors
    currDataLine = currentRow;
    stopLine = currentRow + 2*this->slowData->numVoltageChannels;
    chanInd = 0;
    while(currDataLine < stopLine)
    {
        //build the voltage data string
        std::ostringstream builder;
        //first add the channel in the usual format
        builder << "| u" << (this->mpodMapper->moduleNum[chanInd] - 1) << (this->mpodMapper->channelNum[chanInd] - 1) << " | ";
        //add the voltage
        builder << std::setw(4) << std::setprecision(2) << (this->slowData->terminalVoltage[chanInd]/1000.0) <<"kV | ";
        //add the current
        builder << std::setw(5) << std::setprecision(1) << (this->slowData->current[chanInd]) <<"uA | ";
        //add the status
        std::string statusString;
        this->slowData->genChannelInfoString(i, statusString);
        builder << statusString;
        mvwprintw(this->textWindow, currentRow, volStartCol, builder.str().c_str());
        ++chanInd;
    }
    
    mvwprintw(this->textWindow, currentRow, volStartCol, "=======================================");
    
}

void UIThread::drawRunningScreen()
{
    //draw the file information line
    this->drawFileInfo();
    //draw the digitizer info line
    
    //draw the slow controls global info line
    this->drawGlobalSlowControlsInformation();
    //draw the slow controls info grid();
    this->drawSlowControlsGrid();
    //draw the message stuff
    this->drawPersistentMessage();
    this->drawCommandInProgress();
}

void UIThread::drawCommandInProgress()
{
    mvwprintw(this->messageWindow, 3, 0, "?> ");
    mvwprintw(this->messageWindow, 3, 3, command.c_str());
}

void UIThread::drawPersistentMessage()
{
    //first figure out if we should display the persistent message
    if(persistCount > 0)
    {
        //attron(COLOR_PAIR(persistColor));
        wattron(this->messageWindow, A_BOLD);
        wattron(this->messageWindow, COLOR_PAIR(persistColor));
        mvwprintw(this->messageWindow, 1, 0, this->persistentMessage.c_str());
        wattroff(this->messageWindow, A_BOLD);
        wattroff(this->messageWindow, COLOR_PAIR(persistColor));
        --persistCount;
    }
    else if(persistCount == 0)
    {
        wmove(this->messageWindow, 1, 0);
        persistentMessage.clear();
        wclrtoeol(this->messageWindow);
        --persistCount;
    }
}

void UIThread::handleCommand()
{
    //normalize the command to lowercase
    boost::to_lower(command);
    // log that we have a command
    BOOST_LOG_SEV(this->lg, Information) << "Got Command: " << command;
    //look up the command inside dispatch map
    std::map<std::string, UICommands>::const_iterator cmdFind = UI_COMMAND_DISPATCH.find(command);
    //clear the command and screen
    command.clear();
    clear();
    // get the command enumeration or set it to invalid
    UICommands cmd;
    if(cmdFind != UI_COMMAND_DISPATCH.end())
    {
        cmd = cmdFind->second;
        bool okFlag = false;
        switch(this->mode)
        {
        case UIMode::Init:
            okFlag = INIT_MODE_VALID_CMD.find(cmd)->second;
            break;
        case UIMode::Idle:
            okFlag = IDLE_MODE_VALID_CMD.find(cmd)->second;
            break;
        case UIMode::Running:
            okFlag = RUN_MODE_VALID_CMD.find(cmd)->second;
            break;
        }
        if(!okFlag)
        {
            BOOST_LOG_SEV(this->lg, Information) << "Command is unavailable in current mode";
            cmd = UICommands::Unavailable;
        }
    }
    else
    {
        BOOST_LOG_SEV(this->lg, Information) << "Command is invalid";
        cmd = UICommands::Invalid;
    }
    
    // switch on the command enum to figure out what to do
    switch(cmd)
    {
    case UICommands::Invalid:
        this->persistentMessage = "Error:  Invalid Command";
        this->persistColor = errorColor;
        this->persistCount = refreshRate*5;
        break;
    case UICommands::Quit:
        this->persistCount = -1; //clear any error, clearly they fixed it
        wmove(this->messageWindow, 1, 0);
        persistentMessage.clear();
        wclrtoeol(this->messageWindow);
        this->runGracefulShutdown();
        break;
    case UICommands::TurnOn:
        this->persistCount = -1; //clear any error, clearly they fixed it
        wmove(this->messageWindow, 1, 0);
        persistentMessage.clear();
        wclrtoeol(this->messageWindow);
        this->turnOn();
        break;
    case UICommands::TurnOff:
        this->persistCount = -1; //clear any error, clearly they fixed it
        wmove(this->messageWindow, 1, 0);
        persistentMessage.clear();
        wclrtoeol(this->messageWindow);
        this->turnOff();
        break;
    case UICommands::Start:
        this->persistCount = -1; //clear any error, clearly they fixed it
        wmove(this->messageWindow, 1, 0);
        persistentMessage.clear();
        wclrtoeol(this->messageWindow);
        this->startDataTaking();
        break;
    case UICommands::Stop:
        this->persistCount = -1; //clear any error, clearly they fixed it
        wmove(this->messageWindow, 1, 0);
        persistentMessage.clear();
        wclrtoeol(this->messageWindow);
        this->stopDataTaking();
        break;
    case UICommands::Next:
        this->persistCount = -1; //clear any error, clearly they fixed it
        wmove(this->messageWindow, 1, 0);
        persistentMessage.clear();
        wclrtoeol(this->messageWindow);
        this->incrementRunNumber();
        break;
    case UICommands::Number:
        this->persistCount = -1; //clear any error, clearly they fixed it
        wmove(this->messageWindow, 1, 0);
        persistentMessage.clear();
        wclrtoeol(this->messageWindow);
        this->setRunNumber();
        break;
    case UICommands::Name:
        this->persistCount = -1; //clear any error, clearly they fixed it
        wmove(this->messageWindow, 1, 0);
        persistentMessage.clear();
        wclrtoeol(this->messageWindow);
        this->setRunName();
        break;
    case UICommands::Unavailable:
        this->persistentMessage = "Error:  Command unvailable in this mode";
        this->persistColor = errorColor;
        this->persistCount = refreshRate*5;
        break;
    default:
        BOOST_LOG_SEV(this->lg, Critical) << "Critical Error: Got to command switch default case. This should not be possible";
        this->persistentMessage = "Critical Error: Got to command switch default case. This should be IMPOSSIBLE!";
        this->persistColor = errorColor;
        this->persistCount = refreshRate*60;
        break;
    }
}

void UIThread::handleScreenResize()
{
    //get the terminal size, if it is not big enough force the user to resize
    //the terminal until we get at least the minimum size
    this->numRows = LINES; this->numCols = COLS;
    while((this->numRows < 14) || (this->numCols < 80))
    {
        wclear(this->textWindow);
        wclear(this->messageWindow);
        clear();
        wmove(this->textWindow, 0, 0);
        printw("Too Small! Make Window Bigger");
        this->waitForResize();
        this->numRows = LINES; this->numCols = COLS;
    }
    clear();
    delwin(this->textWindow);
    delwin(this->messageWindow);
    //make the sub windows
    this->textWindow = newwin(this->numRows - 4, this->numCols, 0, 0);
    this->messageWindow = newwin(4, this->numCols, this->numRows - 4, 0);
    scrollok(this->textWindow, true);
}

void UIThread::waitForResize()
{
    bool flag = true;
    int inChar;
    while(flag)
    {
        boost::this_thread::sleep_for(this->refreshPeriod);
        //here we check if a key was pressed
        if((inChar = getch()) == KEY_RESIZE) flag=false;
    }
    boost::this_thread::sleep_for(this->refreshPeriod);
}

void UIThread::handleKeyPress(int inChar)
{
    int lastCharInd = (command.size() - 1);
    switch(inChar)
    {
    case KEY_ENTER:
    case '\r':
    case '\n':
    case '\f':
        this->handleCommand();
        break;
    case KEY_RESIZE:
        this->handleScreenResize();
        break;
    case KEY_BACKSPACE:
    case KEY_DC:
        if(lastCharInd >= 0)
        {
            mvwprintw(this->messageWindow, 3, lastCharInd+3, " ");
            command.erase(lastCharInd, 1);   
        }
        break;
    case 'a'://I know it is probably a bit dumb, but this was my first thought
    case 'b'://on how to make certain that only non silly characters make it
    case 'c'://to the command with this all the meta characters etc are filtered
    case 'd':
    case 'e':
    case 'f':
    case 'g':
    case 'h':
    case 'i':
    case 'j':
    case 'k':
    case 'l':
    case 'm':
    case 'n':
    case 'o':
    case 'p':
    case 'q':
    case 'r':
    case 's':
    case 't':
    case 'u':
    case 'v':
    case 'w':
    case 'x':
    case 'y':
    case 'z':
    case 'A':
    case 'B':
    case 'C':
    case 'D':
    case 'E':
    case 'F':
    case 'G':
    case 'H':
    case 'I':
    case 'J':
    case 'K':
    case 'L':
    case 'M':
    case 'N':
    case 'O':
    case 'P':
    case 'Q':
    case 'R':
    case 'S':
    case 'T':
    case 'U':
    case 'V':
    case 'W':
    case 'X':
    case 'Y':
    case 'Z':
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    case '_':
    case '-':
    case '(':
    case ')':
    case '*':
    case '+':
    case '=':
    case ',':
    case '.':
    case '[':
    case ']':
    case '{':
    case '}':
    case ' ':
        command.append(1, inChar);
        break;
    default://anything not listed explicitly, ignore
        break;
    }
}

void UIThread::handleSetRunNumKeyPress(int inChar)
{
    int lastCharInd = (command.size() - 1);
    switch(inChar)
    {
    case KEY_ENTER:
    case '\r':
    case '\n':
    case '\f':
    {
        int temp;
        std::ostringstream builder;
        builder << "Run number set to: " << command ;
        this->persistentMessage = builder.str();
        this->persistColor = goodColor;
        this->persistCount = refreshRate*2;
        boost::spirit::qi::parse(command.begin(), command.end(), boost::spirit::qi::int_, temp);
        //TODO: actually call the command structure to change the file thread
        this->fileData->setRunNumber(temp);
        this->fileData->setSequenceNumber(0);
        command.clear();
        this->runSubLoop = false;
    }
        break;
    case KEY_RESIZE:
        this->handleScreenResize();
        break;
    case KEY_BACKSPACE:
    case KEY_DC:
        if(lastCharInd >= 0)
        {
            mvwprintw(this->messageWindow, 3, lastCharInd+3, " ");
            command.erase(lastCharInd, 1);   
        }
        break;
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
        command.append(1, inChar);
        break;
 
    default://anything not listed explicitly, ignore
        break;
    }
}

void UIThread::handleSetRunTitleKeyPress(int inChar)
{
    int lastCharInd = (command.size() - 1);
    switch(inChar)
    {
    case KEY_ENTER:
    case '\r':
    case '\n':
    case '\f':
    {
        //TODO: actually call the command structure to change the file thread
        std::ostringstream builder;
        builder << "Run title set to: " << command ;
        this->persistentMessage = builder.str();
        this->persistColor = goodColor;
        this->persistCount = refreshRate*2;
        this->fileData->setRunTitle(this->command);
        this->fileData->setSequenceNumber(0);
        command.clear();
        this->runSubLoop = false;
    }
        break;
    case KEY_RESIZE:
        this->handleScreenResize();
        break;
    case KEY_BACKSPACE:
    case KEY_DC:
        if(lastCharInd >= 0)
        {
            mvwprintw(this->messageWindow, 3, lastCharInd+3, " ");
            command.erase(lastCharInd, 1);   
        }
        break;
    case 'a'://I know it is probably a bit dumb, but this was my first thought
    case 'b'://on how to make certain that only non silly characters make it
    case 'c'://to the command with this all the meta characters etc are filtered
    case 'd':
    case 'e':
    case 'f':
    case 'g':
    case 'h':
    case 'i':
    case 'j':
    case 'k':
    case 'l':
    case 'm':
    case 'n':
    case 'o':
    case 'p':
    case 'q':
    case 'r':
    case 's':
    case 't':
    case 'u':
    case 'v':
    case 'w':
    case 'x':
    case 'y':
    case 'z':
    case 'A':
    case 'B':
    case 'C':
    case 'D':
    case 'E':
    case 'F':
    case 'G':
    case 'H':
    case 'I':
    case 'J':
    case 'K':
    case 'L':
    case 'M':
    case 'N':
    case 'O':
    case 'P':
    case 'Q':
    case 'R':
    case 'S':
    case 'T':
    case 'U':
    case 'V':
    case 'W':
    case 'X':
    case 'Y':
    case 'Z':
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    case '_':
    case '-':
    case '(':
    case ')':
    case '*':
    case '+':
    case '=':
    case ',':
    case '.':
    case '[':
    case ']':
        command.append(1, inChar);
        break;
    case ' ':
        command.append(1, '_');
        break;
    default://anything not listed explicitly, ignore
        break;
    }
}

void UIThread::waitForAllTerminations()
{
    this->waitForSlowControlsThreadTermination();
    //this->waitForDigitizerThreadTermination();
    //this->waitForEventProcessingThreadsTermination();
    //this->waitForFileThreadTermination();
}

void UIThread::waitForSlowControlsThreadTermination()
{
    wclear(this->textWindow);
    mvwprintw(this->textWindow, 0, 0, "Waiting For Termination of Slow Controls Thread");
    wrefresh(this->textWindow);
    wclear(this->messageWindow);
    wrefresh(this->messageWindow);
    sctControl->setToTerminate();
    //loop until we see the terminate signal from the slow controls thread
    while(!(sctControl->isDone()))
    {
        boost::this_thread::sleep_for(this->refreshPeriod);
    }
}

void UIThread::runGracefulShutdown()
{
    BOOST_LOG_SEV(this->lg, Information) << "Running Graceful Shutdown";
    if(mode == UIMode::Idle)
    {
        turnOff();
    }
    else if(mode == UIMode::Running)
    {
        stopDataTaking();
        turnOff();
    }
    this->runLoop = false;
    BOOST_LOG_SEV(this->lg, Information) << "Graceful Shutdown Complete";
}

void UIThread::turnOn()
{
    BOOST_LOG_SEV(this->lg, Information) << "Turning on the MPOD Crate";
    if(!this->mpodController->turnCrateOn())
    {
        BOOST_LOG_SEV(this->lg, Critical) << "Critical Error: MPOD either did not turn on or did not initialize";
        this->persistentMessage = "Critical Error: MPOD either did not turn on or did not initialize";
        this->persistColor = errorColor;
        this->persistCount = refreshRate*10;
        this->mpodController->turnCrateOff();//Undo anything that may have happened
        return; //then return without changing mode
    }
    BOOST_LOG_SEV(this->lg, Information) << "Turning on the HV channels";
    if(!this->mpodController->activateAllChannels())
    {
        BOOST_LOG_SEV(this->lg, Critical) << "Critical Error:  Error in turning on HV channels";
        this->persistentMessage = "Critical Error:  Error in turning on HV channels";
        this->persistColor = errorColor;
        this->persistCount = refreshRate*10;
        this->mpodController->deactivateAllChannels();
        this->mpodController->turnCrateOff();//Undo anything that may have happened
        return; //then return without changing mode
    }
    BOOST_LOG_SEV(this->lg, Information) << "Setting Slow Controls thread to polling";
    this->sctControl->setToPolling();
    wclear(this->textWindow);
    mvwprintw(this->textWindow, 0, 0, "Waiting for voltage ramp up");
    wrefresh(this->textWindow);
    //sleep for 1.5 * poll period to give the slow controls thread a chance to
    //read when the ramp up is running
    boost::this_thread::sleep_for(this->slowControlsPollingWaitPeriod);
    bool stillRamping = true;
    while(stillRamping)
    {
        wclear(this->textWindow);
        mvwprintw(this->textWindow, 0, 0, "Waiting for voltage ramp up");
        wrefresh(this->textWindow);
        //here we sleep a bit
        boost::this_thread::sleep_for(this->refreshPeriod);
        //here we check if there are any channels still ramping their voltages
        int rampCount = std::count(this->slowData->outputRampUp,
                                   &(this->slowData->outputRampUp[this->slowData->numVoltageChannels]), true);
        if(rampCount == 0)
        {
            stillRamping = false;
        }
    }
    BOOST_LOG_SEV(this->lg, Information) << "HV done ramping, connecting to digitizer";
    //TODO write code to connect to digitizer
    mode = UIMode::Idle;
    //this->startLine = 0;
    wclear(this->textWindow);
}

void UIThread::turnOff()
{
    BOOST_LOG_SEV(this->lg, Information) << "Disconnecting from digitizer";
    //TODO write code to handle disconnecting from the digitizer
    BOOST_LOG_SEV(this->lg, Information) << "Putting Slow Controls thread into polling mode";
    this->sctControl->setToPolling();
    BOOST_LOG_SEV(this->lg, Information) << "Starting channel ramp down";
    this->mpodController->deactivateAllChannels();
    wclear(this->textWindow);
    mvwprintw(this->textWindow, 0, 0, "Waiting for voltage ramp down");
    wrefresh(this->textWindow);
    //sleep for 1.5 * poll period to give the slow controls thread a chance to
    //read when the ramp down is running
    boost::this_thread::sleep_for(this->slowControlsPollingWaitPeriod);
    bool stillRamping = true;
    while(stillRamping)
    {
        wclear(this->textWindow);
        mvwprintw(this->textWindow, 0, 0, "Waiting for voltage ramp down");
        wrefresh(this->textWindow);
        //here we sleep a bit
        boost::this_thread::sleep_for(this->refreshPeriod);
        //here we check if there are any channels still ramping their voltages
        int rampCount = std::count(this->slowData->outputRampDown,
                                   &(this->slowData->outputRampDown[this->slowData->numVoltageChannels]), true);
        
        if(rampCount == 0)
        {
            stillRamping = false;
        }
    }
    BOOST_LOG_SEV(this->lg, Information) << "Done Ramping, stopping slow controls thread";
    this->sctControl->setToStop();
    this->mpodController->turnCrateOff();
    mode = UIMode::Init;
    wclear(this->textWindow);
}

void UIThread::startDataTaking()
{
    BOOST_LOG_SEV(this->lg, Information) << "Starting File Writing";
    //TODO put the file thread into running data mode
    BOOST_LOG_SEV(this->lg, Information) << "Starting Event Processing";
    //TODO put the event processing threads into running mode
    BOOST_LOG_SEV(this->lg, Information) << "Starting Slow Controls Event Generation";
    this->sctControl->setToWriting();
    //TODO put the digitizer thread into running mode
    BOOST_LOG_SEV(this->lg, Information) << "Starting Digitizer Thread Acquisition";
    //TODO put the digitizer into running mode
    BOOST_LOG_SEV(this->lg, Information) << "Starting Digitizer Run";
    
    mode = UIMode::Running;
    //this->startLine = 0;
    wclear(this->textWindow);
}

void UIThread::stopDataTaking()
{
    BOOST_LOG_SEV(this->lg, Information) << "Stopping Digitzer Run";
    //TODO put the digitizer into stopped mode
    BOOST_LOG_SEV(this->lg, Information) << "Digitizer Thread Set To Stop";
    //TODO put the digitizer thread into stopped mode
    BOOST_LOG_SEV(this->lg, Information) << "Event Processing Threads Set To Stop";
    //TODO put the event processing threads into finish and stop mode
    //TODO put in wait for processing threads to stop
    BOOST_LOG_SEV(this->lg, Information) << "Slow Controls Set To Polling Only";
    this->sctControl->setToPolling();
    BOOST_LOG_SEV(this->lg, Information) << "File Thread Set To Stop";
    //TODO put the file thread into finish and stop mode
    //TODO wait for file thread to stop
    mode = UIMode::Idle;
    //this->startLine = 0;
    wclear(this->textWindow);
}

void UIThread::incrementRunNumber()
{
    BOOST_LOG_SEV(this->lg, Information) << "Incrementing Run Number";
    this->stopDataTaking();
    //TODO call actual file thread control structure
    this->fileData->incrementRunNumber();
    this->startDataTaking();
    //this->startLine = 0;
    wclear(this->textWindow);
}

void UIThread::setRunNumber()
{
    BOOST_LOG_SEV(this->lg, Information) << "Setting Run Number";
    //run number to that and the sequence number to zero
    int inChar;
    this->runSubLoop = true;
    while(runSubLoop)
    {
        //here we draw the screen
        wclear(this->textWindow);
        mvwprintw(this->textWindow, 0, 0, "Please enter a run number");
        wrefresh(this->textWindow);
        this->drawPersistentMessage();
        this->drawCommandInProgress();
        wrefresh(this->messageWindow);
        //here we check if a key was pressed
        if((inChar = getch()) != ERR) //if we get ERR then no key was pressed in the period
        {
            this->handleSetRunNumKeyPress(inChar);
        }
        //now sleep for a bit
        boost::this_thread::sleep_for(this->refreshPeriod);
    }
    wclear(this->messageWindow);
}

void UIThread::setRunName()
{
    BOOST_LOG_SEV(this->lg, Information) << "Setting Run Name";
    //run number to that and the sequence number to zero
    int inChar;
    this->runSubLoop = true;
    while(runSubLoop)
    {
        //here we draw the screen
        wclear(this->textWindow);
        mvwprintw(this->textWindow, 0, 0, "Please enter a run title");
        wrefresh(this->textWindow);
        this->drawPersistentMessage();
        this->drawCommandInProgress();
        wrefresh(this->messageWindow);
        //here we check if a key was pressed
        if((inChar = getch()) != ERR) //if we get ERR then no key was pressed in the period
        {
            this->handleSetRunTitleKeyPress(inChar);
        }
        //now pause for a bit
        boost::this_thread::sleep_for(this->refreshPeriod);
    }
    wclear(this->messageWindow);
}

}
