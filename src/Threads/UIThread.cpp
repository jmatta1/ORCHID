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
    mode(UIMode::Options)
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
    //holds characters that were input
    int inChar;
    wmove(stdscr, 20, 0);
    if(has_colors())
    {  
        start_color();
        use_default_colors();
    }
    //init some colors for usage
    init_pair(errorColor, COLOR_RED, COLOR_BLACK);
    init_pair(goodColor, COLOR_GREEN, COLOR_BLACK);
    while(runLoop)
    {
        //here we check if a key was pressed
        if((inChar = getch()) != ERR)
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
    case UIMode::Options:
        this->drawOptionsScreen();
        break;
    case UIMode::Running:
        this->drawRunningScreen();
        break;
    }
}

void UIThread::drawOptionsScreen()
{
    wmove(stdscr, 0, 0);
    printw("Status: Not taking data");
    wmove(stdscr, 1, 0);
    printw("Commands Available");
    wmove(stdscr, 2, 4);
    printw("start");
    wmove(stdscr, 2, 16);
    printw("- Starts data taking with current settings");
    wmove(stdscr, 3, 4);
    printw("name");
    wmove(stdscr, 3, 16);
    printw("- Change run name");
    wmove(stdscr, 4, 4);
    printw("number");
    wmove(stdscr, 4, 16);
    printw("- Change run number");
    wmove(stdscr, 5, 4);
    printw("quit/exit");
    wmove(stdscr, 5, 16);
    printw("- Exit ORCHID");
    
    wmove(stdscr, 20, 0);
    printw(command.c_str());
    refresh();
}

void UIThread::drawRunningScreen()
{
    //first figure out if we should display the persistent message
    if(persistCount > 0)
    {
        wmove(stdscr, 0, 0);
        printw("Message: ");
        attron(COLOR_PAIR(persistColor));
        attron(A_BOLD);
        printw(this->persistentMessage.c_str());
        attroff(A_BOLD);
        attroff(COLOR_PAIR(persistColor));
        --persistCount;
    }
    else if(persistCount == 0)
    {
        wmove(stdscr, 0, 0);
        printw("         ");
        int pMsgSize = persistentMessage.size();
        persistentMessage.clear();
        persistentMessage.append(pMsgSize, ' ');
        printw(persistentMessage.c_str());
        persistentMessage.clear();
        --persistCount;
    }
    wmove(stdscr, 20, 0);
    printw(command.c_str());
    refresh();
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
    }
    else
    {
        cmd = UICommands::Invalid;
    }
    
    // switch on the command enum to figure out what to do
    switch(cmd)
    {
    case UICommands::Invalid:
        this->persistentMessage = "Invalid Command";
        this->persistColor = errorColor;
        this->persistCount = refreshRate*5;
        break;
    case UICommands::Quit:
        this->runLoop = false;
        break;
    default:
        this->persistentMessage = "Got to command loop default. This should be IMPOSSIBLE!";
        this->persistCount = refreshRate*10;
        break;
    }
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
    case KEY_BACKSPACE:
    case KEY_DC:
        if(lastCharInd >= 0)
        {
            wmove(stdscr, 20, lastCharInd);
            printw(" ");
            command.erase(lastCharInd, 1);   
        }
        break;
    case 'a'://I know it is probably a bit silly, but this was my first thought
    case 'b'://on how to make certain that only non silly characters make it
    case 'c'://to the command
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

}
