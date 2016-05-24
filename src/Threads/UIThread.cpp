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
UIThread::UIThread(SlowControls::SlowData* slDat,
                   FastData::RateData* rtDat,
                   int refreshFrequency):
    slowData(slDat), rateData(rtDat), persistCount(0), command(""),
    persistentMessage(""), runLoop(true), refreshRate(refreshFrequency)
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
    printw("ncurses has been initialized");
    boost::this_thread::sleep_for(boost::chrono::nanoseconds(500000000));
    clear();
    while(runLoop)
    {
        //here we check if a key was pressed
        if((inChar = getch()) != ERR)
        {
            this->handleKeyPress(inChar);
        }
        //here we draw the screen
        //draw the command in progress
        wmove(stdscr, 20, 0);
        printw(command.c_str());
        boost::this_thread::sleep_for(this->refreshPeriod);
    }

    endwin();
}

void UIThread::handleCommand()
{
    //normalize the command to lowercase
    boost::to_lower(command);
    //look up the command inside dispatch map
    std::map::iterator cmdFind = UI_COMMAND_DISPATCH.find(command);
    //clear the command and screen
    command.clear();
    clear();
    // get the command enumeration or set it to invalid
    UICommand cmd;
    if(cmdFind != UI_COMMAND_DISPATCH.end())
    {
        cmd = cmdFind->second;
    }
    else
    {
        cmd = UICommands::Invalid;
    }
    
    // switch on the command enum to figure out what to do
    switch()
    {
    case UICommands::Invalid:
        this->persistentMessage = "Invalid Command";
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
        wmove(stdscr, 20, lastCharInd);
        printw(" ");
        command.erase(lastCharInd, 1);
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
        command.append(1, inChar);
        break;
    default://anything not listed explicitly, ignore
        break;
    }
}

}
