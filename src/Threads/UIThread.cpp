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
// includes from other libraries
#include<boost/thread.hpp>
#include<boost/algorithm/string.hpp>
#include<ncurses.h>
// includes from ORCHID
#include"UICommandTable.h"

namespace Threads
{

static const int errorColor = 1;
static const int goodColor = 2;

UIThread::UIThread(SlowControls::SlowData* slDat,
                   FastData::RateData* rtDat,
                   int refreshFrequency):
    slowData(slDat), rateData(rtDat), persistCount(-1), command(""),
    persistentMessage(""), runLoop(true), refreshRate(refreshFrequency),
    mode(UIMode::Init)
{
    //calculate the refresh period in seconds then multiply by one billion to get
    //nanoseconds, which is what boost thread takes
    float refPeriod = 1000000000.0*(1.0/static_cast<float>(refreshFrequency));
    this->rateMultiplier = static_cast<float>(refreshFrequency);
    this->refreshPeriod = boost::chrono::nanoseconds(static_cast<long long int>(refPeriod));
}

void UIThread::operator() ()
{
    //do initial set up of ncurses
    initscr();
    clear();
    keypad(stdscr, true);
    cbreak();
    nodelay(stdscr, true);
    //prep for initial drawing
    //get the screen size
    this->handleScreenResize();
    
    //find out if we can use color to draw things
    if(has_colors())
    {  
        start_color();
        use_default_colors();
    }
    //init some colors for usage
    init_pair(errorColor, COLOR_RED, COLOR_BLACK);
    init_pair(goodColor, COLOR_GREEN, COLOR_BLACK);
    
    //holds characters that were input during the loop
    int inChar;
    //start the event loop
    while(runLoop)
    {
        //here we check if a key was pressed
        if((inChar = getch()) != ERR) //if we get ERR then no key was pressed in the period
        {
            this->handleKeyPress(inChar);
        }
        //here we draw the screen
        //draw the command in progress
        drawScreen();
        boost::this_thread::sleep_for(this->refreshPeriod);
    }
    endwin();
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
}

void UIThread::drawInitScreen()
{
    wmove(stdscr, 0, 0);
    printw("Status: Not Ready");
    wmove(stdscr, 1, 0);
    printw("Commands Available");
    wmove(stdscr, 2, 4);
    printw("turnon");
    wmove(stdscr, 2, 16);
    printw("- Connects to digitizer and activates MPOD");
    wmove(stdscr, 3, 4);
    printw("quit/exit");
    wmove(stdscr, 3, 16);
    printw("- Exit ORCHID");
    
    this->drawPersistentMessage();
    this->drawCommandInProgress();
}

void UIThread::drawIdleScreen()
{
    wmove(stdscr, 0, 0);
    printw("Status: Idle");
    wmove(stdscr, 1, 0);
    printw("Commands Available");
    wmove(stdscr, 2, 4);
    printw("start");
    wmove(stdscr, 2, 16);
    printw("- Start taking data");
    wmove(stdscr, 3, 4);
    printw("name");
    wmove(stdscr, 3, 16);
    printw("- Set run name");
    wmove(stdscr, 4, 4);
    printw("number");
    wmove(stdscr, 4, 16);
    printw("- Set run number");
    wmove(stdscr, 5, 4);
    printw("turnoff");
    wmove(stdscr, 5, 16);
    printw("- Disconnect from digitizer and shutdown MPOD");
    wmove(stdscr, 6, 4);
    printw("quit/exit");
    wmove(stdscr, 6, 16);
    printw("- Exit ORCHID");
    
    this->drawPersistentMessage();
    this->drawCommandInProgress();
}

void UIThread::drawRunningScreen()
{
    this->drawPersistentMessage();
    this->drawCommandInProgress();
}

void UIThread::drawCommandInProgress()
{
    wmove(stdscr, this->commandRow, 0);
    printw("?> ");
    printw(command.c_str());
    refresh();
}

void UIThread::drawPersistentMessage()
{
    //first figure out if we should display the persistent message
    if(persistCount > 0)
    {
        wmove(stdscr, this->messageRow, 0);
        attron(COLOR_PAIR(persistColor));
        attron(A_BOLD);
        printw(this->persistentMessage.c_str());
        attroff(A_BOLD);
        attroff(COLOR_PAIR(persistColor));
        --persistCount;
    }
    else if(persistCount == 0)
    {
        wmove(stdscr, this->messageRow, 0);
        persistentMessage.clear();
        clrtoeol();
        --persistCount;
    }
}

void UIThread::handleCommand()
{
    //normalize the command to lowercase
    boost::to_lower(command);
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
            cmd = UICommands::Unavailable;
        }
    }
    else
    { cmd = UICommands::Invalid; }
    
    // switch on the command enum to figure out what to do
    switch(cmd)
    {
    case UICommands::Invalid:
        this->persistentMessage = "Error:  Invalid Command";
        this->persistColor = errorColor;
        this->persistCount = refreshRate*5;
        break;
    case UICommands::Quit:
        this->runGracefulShutdown();
        break;
    case UICommands::TurnOn:
        this->turnOn();
        break;
    case UICommands::TurnOff:
        this->turnOff();
        break;
    case UICommands::Start:
        this->startDataTaking();
        break;
    case UICommands::Stop:
        this->stopDataTaking();
        break;
    case UICommands::Next:
        this->incrementRunNumber();
        break;
    case UICommands::Number:
        this->setRunNumber();
        break;
    case UICommands::Name:
        this->setRunName();
        break;
    case UICommands::Unavailable:
        this->persistentMessage = "Error:  Command unvailable in this mode";
        this->persistColor = errorColor;
        this->persistCount = refreshRate*5;
        break;
    default:
        this->persistentMessage = "Critical Error: Got to command loop default case. This should be IMPOSSIBLE!";
        this->persistCount = refreshRate*60;
        break;
    }
}

void UIThread::handleScreenResize()
{
    getmaxyx(stdscr, this->numRows, this->numCols);
    commandRow = this->numRows - 1;
    messageRow = this->numRows - 3;
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
            wmove(stdscr, this->commandRow, lastCharInd+3);
            printw(" ");
            command.erase(lastCharInd, 1);   
        }
        break;
    case 'a'://I know it is probably a bit silly, but this was my first thought
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
    case ' ':
        command.append(1, inChar);
        break;
    default://anything not listed explicitly, ignore
        break;
    }
}

void UIThread::runGracefulShutdown()
{
    if(mode != UIMode::Init)
    {
        turnOff();
    }
    this->runLoop = false;
}

void UIThread::turnOn()
{
    //TODO write code to handle connecting to the digitizer and shutting
    //down the MPOD
    mode = UIMode::Idle;
}

void UIThread::turnOff()
{
    //TODO write code to handle disconnecting from the digitizer and shutting
    //down the MPOD
    mode = UIMode::Init;
}

void UIThread::startDataTaking()
{
    //TODO write code to put the digitizer in acquire mode, start the processing
    //threads, start the file thread, and start the slow controls polling thread
    mode = UIMode::Running;
}

void UIThread::stopDataTaking()
{
    //TODO write the code to put the digitizer in stopped mode, halt the processing
    //threads, halt the file thread, and stop the slow controls polling
    mode = UIMode::Idle;
}

void UIThread::incrementRunNumber()
{
    this->stopDataTaking();
    //TODO write code to increment the run number and set the sequence # to 0
    this->startDataTaking();
}

void UIThread::setRunNumber()
{
    //TODO write code to get a new run number from the user and then set the
    //run number to that and the sequence number to zero
}

void UIThread::setRunName()
{
    //TODO write code to get a new run name from the user and then set the run
    // name to that and reset the run number and sequence number to 0
}

}
