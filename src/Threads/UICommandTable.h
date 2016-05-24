/***************************************************************************//**
********************************************************************************
**
** @file UICommandTable.h
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
** @details Holds the command enumeration list and the map to strings to enums
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHID_SRC_THREADS_UICOMMANDTABLE_H
#define ORCHID_SRC_THREADS_UICOMMANDTABLE_H

// includes for C system headers
// includes for C++ system headers
#include<map>
#include<string>
// includes from other libraries
#include<boost/assign/list_of.hpp>
using boost::assign::map_list_of;
// includes from ORCHID
namespace Threads
{

//The various enumerated commands
enum class UICommands : char {Quit, Invalid};

//A lookup between string and the command
static const std::map<std::string, UICommands> UI_COMMAND_DISPATCH =
    map_list_of("exit", UICommands::Quit)
        ("quit", UICommands::Quit)
        ("nope", UICommands::Quit);
}

#endif //ORCHID_SRC_THREADS_UICOMMANDTABLE_H
