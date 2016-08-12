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
** @details Implementation file for the Digitizer class
**
********************************************************************************
*******************************************************************************/
#include"Vx1730Digitizer.h"
// includes for C system headers
// includes for C++ system headers
#include<iomanip>
// includes from other libraries
#include<CAENComm.h>
// includes from ORCHID
#include"Vx1730DigitizerRegisters.h"
#include"Utility/OrchidLogger.h"

namespace Digitizer
{

Vx1730Digitizer::Vx1730Digitizer(int modNum, InputParser::DigitizerModuleData* modData,
                                 InputParser::DigitizerChannelData* chanData) :
    moduleNumber(modNum), channelStartInd(0), numChannel(1), digitizerHandle(0),
    acqRunning(false), digitizerOpen(false), moduleData(modData), chanData(channelData)
{
    //find the channel start index
    while(chanData->moduleNumber[channelStartInd] != modNum)
    {
        ++channelStartInd;
    }
    //count the number of channels
    int ind = channelStartInd + 1;
    while(chanData->moduleNumber[ind] == modNum)
    {
        ++numChannel;
        ++ind;
    }
}

Vx1730Digitizer::~Vx1730Digitizer()
{
    if (acqRunning)
    {
        this->stopAcquisition();
    }
    if(digitizerOpen)
    {
        CAENComm_CloseDevice(this->digitizerHandle);
    }
}

void Vx1730Digitizer::setupDigitizer()
{
    //first open the digitizer
    CAENComm_ErrorCode errVal;
    InputParser::LinkType lType = moduleData->linkType[moduleNumber];
    if(lType == InputParser::LinkType::Optical)
    {
        BOOST_LOG_SEV(lg, Information) << "Opening VME Card Via Optical Link, Digitizer #" << moduleNumber << "\n";
        errVal = CAENComm_OpenDevice(CAENComm_OpticalLink,
                                     moduleData->linkNumber[moduleNumber],
                                     moduleData->daisyChainNumber[moduleNumber],
                                     moduleData->vmeBaseAddr[moduleNumber],
                                     &(this->digitizerHandle));
    }
    else if(lType == InputParser::LinkType::USB)
    {
        BOOST_LOG_SEV(lg, Information) << "Opening VME Card Via USB: Digitizer #" << moduleNumber << "\n";
        errVal = CAENComm_OpenDevice(CAENComm_USB,
                                     moduleData->linkNumber[moduleNumber],
                                     0, moduleData->vmeBaseAddr[moduleNumber],
                                     &(this->digitizerHandle));
    }
    else
    {
        BOOST_LOG_SEV(lg, Information) << "Opening Digitizer Directly Via Optical Link: Digitizer #" << moduleNumber << "\n";
        errVal = CAENComm_OpenDevice(CAENComm_OpticalLink,
                                     moduleData->linkNumber[moduleNumber],
                                     moduleData->daisyChainNumber[moduleNumber],
                                     0, &(this->digitizerHandle));
    }
    if (errVal < 0)
    {
        this->writeErrorAndThrow(errVal);
    }
    else
    {
        BOOST_LOG_SEV(lg, Information) << "Successfully Opened Digitizer #" << moduleNumber << "\n";
        digitizerOpen = true;
    }
    //now write all the registers
    this->writeCommonRegisterData();
    this->writeGroupRegisterData();
    this->writeIndividualRegisterData();
}

//puts the digitizer in running mode
void Vx1730Digitizer::startAcquisition()
{
    BOOST_LOG_SEV(lg, Information) << "Starting Acqusition On Digitizer #" << moduleNumber << "\n";
    
    acqRunning = true;
}

//takes the digitizer out of running mode
void Vx1730Digitizer::stopAcquisition()
{
    BOOST_LOG_SEV(lg, Information) << "Stopping Acqusition On Digitizer #" << moduleNumber << "\n";
    
    acqRunning = false;
}

//waits for the digitizer to raise and interrupt about data full.
//once an interrupt is raised, it reads the event size value to find out
//how much it needs to read, then it iteratively reads data that data into
//the given buffer
void Vx1730Digitizer::waitForInterruptToReadData(char* buffer)
{
    
}

//give the max possible size of a buffer in bytes so that they can be pre-
//allocated for the queueing system
int Vx1730Digitizer::getSizeOfReadBufferInBytes()
{
    
}

//log an error and throw an exception to close things
void Vx1730Digitizer::writeErrorAndThrow(CAENComm_ErrorCode errVal)
{
    switch(errVal)
    {
    case CAENComm_Success:
        BOOST_LOG_SEV(lg, Error) << "Digitizer #" << moduleNumber << " - Code: Success\n";
        throw std::runtime_error("Vx1730 Error - Success");
        break;
    case CAENComm_VMEBusError:
        BOOST_LOG_SEV(lg, Error) << "Digitizer #" << moduleNumber << " - Code: VME Bus Error During Cycle\n";
        throw std::runtime_error("Vx1730 Error - VME Bus Error During Cycle");
        break;
    case CAENComm_CommError:
        BOOST_LOG_SEV(lg, Error) << "Digitizer #" << moduleNumber << " - Code: Communication Error\n";
        throw std::runtime_error("Vx1730 Error - Communication Error");
        break;
    case CAENComm_GenericError:
        BOOST_LOG_SEV(lg, Error) << "Digitizer #" << moduleNumber << " - Code: Unspecified Error\n";
        throw std::runtime_error("Vx1730 Error - Unspecified Error");
        break;
    case CAENComm_InvalidParam:
        BOOST_LOG_SEV(lg, Error) << "Digitizer #" << moduleNumber << " - Code: Invalid Parameter\n";
        throw std::runtime_error("Vx1730 Error - Invalid Parameter");
        break;
    case CAENComm_InvalidLinkType:
        BOOST_LOG_SEV(lg, Error) << "Digitizer #" << moduleNumber << " - Code: Invalid Link Type\n";
        throw std::runtime_error("Vx1730 Error - Invalid Link Type");
        break;
    case CAENComm_InvalidHandler:
        BOOST_LOG_SEV(lg, Error) << "Digitizer #" << moduleNumber << " - Code: Invalid Device Handler\n";
        throw std::runtime_error("Vx1730 Error - Invalid Device Handler");
        break;
    case CAENComm_CommTimeout:
        BOOST_LOG_SEV(lg, Error) << "Digitizer #" << moduleNumber << " - Code: Communication Timeout\n";
        throw std::runtime_error("Vx1730 Error - Communication Timeout");
        break;
    case CAENComm_DeviceNotFound:
        BOOST_LOG_SEV(lg, Error) << "Digitizer #" << moduleNumber << " - Code: Unable To Open Requested Device\n";
        throw std::runtime_error("Vx1730 Error - Unable To Open Requested Device");
        break;
    case CAENComm_MaxDevicesError:
        BOOST_LOG_SEV(lg, Error) << "Digitizer #" << moduleNumber << " - Code: Maximum Number of Devices Exceeded\n";
        throw std::runtime_error("Vx1730 Error - Maximum Number of Devices Exceeded");
        break;
    case CAENComm_DeviceAlreadyOpen:
        BOOST_LOG_SEV(lg, Error) << "Digitizer #" << moduleNumber << " - Code: Device Already Open\n";
        throw std::runtime_error("Vx1730 Error - Device Already Open");
        break;
    case CAENComm_NotSupported:
        BOOST_LOG_SEV(lg, Error) << "Digitizer #" << moduleNumber << " - Code: Not Supported Function\n";
        throw std::runtime_error("Vx1730 Error - Not Supported Function");
        break;
    case CAENComm_UnusedBridge:
        BOOST_LOG_SEV(lg, Error) << "Digitizer #" << moduleNumber << " - Code: There Are No Boards Controlled By That Bridge\n";
        throw std::runtime_error("Vx1730 Error - There Are No Boards Controlled By That Bridge");
        break;
    case CAENComm_Terminated:
        BOOST_LOG_SEV(lg, Error) << "Digitizer #" << moduleNumber << " - Code: Communication Terminated By Device\n";
        throw std::runtime_error("Vx1730 Error - nication Terminated By Device");
        break;
    }
}


void Vx1730Digitizer::writeCommonRegisterData()
{
    int numRegToWrite = 29;
    //First create the arrays to write the common register in blocks
    unsigned int* addrArray = new unsigned int[numRegToWrite];
    unsigned int* dataArray = new unsigned int[numRegToWrite];
    
    //set the components of the address and data arrays
    addrArray[0]
    
    
    //create and initialize the error array
    CAENComm_ErrorCode* cycleErrs = new CAENComm_ErrorCode[numRegToWrite];
    for(int i=0; i<numRegToWrite; ++i) cycleErrs[i] = 0;
    
    //call the write
    CAENComm_ErrorCode overallErr = CAENComm_MultiWrite32(this->digitizerHandle,
                                               addrArray, numRegToWrite,
                                               dataArray, cycleErrs);
    //test for errors in the individual cycles
    for(int i=0; i<numRegToWrite; ++i)
    {
        if(cycleErrs[i] < 0)
        {
            BOOST_LOG_SEV(lg, Error) << "Error Writing To Address: 0x" << std::hex << std::setw(4) << std::setfill('0') << addrArray[i] << std::dec;
            this->writeErrorAndThrow(cycleErrs[i]);
        }
    }
    //test for an overall error
    if(overallErr < 0)
    {
        BOOST_LOG_SEV(lg, Error) << "Overall Error In Writing Common Addresses for Digitizer #" << moduleNumber;
        this->writeErrorAndThrow(overallErr);
    }
    
    delete[] addrArray;
    delete[] dataArray;
    delete[] cycleErrs;
}

void Vx1730Digitizer::writeGroupRegisterData()
{
    int numRegToWrite = 4;
    //First create the arrays to write the common register in blocks
    unsigned int* addrArray = new unsigned int[numRegToWrite];
    unsigned int* dataArray = new unsigned int[numRegToWrite];
    
    //set the components of the address and data arrays
    
    //create and initialize the error array
    CAENComm_ErrorCode* cycleErrs = new CAENComm_ErrorCode[numRegToWrite];
    for(int i=0; i<numRegToWrite; ++i) cycleErrs[i] = 0;
    
    //call the write
    CAENComm_ErrorCode overallErr = CAENComm_MultiWrite32(this->digitizerHandle,
                                               addrArray, numRegToWrite,
                                               dataArray, cycleErrs);
    //test for errors in the individual cycles
    for(int i=0; i<numRegToWrite; ++i)
    {
        if(cycleErrs[i] < 0)
        {
            BOOST_LOG_SEV(lg, Error) << "Error Writing To Address: 0x" << std::hex << std::setw(4) << std::setfill('0') << addrArray[i] << std::dec;
            this->writeErrorAndThrow(cycleErrs[i]);
        }
    }
    //test for an overall error
    if(overallErr < 0)
    {
        BOOST_LOG_SEV(lg, Error) << "Overall Error In Writing Common Addresses for Digitizer #" << moduleNumber;
        this->writeErrorAndThrow(overallErr);
    }
    
    delete[] addrArray;
    delete[] dataArray;
    delete[] cycleErrs;
}

void Vx1730Digitizer::writeIndividualRegisterData()
{
    
}

}
