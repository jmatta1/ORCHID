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
    Vx1730Digitizer(int modNum, InputParser::DigitizerModuleData* modData,
                    InputParser::DigitizerChannelData* chanData);
    ~Vx1730Digitizer();
    
    //this takes a little while as the system has to not only set all the
    //parameters, it then needs to wait a little while, run the calibration,
    //and then wait a little more time
    void setupDigitizer();
    
    //puts the digitizer in running mode
    void startAcquisition();
    //takes the digitizer out of running mode
    void stopAcquisition();
    
    //waits for the digitizer to raise and interrupt about data full.
    //once an interrupt is raised, it reads the event size value to find out
    //how much it needs to read, then it iteratively reads data that data into
    //the given buffer
    void waitForInterruptToReadData(char* buffer);
    
    //give the max possible size of a buffer in bytes so that they can be pre-
    //allocated for the queueing system
    int getSizeOfReadBufferInBytes();
private:
    void writeErrorAndThrow(CAENComm_ErrorCode errVal);
    
    void writeCommonRegisterData();
    void writeGroupRegisterData();
    void writeIndividualRegisterData();
    
    int moduleNumber;
    int channelStartInd;
    int numChannel;
    int digitizerHandle;
    bool acqRunning;
    bool digitizerOpen;
    InputParser::DigitizerModuleData* moduleData;
    InputParser::DigitizerChannelData* channelData;
    
};

}

#endif //ORCHID_SRC_HARDWARE_DIGITIZER_DIGITIZER_H
