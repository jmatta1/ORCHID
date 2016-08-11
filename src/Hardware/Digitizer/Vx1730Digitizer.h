/***************************************************************************//**
********************************************************************************
**
** @file Digitizer.h
** @author James Till Matta
** @date 27 June, 2016
** @brief
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** @details Definition file for the Digitizer class
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHID_SRC_HARDWARE_DIGITIZER_DIGITIZER_H
#define ORCHID_SRC_HARDWARE_DIGITIZER_DIGITIZER_H
// includes for C system headers
// includes for C++ system headers
// includes from other libraries
// includes from ORCHID
#include"InputLib/Blocks/DigitizerModuleData.h"
#include"InputLib/Blocks/DigitizerChannelData.h"

namespace Digitizer
{

class Vx1730Digitizer
{
public:
    Vx1730Digitizer(int modNum, const InputParser::DigitizerModuleData& modData,
                    const InputParser::DigitizerChannelData& chanData);
    ~Vx1730Digitizer();
    
    void setupDigitizer();
private:
    
};

}

#endif //ORCHID_SRC_HARDWARE_DIGITIZER_DIGITIZER_H
