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
#include<boost/chrono.hpp>
#include<boost/date_time/posix_time/posix_time.hpp>
#include<boost/thread.hpp>
// includes from ORCHID
#include"Vx1730DigitizerRegisters.h"

namespace Digitizer
{
//TODO: Make names associated with the bits we set in registers

enum {MultiRWArraySize = 320};

Vx1730Digitizer::Vx1730Digitizer(int modNum, InputParser::DigitizerModuleData* modData,
                                 InputParser::DigitizerChannelData* chanData) :
    moduleNumber(modNum), channelStartInd(0), numChannel(1), digitizerHandle(0),
    acqRunning(false), digitizerOpen(false), moduleData(modData),
    channelData(chanData), addrArray(NULL), dataArray(NULL),
    cycleErrsArray(NULL), arraySize(MultiRWArraySize), acquisitionCtrlRegBase(0),
    lg(OrchidLog::get())
{
    BOOST_LOG_SEV(lg, Debug)  << moduleData << ", " <<  channelData;
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
    //allocate the multiread and multiwrite arrays
    addrArray = new unsigned int[arraySize];
    dataArray = new unsigned int[arraySize];
    cycleErrsArray = new CAENComm_ErrorCode[arraySize];
    for(int i=0; i<arraySize; ++i)
    {
        addrArray[i] = 0;
        dataArray[i] = 0;
        cycleErrsArray[i] = CAENComm_Success;
    }
}

Vx1730Digitizer::~Vx1730Digitizer()
{
    //stop acquisition if the object controlling the digitizer is being deleted
    //necessary if there is a crash causing stack unwinding
    if (acqRunning)
    {
        this->stopAcquisition();
        acqRunning = false;
    }
    //close the digitizer link as this object is deleted
    if(digitizerOpen)
    {
        CAENComm_CloseDevice(this->digitizerHandle);
        digitizerOpen = false;
    }
    //delete the arrays we allocated
    delete[] addrArray;
    delete[] dataArray;
}

void Vx1730Digitizer::setupDigitizer()
{
    using LowLvl::Vx1730WriteRegisters;
    using LowLvl::Vx1730CommonWriteRegistersAddr;
    
    //check to make sure we have 8 or 16 channels
    if ((numChannel != 8) && (numChannel != 16))
    {
        BOOST_LOG_SEV(lg, Information) << "Error Digitizer #" << moduleNumber << " does not have 8 or 16 channels\n";
        throw std::runtime_error("Vx1730 Error - Wrong Channel Count");
    }
    
    //open the digitizer
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
    
    //now that we have the digitizer opened, hit the software reset.
    CAENComm_Write32(digitizerHandle, Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::SoftwReset>::value, 0x00000001);
    //now hit the software clear to blank the data
    CAENComm_Write32(digitizerHandle, Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::SoftwClear>::value, 0x00000001);
    //now hit the calibration and wait
    CAENComm_Write32(digitizerHandle, Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::ChannelCal>::value, 0x00000001);
    BOOST_LOG_SEV(lg, Information) << "Pausing for digitizer " << moduleNumber << " self calibration stabilization.";
    boost::this_thread::sleep_for(boost::chrono::seconds(5));
    
    
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
    //TODO: Clear the board of any data remaining on it after stopping acquisition
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
    using LowLvl::Vx1730WriteRegisters;
    using LowLvl::Vx1730CommonWriteRegistersAddr;
    int regCount=0;
    //set the components of the address and data arrays
    BOOST_LOG_SEV(lg, Information) << "Writing settings to digitizer # " << moduleNumber << "\n";
    addrArray[regCount] = Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::BoardConfig>::value;
    dataArray[regCount] = calculateBoardConfigRegVal();
    BOOST_LOG_SEV(lg, Information) << std::hex << addrArray[regCount] << " | " << dataArray[regCount] << std::dec << " | " << regCount << "\n";
    ++regCount;
    addrArray[regCount] = Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::AggregateOrg>::value;
    dataArray[regCount] = (this->moduleData->chanBuffPerAgg[moduleNumber] & 0x0000000F);
    BOOST_LOG_SEV(lg, Information) << std::hex << addrArray[regCount] << " | " << dataArray[regCount] << std::dec << " | " << regCount << "\n";
    ++regCount;
    addrArray[regCount] = Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::AcquisitionCtrl>::value;
    calculateAcqCtrlRegBase();
    dataArray[regCount] = (acquisitionCtrlRegBase);
    BOOST_LOG_SEV(lg, Information) << std::hex << addrArray[regCount] << " | " << dataArray[regCount] << std::dec << " | " << regCount << "\n";
    ++regCount;
    addrArray[regCount] = Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::GlobalTrgMask>::value;
    dataArray[regCount] = calculateGlblTrigMaskRegVal();
    BOOST_LOG_SEV(lg, Information) << std::hex << addrArray[regCount] << " | " << dataArray[regCount] << std::dec << " | " << regCount << "\n";
    ++regCount;
    addrArray[regCount] = Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::TrgOutEnMask>::value;
    dataArray[regCount] = calculateTrigOutEnableMaskRegVal();
    BOOST_LOG_SEV(lg, Information) << std::hex << addrArray[regCount] << " | " << dataArray[regCount] << std::dec << " | " << regCount << "\n";
    ++regCount;
    addrArray[regCount] = Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::FrontIoCtrl>::value;
    dataArray[regCount] = 0x00000002;//set everything to zero except the bit that makes the lvds high impedance
    BOOST_LOG_SEV(lg, Information) << std::hex << addrArray[regCount] << " | " << dataArray[regCount] << std::dec << " | " << regCount << "\n";
    ++regCount;
    addrArray[regCount] = Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::ChanEnMask>::value;
    dataArray[regCount] = calculateChanEnMaskRegVal();
    BOOST_LOG_SEV(lg, Information) << std::hex << addrArray[regCount] << " | " << dataArray[regCount] << std::dec << " | " << regCount << "\n";
    ++regCount;
    addrArray[regCount] = Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::SetMonitorDac>::value;
    dataArray[regCount] = 0x00UL;
    BOOST_LOG_SEV(lg, Information) << std::hex << addrArray[regCount] << " | " << dataArray[regCount] << std::dec << " | " << regCount << "\n";
    ++regCount;
    addrArray[regCount] = Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::MonitorDacMode>::value;
    dataArray[regCount] = 0x00000003UL;
    BOOST_LOG_SEV(lg, Information) << std::hex << addrArray[regCount] << " | " << dataArray[regCount] << std::dec << " | " << regCount << "\n";
    ++regCount;
    addrArray[regCount] = Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::MemBuffAlmtFullLvl>::value;
    dataArray[regCount] = (this->moduleData->memBuffAlmostFullLevel[moduleNumber] & 0x000003FF);
    BOOST_LOG_SEV(lg, Information) << std::hex << addrArray[regCount] << " | " << dataArray[regCount] << std::dec << " | " << regCount << "\n";
    ++regCount;
    addrArray[regCount] = Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::RunStrtStpDelay>::value;
    dataArray[regCount] = (this->moduleData->runStartStopDelay[moduleNumber] & 0xFFFFFFFF);
    BOOST_LOG_SEV(lg, Information) << std::hex << addrArray[regCount] << " | " << dataArray[regCount] << std::dec << " | " << regCount << "\n";
    ++regCount;
    addrArray[regCount] = Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::DisableExtTrig>::value;
    dataArray[regCount] = ( (this->moduleData->useExtTrigger[moduleNumber]) ? 0x00000000 : 0x00000001);
    BOOST_LOG_SEV(lg, Information) << std::hex << addrArray[regCount] << " | " << dataArray[regCount] << std::dec << " | " << regCount << "\n";
    ++regCount;
    addrArray[regCount] = Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::DisableExtTrig>::value;
    dataArray[regCount] = ( (this->moduleData->useExtTrigger[moduleNumber]) ? 0x00000000 : 0x00000001);
    BOOST_LOG_SEV(lg, Information) << std::hex << addrArray[regCount] << " | " << dataArray[regCount] << std::dec << " | " << regCount << "\n";
    ++regCount;
    addrArray[regCount] = Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::FrontLvdsIoNew>::value;
    dataArray[regCount] = 0x00000000;
    BOOST_LOG_SEV(lg, Information) << std::hex << addrArray[regCount] << " | " << dataArray[regCount] << std::dec << " | " << regCount << "\n";
    ++regCount;
    addrArray[regCount] = Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::ReadoutCtrl>::value;
    dataArray[regCount] = 0x00000088;
    BOOST_LOG_SEV(lg, Information) << std::hex << addrArray[regCount] << " | " << dataArray[regCount] << std::dec << " | " << regCount << "\n";
    ++regCount;
    addrArray[regCount] = Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::InterruptStatID>::value;
    dataArray[regCount] = (0xFFFFFFFF & moduleNumber);
    BOOST_LOG_SEV(lg, Information) << std::hex << addrArray[regCount] << " | " << dataArray[regCount] << std::dec << " | " << regCount << "\n";
    ++regCount;
    addrArray[regCount] = Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::InterruptEventNum>::value;
    dataArray[regCount] = (0x000003FF & this->moduleData->interruptEventCount[moduleNumber]);
    BOOST_LOG_SEV(lg, Information) << std::hex << addrArray[regCount] << " | " << dataArray[regCount] << std::dec << " | " << regCount << "\n";
    ++regCount;
    addrArray[regCount] = Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::AggregateNumPerBlt>::value;
    dataArray[regCount] = (0x000000FF & this->moduleData->aggregatesPerBlockTransfer[moduleNumber]);
    BOOST_LOG_SEV(lg, Information) << std::hex << addrArray[regCount] << " | " << dataArray[regCount] << std::dec << " | " << regCount << "\n";
    ++regCount;
    
    //TODO: Remove this test/debug code
    BOOST_LOG_SEV(lg, Information) << "Attempting to write to digitizer #" << moduleNumber <<"\n";
    BOOST_LOG_SEV(lg, Information) << "    Addr   |     Data  \n";
    for(int i=0; i<regCount; ++i)
    {
        BOOST_LOG_SEV(lg, Information) << "0x" << std::hex << std::setw(8) << std::setfill('0') << addrArray[i]<< " | 0x" << std::hex << std::setw(8) << std::setfill('0') << dataArray[i] << "\n";
    }
    //END TODO
    //call the write
    CAENComm_ErrorCode overallErr = CAENComm_MultiWrite32(this->digitizerHandle,
                                                          addrArray, regCount,
                                                          dataArray, cycleErrsArray);
    
    //test for errors in the individual cycles
    for(int i=0; i<regCount; ++i)
    {
        if(cycleErrsArray[i] < 0)
        {
            BOOST_LOG_SEV(lg, Error) << "Error Writing To Address: 0x" << std::hex << std::setw(4) << std::setfill('0') << addrArray[i] << std::dec;
            this->writeErrorAndThrow(cycleErrsArray[i]);
        }
    }
    //test for an overall error
    if(overallErr < 0)
    {
        BOOST_LOG_SEV(lg, Error) << "Overall Error In Writing Common Addresses for Digitizer #" << moduleNumber;
        this->writeErrorAndThrow(overallErr);
    }
    //TODO: Remove this test/debug code
    for(int i=0; i<regCount; ++i)
    {
        dataArray[i] = 0x12345678UL;
    }
    overallErr = CAENComm_MultiRead32(this->digitizerHandle,
                                      addrArray, regCount,
                                      dataArray, cycleErrsArray);
    BOOST_LOG_SEV(lg, Information) << "Read from digitizer #" << moduleNumber <<"\n";
    BOOST_LOG_SEV(lg, Information) << "    Addr   |     Data  \n";
    for(int i=0; i<regCount; ++i)
    {
        BOOST_LOG_SEV(lg, Information) << "0x" << std::hex << std::setw(8) << std::setfill('0') << addrArray[i]<< " | 0x" << std::hex << std::setw(8) << std::setfill('0') << dataArray[i] << "\n";
    }
    //test for errors in the individual cycles
    for(int i=0; i<regCount; ++i)
    {
        if(cycleErrsArray[i] < 0)
        {
            BOOST_LOG_SEV(lg, Error) << "Error Writing To Address: 0x" << std::hex << std::setw(4) << std::setfill('0') << addrArray[i] << std::dec;
            this->writeErrorAndThrow(cycleErrsArray[i]);
        }
    }
    //test for an overall error
    if(overallErr < 0)
    {
        BOOST_LOG_SEV(lg, Error) << "Overall Error In Writing Common Addresses for Digitizer #" << moduleNumber;
        this->writeErrorAndThrow(overallErr);
    }
    //END TODO
}

void Vx1730Digitizer::writeGroupRegisterData()
{
    using LowLvl::Vx1730WriteRegisters;
    using LowLvl::Vx1730GroupWriteRegistersAddr;
    using LowLvl::Vx1730GroupWriteRegistersOffs;
    int regCount=0;
    int stopInd = this->channelStartInd + this->numChannel;
    for(int i=channelStartInd; i<stopInd; i+=2)
    {
        addrArray[regCount] = (Vx1730GroupWriteRegistersAddr<Vx1730WriteRegisters::RecordLength>::value +
                               (((i-channelStartInd)/2) * Vx1730GroupWriteRegistersOffs<Vx1730WriteRegisters::RecordLength>::value));
        dataArray[regCount] = (0x0000FFFF & this->channelData->recordLength[i]);
        ++regCount;
        
        addrArray[regCount] = (Vx1730GroupWriteRegistersAddr<Vx1730WriteRegisters::EventsPerAggregate>::value +
                               (((i-channelStartInd)/2) * Vx1730GroupWriteRegistersOffs<Vx1730WriteRegisters::EventsPerAggregate>::value));
        dataArray[regCount] = (0x000003FF & this->channelData->aggregateEvents[i]);
        ++regCount;
        
        addrArray[regCount] = (Vx1730GroupWriteRegistersAddr<Vx1730WriteRegisters::LocalTrgManage>::value +
                               (((i-channelStartInd)/2) * Vx1730GroupWriteRegistersOffs<Vx1730WriteRegisters::LocalTrgManage>::value));
        dataArray[regCount] = calculateLocalTrgManagementRegVal(i);
        ++regCount;
        
        addrArray[regCount] = (Vx1730GroupWriteRegistersAddr<Vx1730WriteRegisters::TriggerValMask>::value +
                               (((i-channelStartInd)/2) * Vx1730GroupWriteRegistersOffs<Vx1730WriteRegisters::TriggerValMask>::value));
        dataArray[regCount] = calculateTriggerValidationMask(i);
        ++regCount;
    }
}

unsigned int Vx1730Digitizer::calculateTriggerValidationMask(int ind)
{
    unsigned int output = 0x00000000;
    
    output |= (this->channelData->triggerValidMask[ind] & 0xFFFFFFFFUL);
    
    return output;
}

unsigned int Vx1730Digitizer::calculateLocalTrgManagementRegVal(int ind)
{
    unsigned int output = 0x00000000;
    if(this->channelData->useLocalShapedTrig[ind])
    {
        output |= 0x00000004;
    }
    
    output |= (this->channelData->localShapedTrigMode[ind] & 0x03UL);
    
    if(this->channelData->useLocalShapedTrig[ind])
    {
        output |= 0x00000040;
    }
    
    output |= ((this->channelData->localTrigValMode[ind] & 0x03UL) << 4);
    
    if(this->channelData->localTrigValAsVeto[ind])
    {
        output |= 0x80UL;
    }
    
    output |= ((this->channelData->extrasWordOptions[ind] & 0x07UL) << 8);
    
    if(this->channelData->smoothIntegration[ind])
    {
        output |= 0x00000800;
    }
    
    output |= ((this->channelData->inputSmoothing[ind] & 0x0FUL) << 12);
    
    return output;
}

unsigned int Vx1730Digitizer::calculateChanEnMaskRegVal()
{
    int stopInd = this->channelStartInd + this->numChannel;
    unsigned int output = 0x00000000;
    BOOST_LOG_SEV(lg, Information) << std::dec << stopInd << " | " << channelStartInd << " | " << numChannel << " | " << channelData->channelEnable.size()<< " | " << channelData->channelNumber.size();
    for(int i=channelStartInd; i<stopInd; ++i)
    {
        BOOST_LOG_SEV(lg, Information) << std::dec << i << " | " << channelData->channelEnable[i] << " | " << channelData->channelNumber[i];
        if(channelData->channelEnable[i])
        {
            output |= (0x01UL << channelData->channelNumber[i]);
        }
    }
    return output;
}

unsigned int Vx1730Digitizer::calculateTrigOutEnableMaskRegVal()
{
    unsigned int output = 0x00000000;
    output |= (0xFFUL & this->moduleData->chanPairTrigOutMask[moduleNumber]);
    output |= ((0x03UL & this->moduleData->trigOutGenerationLogic[moduleNumber]) << 8);
    output |= ((0x07UL & this->moduleData->trigOutMajorityLevel[moduleNumber]) << 10);
    output |= ((0x01UL & this->moduleData->extTrigInTrigOut[moduleNumber]) << 30);
    return output;
}

unsigned int Vx1730Digitizer::calculateGlblTrigMaskRegVal()
{
    unsigned int output = 0x00000000;
    output |= (0xFF & this->moduleData->globalChanPairTrigMask[moduleNumber]);
    output |= ((0xFUL & (unsigned int)this->moduleData->globalCoincidenceWindow[moduleNumber]) << 20);
    output |= ((0x7UL & (unsigned int)this->moduleData->globalMajorityLevel[moduleNumber]) << 24);
    if(this->moduleData->externalTrigger[moduleNumber])
    {
        output |= 0x40000000;
    }
    return output;
}

void Vx1730Digitizer::calculateAcqCtrlRegBase()
{
    acquisitionCtrlRegBase = 0x00000000;
    if(this->moduleData->triggerCountingMethod[moduleNumber])
    {
        acquisitionCtrlRegBase |= 0x00000008;
    }
    if(this->moduleData->memFullMode[moduleNumber])
    {
        acquisitionCtrlRegBase |= 0x00000020;
    }
    if(this->moduleData->pllRefClock[moduleNumber])
    {
        acquisitionCtrlRegBase |= 0x00000040;
    }
}

unsigned int Vx1730Digitizer::calculateBoardConfigRegVal()
{
    //initial value, does not start with zero to reflect required bit sets
    unsigned int output = 0x000C0110;
    if(this->moduleData->enableAutoFlush[moduleNumber])
    {
        output |= 0x00000001;
    }
    if(this->moduleData->propogateTrigs[moduleNumber])
    {
        output |= 0x00000004;
    }
    if(this->moduleData->dualTrace[moduleNumber] == 1)
    {
        output |= 0x00000800;
    }
    if(this->moduleData->analogProbe[moduleNumber] != 0)
    {
        if (this->moduleData->analogProbe[moduleNumber] == 1)
        {
            output |= 0x00001000;
        }
        else //so it is 2
        {
            output |= 0x00002000;
        }
    }
    if(this->moduleData->recordWaveforms[moduleNumber])
    {
        output |= 0x00010000;
    }
    if(this->moduleData->recExtrasWord[moduleNumber])
    {
        output |= 0x00020000;
    }
    if(this->moduleData->digVirtProbe1[moduleNumber] != 0)
    {
        unsigned int temp = this->moduleData->digVirtProbe1[moduleNumber];
        output |= (temp << 23);
    }
    if(this->moduleData->digVirtProbe2[moduleNumber] != 0)
    {
        unsigned int temp = this->moduleData->digVirtProbe1[moduleNumber];
        output |= (temp << 26);
    }
    return output;
}

void Vx1730Digitizer::writeIndividualRegisterData()
{
    
}

}
