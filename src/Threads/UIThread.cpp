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
#include<ncurses.h>
// includes from ORCHID
namespace Threads
{
UIThread::UIThread(SlowControls::SlowData* slDat, int refreshFrequency):
    slowData(slDat)
{
    //calculate the refresh period in seconds then multiply by one billion to get
    //nanoseconds, which is what boost thread takes
    float refPeriod = 1000000000.0*(1.0/static_cast<float>(refreshFrequency));
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
    //make the stuff used to hanndle characters typed as we go
    int inChar;
    std::string command;
    printw("ncurses has been initiallized");
    boost::this_thread::sleep_for(this->refreshPeriod);
    bool runFlag = true;
    while(runFlag)
    {
        //here we check if a key was pressed
        if((inChar=getch())!=ERR)
        {
            int lastCharInd = (command.size() - 1);
            switch(inChar)
            {
            case KEY_ENTER:
            case '\r':
            case '\n':
            case '\f':
                //TODO better command execution
                if(command=="quit" || command=="exit")
                {
                    runFlag = false;
                }
                command.clear();
                clear();
                break;
            case KEY_BACKSPACE:
            case KEY_DC:
                wmove(stdscr, 20, lastCharInd);
                printw(" ");
                command.erase(lastCharInd, 1);
                break;
            case KEY_CTAB:
                // translate tabs into spaces
                command.append(1, ' ');
                break;
            default:
                command.append(1, inChar);
                break;
            }
        }
        //here we draw the screen
        //draw the command in progress
        wmove(stdscr, 20, 0);
        printw(command.c_str());
        boost::this_thread::sleep_for(this->refreshPeriod);
    }

    endwin();
}

}
