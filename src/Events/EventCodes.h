/***************************************************************************//**
********************************************************************************
**
** @file EventCodes.h
** @author James Till Matta
** @date 07 June, 2016
** @brief
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** @details Holds the values that define each event type
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHID_SRC_EVENTS_EVENTCODES_H
#define ORCHID_SRC_EVENTS_EVENTCODES_H

namespace Events
{

static const int FileEventCode = 0x00000000;
static const int SlowControlsEventCode = 0x00000001;
static const int DigitizerPsdEventCode = 0x00000002;

}

#endif //ORCHID_SRC_EVENTS_EVENTCODES_H
