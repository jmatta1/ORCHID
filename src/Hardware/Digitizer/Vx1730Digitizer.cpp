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
#include<cstdlib>
// includes from other libraries
#include<boost/chrono.hpp>
#include<boost/date_time/posix_time/posix_time.hpp>
#include<boost/thread.hpp>
// includes from ORCHID

namespace Digitizer
{
//TODO: Make names associated with the bits we set in registers
//TODO: Maybe remove some of the error handling from CAENComm calls, it might be overkill

enum {MultiRWArraySize = 320, IrqTimeoutMs = 5000};

static const int MaxMbltReadSizeInLongWords = 0x200000; //8MB

Vx1730Digitizer::Vx1730Digitizer(int modNum, InputParser::DigitizerModuleData* modData,
                                 InputParser::DigitizerChannelData* chanData) :
    moduleNumber(modNum), channelStartInd(0), numChannel(1), digitizerHandle(0),
    eventsPerInterrupt(0), acqRunning(false), digitizerOpen(false), moduleData(modData),
    channelData(chanData), addrArray(NULL), dataArray(NULL), rdbkArray(NULL),
    cycleErrsArray(NULL), arraySize(MultiRWArraySize), acquisitionCtrlRegBase(0),
    lg(OrchidLog::get())
{
    eventsPerInterrupt = moduleData->interruptEventCount[moduleNumber];
    //find the channel start index
    while(chanData->moduleNumber[channelStartInd] != modNum)
    {
        ++channelStartInd;
    }
    //count the number of channels
    int ind = channelStartInd + 1;
    while(chanData->moduleNumber[ind] == modNum && ind < chanData->moduleNumber.size())
    {
        ++numChannel;
        ++ind;
    }
    //allocate the multiread and multiwrite arrays
    addrArray = new unsigned int[arraySize];
    dataArray = new unsigned int[arraySize];
    rdbkArray = new unsigned int[arraySize];
    cycleErrsArray = new CAENComm_ErrorCode[arraySize];
    for(int i=0; i<arraySize; ++i)
    {
        addrArray[i] = 0;
        dataArray[i] = 0;
        rdbkArray[i] = 0;
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
    delete[] rdbkArray;
    delete[] cycleErrsArray;
}

void Vx1730Digitizer::setupDigitizer()
{
    //check to make sure we have 8 or 16 channels
    if ((numChannel != 8) && (numChannel != 16))
    {
        BOOST_LOG_SEV(lg, Information) << "ACQ Thread: Error Digitizer #" << moduleNumber << " does not have 8 or 16 channels. Seeing instead: " << numChannel <<" number of channels";
        std::abort(); //using abort instead of throw may prevent local variable destruction from stack unwinding, making core dumps more useful
        //throw std::runtime_error("Vx1730 Error - Wrong Channel Count");
    }
    
    //open the digitizer
    this->openDigitizer();
    //clear everything!
    this->clearResetAndSync();
    //now hit the calibration and wait for stabilization
    this->calibrateDigitizer();
    
    //now write all the registers
    this->writeCommonRegisterData();
    this->writeGroupRegisterData();
    this->writeIndividualRegisterData();
    this->calculateMaximumSizes();
}

//puts the digitizer in running mode
void Vx1730Digitizer::startAcquisition()
{
    using LowLvl::Vx1730WriteRegisters;
    using LowLvl::Vx1730CommonWriteRegistersAddr;
    BOOST_LOG_SEV(lg, Information) << "ACQ Thread: Starting/Arming Acqusition On Digitizer #" << moduleNumber ;
    CAENComm_ErrorCode errVal;
    this->enableInterrupts();
    //now hit the software clear to blank the data
    this->softwareClear();
    //now take the acquisition control register base, add bit 2 and write it
    unsigned int acqusitionRegister = (acquisitionCtrlRegBase | 0x4UL);
    errVal = CAENComm_Write32(digitizerHandle, Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::AcqControl>::value, acqusitionRegister);
    if(errVal < 0)
    {
        BOOST_LOG_SEV(lg, Error) << "ACQ Thread: Error Starting Acqusition In Digitizer #" << moduleNumber;
        this->writeErrorAndThrow(errVal);
    }
    BOOST_LOG_SEV(lg, Information) << "ACQ Thread: Digitizer #" << moduleNumber << " Acquisition Started/Armed";
    interuptWaitAttempts = 0;
    interuptTimeouts = 0;
    acqRunning = true;
}

//takes the digitizer out of running mode
void Vx1730Digitizer::stopAcquisition()
{
    using LowLvl::Vx1730WriteRegisters;
    using LowLvl::Vx1730CommonWriteRegistersAddr;
    BOOST_LOG_SEV(lg, Information) << "ACQ Thread: Stopping/Disarming Acqusition On Digitizer #" << moduleNumber;
    //now take the acquisition control register base and write it, it should already have bit[2] == 0
    CAENComm_ErrorCode errVal;
    errVal = CAENComm_Write32(digitizerHandle, Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::AcqControl>::value, acquisitionCtrlRegBase);
    if(errVal < 0)
    {
        BOOST_LOG_SEV(lg, Error) << "ACQ Thread: Error Stopping Acquisition For Digitizer #" << moduleNumber;
        this->writeErrorAndThrow(errVal);
    }
    BOOST_LOG_SEV(lg, Information) << "ACQ Thread: Digitizer #" << moduleNumber << " Acquisition Stopped/Disarmed";
    BOOST_LOG_SEV(lg, Information) << "ACQ Thread: Digitizer #" << moduleNumber << " Interrupt Wait Attempts: "
                                   << interuptWaitAttempts << " Timeouts: " << interuptTimeouts;
    interuptWaitAttempts = 0;
    interuptTimeouts = 0;
    acqRunning = false;
}

unsigned int Vx1730Digitizer::getData(unsigned int* buffer)
{
    using LowLvl::Vx1730ReadRegisters;
    using LowLvl::Vx1730CommonReadRegistersAddr;
    unsigned int readValue = 0;
    bool eventReady = false;
    CAENComm_ErrorCode readError;
    readError = CAENComm_Read32(digitizerHandle, Vx1730CommonReadRegistersAddr<Vx1730ReadRegisters::ReadoutStatus>::value, &readValue);
    if(readError < 0)
    {
        BOOST_LOG_SEV(lg, Error) << "ACQ Thread: Error Reading Event Ready Status Before Waiting for IRQ on Digitizer #" << moduleNumber;
        this->writeErrorAndThrow(readError);
    }
    eventReady = (readValue & 0x00000001);
    if(eventReady)
    {
        return readImpromptuDataAvailable(buffer);
    }
    else
    {
        return waitForInterruptToReadData(buffer);
    }
}

//waits for the digitizer to raise and interrupt about data full.
//once an interrupt is raised, it reads the event size value to find out
//how much it needs to read, then it iteratively reads data that data into
//the given buffer, returns bytes read, which is 0 if we timed out instead of getting a buffer
unsigned int Vx1730Digitizer::waitForInterruptToReadData(unsigned int* buffer)
{
    using LowLvl::Vx1730ReadRegisters;
    using LowLvl::Vx1730CommonReadRegistersAddr;
    //presume acqusition has been started, now wait on an interrupt
    CAENComm_ErrorCode readError;
    ++interuptWaitAttempts;
    this->enableInterrupts();
    readError = CAENComm_IRQWait(digitizerHandle, IrqTimeoutMs);
    if(readError == CAENComm_CommTimeout)
    {//timing out is not an error
        ++interuptTimeouts;
        return 0;
    }
    else if(readError < 0)
    {
        BOOST_LOG_SEV(lg, Error) << "ACQ Thread: Non Timeout Error Waiting for Interrupt Request For Digitizer #" << moduleNumber;
        this->writeErrorAndThrow(readError);
    }
    
    return readInterruptDataAvailable(buffer);
}

//is used after a forcible data flush at the end of acquisition
//returns bytes read
unsigned int Vx1730Digitizer::performFinalReadout(unsigned int* buffer)
{
    bool eventReady = isEventReady();
    unsigned int* bufferEdge = buffer;
    unsigned int dataRead = 0;
    while(eventReady)
    {
        unsigned int amtRead = readEvent(bufferEdge);
        bufferEdge += amtRead;
        dataRead += amtRead;
        eventReady = this->isEventReady();
    }
    return dataRead;
}

unsigned int Vx1730Digitizer::readImpromptuDataAvailable(unsigned int* buffer)
{
    CAENComm_ErrorCode readError;
    bool eventReady = true;
    unsigned int* bufferEdge = buffer;
    unsigned int dataRead = 0;
    while(eventReady && (dataRead < maxBufferFillForAnotherRead))
    {
        unsigned int amtRead = readEvent(bufferEdge);
        bufferEdge += amtRead;
        dataRead += amtRead;
        eventReady = this->isEventReady();
    }
    return dataRead;
}


unsigned int Vx1730Digitizer::readInterruptDataAvailable(unsigned int* buffer)
{
    unsigned int* bufferEdge = buffer;
    unsigned int dataRead = 0;
    int readCount = 0;
    while(readCount < eventsPerInterrupt)
    {
        unsigned int amtRead = readEvent(bufferEdge);
        bufferEdge += amtRead;
        dataRead += amtRead;
        ++readCount;
    }
    return dataRead;
}

unsigned int Vx1730Digitizer::readEvent(unsigned int* buffer)
{
    using LowLvl::Vx1730ReadRegisters;
    using LowLvl::Vx1730CommonReadRegistersAddr;
    int totalSizeRead=0;
    unsigned int* bufferEdge = buffer;
    int sizeReadInMblt = 0;
    //there is no need to read the size of the event, we will get CAENComm_Terminated when we hit the end in the super sized block scheme
    CAENComm_ErrorCode readResult;
    do
    {
        int bufferSpaceRemaining = (maxSizeOfEventBlock - totalSizeRead);
        int readMaxSize = ((MaxMbltReadSizeInLongWords > bufferSpaceRemaining) ? bufferSpaceRemaining : MaxMbltReadSizeInLongWords);
        sizeReadInMblt = 0;
        readResult = CAENComm_MBLTRead(this->digitizerHandle,
                                       Vx1730CommonReadRegistersAddr<Vx1730ReadRegisters::EventReadout>::value,
                                       bufferEdge,
                                       readMaxSize,
                                       &sizeReadInMblt);

        if(readResult == CAENComm_Success || readResult == CAENComm_Terminated)
        {//we are either partway through reading the event or done reading the event
            //either way, we need to update size read, if we are done with the read,
            //it does not matter if we update bufferEdge
            totalSizeRead += sizeReadInMblt;
            bufferEdge += sizeReadInMblt;
            break;
        }
        else
        {
            BOOST_LOG_SEV(lg, Error) << "ACQ Thread: Error in Reading Data From Digitizer #" << moduleNumber;
            this->writeErrorAndThrow(readResult);
        }
    }
    while(readResult != CAENComm_Terminated);
    
    return totalSizeRead;
}

//calculate the max possible size of a buffer in lwords so that they can be pre-
//allocated for the queueing system
void Vx1730Digitizer::calculateMaximumSizes()
{
    //first calculate the size of the traces, essentially the number of active
    //channels times the number of samples in a trace
    int activeChannels = 0;
    int stopInd = this->channelStartInd + this->numChannel;
    for(int i=channelStartInd; i<stopInd; i+=2)
    {
        if(this->channelData->channelEnable[i])
        {
            activeChannels += 1;
        }
    }
    int tracesSize = activeChannels*this->moduleData->samplesPerEvent[moduleNumber]/2; //two samples per lword
    maxSizeOfEvent = tracesSize+4;
    maxSizeOfEventBlock = maxSizeOfEvent*this->moduleData->maxEventsPerBLT[moduleNumber];
    maxBufferFillForAnotherRead = (maxSizeOfEventBlock - maxSizeOfEvent);
    //the plus two is because of a quirk of the readout system needing a tiny bit of extra room to ensure we get CAENCOMM terminated
    maxSizeForAllocation = (maxSizeOfEventBlock + 2);
}

//log an error and throw an exception to close things
void Vx1730Digitizer::writeErrorAndThrow(CAENComm_ErrorCode errVal)
{
    switch(errVal)
    {
    case CAENComm_Success:
        BOOST_LOG_SEV(lg, Error) << "ACQ Thread: Digitizer #" << moduleNumber << " - Code: Success\n";
        std::abort(); //using abort instead of throw may prevent local variable destruction from stack unwinding, making core dumps more useful
        //throw std::runtime_error("Vx1730 Error - Success");
        break;
    case CAENComm_VMEBusError:
        BOOST_LOG_SEV(lg, Error) << "ACQ Thread: Digitizer #" << moduleNumber << " - Code: VME Bus Error During Cycle\n";
        std::abort(); //using abort instead of throw may prevent local variable destruction from stack unwinding, making core dumps more useful
        //throw std::runtime_error("Vx1730 Error - VME Bus Error During Cycle");
        break;
    case CAENComm_CommError:
        BOOST_LOG_SEV(lg, Error) << "ACQ Thread: Digitizer #" << moduleNumber << " - Code: Communication Error\n";
        std::abort(); //using abort instead of throw may prevent local variable destruction from stack unwinding, making core dumps more useful
        //throw std::runtime_error("Vx1730 Error - Communication Error");
        break;
    case CAENComm_GenericError:
        BOOST_LOG_SEV(lg, Error) << "ACQ Thread: Digitizer #" << moduleNumber << " - Code: Unspecified Error\n";
        std::abort(); //using abort instead of throw may prevent local variable destruction from stack unwinding, making core dumps more useful
        //throw std::runtime_error("Vx1730 Error - Unspecified Error");
        break;
    case CAENComm_InvalidParam:
        BOOST_LOG_SEV(lg, Error) << "ACQ Thread: Digitizer #" << moduleNumber << " - Code: Invalid Parameter\n";
        std::abort(); //using abort instead of throw may prevent local variable destruction from stack unwinding, making core dumps more useful
        //throw std::runtime_error("Vx1730 Error - Invalid Parameter");
        break;
    case CAENComm_InvalidLinkType:
        BOOST_LOG_SEV(lg, Error) << "ACQ Thread: Digitizer #" << moduleNumber << " - Code: Invalid Link Type\n";
        std::abort(); //using abort instead of throw may prevent local variable destruction from stack unwinding, making core dumps more useful
        //throw std::runtime_error("Vx1730 Error - Invalid Link Type");
        break;
    case CAENComm_InvalidHandler:
        BOOST_LOG_SEV(lg, Error) << "ACQ Thread: Digitizer #" << moduleNumber << " - Code: Invalid Device Handler\n";
        std::abort(); //using abort instead of throw may prevent local variable destruction from stack unwinding, making core dumps more useful
        //throw std::runtime_error("Vx1730 Error - Invalid Device Handler");
        break;
    case CAENComm_CommTimeout:
        BOOST_LOG_SEV(lg, Error) << "ACQ Thread: Digitizer #" << moduleNumber << " - Code: Communication Timeout\n";
        std::abort(); //using abort instead of throw may prevent local variable destruction from stack unwinding, making core dumps more useful
        //throw std::runtime_error("Vx1730 Error - Communication Timeout");
        break;
    case CAENComm_DeviceNotFound:
        BOOST_LOG_SEV(lg, Error) << "ACQ Thread: Digitizer #" << moduleNumber << " - Code: Unable To Open Requested Device\n";
        std::abort(); //using abort instead of throw may prevent local variable destruction from stack unwinding, making core dumps more useful
        //throw std::runtime_error("Vx1730 Error - Unable To Open Requested Device");
        break;
    case CAENComm_MaxDevicesError:
        BOOST_LOG_SEV(lg, Error) << "ACQ Thread: Digitizer #" << moduleNumber << " - Code: Maximum Number of Devices Exceeded\n";
        std::abort(); //using abort instead of throw may prevent local variable destruction from stack unwinding, making core dumps more useful
        //throw std::runtime_error("Vx1730 Error - Maximum Number of Devices Exceeded");
        break;
    case CAENComm_DeviceAlreadyOpen:
        BOOST_LOG_SEV(lg, Error) << "ACQ Thread: Digitizer #" << moduleNumber << " - Code: Device Already Open\n";
        std::abort(); //using abort instead of throw may prevent local variable destruction from stack unwinding, making core dumps more useful
        //throw std::runtime_error("Vx1730 Error - Device Already Open");
        break;
    case CAENComm_NotSupported:
        BOOST_LOG_SEV(lg, Error) << "ACQ Thread: Digitizer #" << moduleNumber << " - Code: Not Supported Function\n";
        std::abort(); //using abort instead of throw may prevent local variable destruction from stack unwinding, making core dumps more useful
        //throw std::runtime_error("Vx1730 Error - Not Supported Function");
        break;
    case CAENComm_UnusedBridge:
        BOOST_LOG_SEV(lg, Error) << "ACQ Thread: Digitizer #" << moduleNumber << " - Code: There Are No Boards Controlled By That Bridge\n";
        std::abort(); //using abort instead of throw may prevent local variable destruction from stack unwinding, making core dumps more useful
        //throw std::runtime_error("Vx1730 Error - There Are No Boards Controlled By That Bridge");
        break;
    case CAENComm_Terminated:
        BOOST_LOG_SEV(lg, Error) << "ACQ Thread: Digitizer #" << moduleNumber << " - Code: Communication Terminated By Device\n";
        std::abort(); //using abort instead of throw may prevent local variable destruction from stack unwinding, making core dumps more useful
        //throw std::runtime_error("Vx1730 Error - Communication Terminated By Device");
        break;
    }
}


void Vx1730Digitizer::writeCommonRegisterData()
{
    using LowLvl::Vx1730WriteRegisters;
    using LowLvl::Vx1730CommonWriteRegistersAddr;
    int regCount=0;
    //set the components of the address and data arrays
    addrArray[regCount] = Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::BoardConfig>::value;
    dataArray[regCount] = calculateBoardConfigRegVal();
    ++regCount;
    addrArray[regCount] = Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::BufferOrg>::value;
    dataArray[regCount] = calculateBufferOrginization();
    ++regCount;
    addrArray[regCount] = Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::CustomSize>::value;
    int temp = this->moduleData->samplesPerEvent[moduleNumber]/10;
    if(this->moduleData->samplesPerEvent[moduleNumber]%10 != 0) ++temp;
    dataArray[regCount] = temp;
    ++regCount;
    addrArray[regCount] = Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::AcqControl>::value;
    calculateAcqCtrlRegBase();
    dataArray[regCount] = (acquisitionCtrlRegBase);
    ++regCount;
    addrArray[regCount] = Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::GlobalTriggerMask>::value;
    dataArray[regCount] = calculateGlblTrigMaskRegVal();
    ++regCount;
    addrArray[regCount] = Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::PostTrigger>::value;
    dataArray[regCount] = (this->moduleData->numPostTrigSamples[moduleNumber] / 8);
    ++regCount;
    addrArray[regCount] = Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::FrontPanelIoCtrl>::value;
    dataArray[regCount] = 0x00400000; // activates the extended timestamps
    ++regCount;
    addrArray[regCount] = Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::ChannelEnableMask>::value;
    dataArray[regCount] = calculateChanEnMaskRegVal();
    ++regCount;
    addrArray[regCount] = Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::MemoryBufferAlmostFullLevel>::value;
    dataArray[regCount] = 0x0;
    ++regCount;
    addrArray[regCount] = Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::RunStartStopDelay>::value;
    dataArray[regCount] = 0x0;
    ++regCount;
    addrArray[regCount] = Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::ReadoutCtrl>::value;
    dataArray[regCount] = 0x00000098;
    ++regCount;
    addrArray[regCount] = Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::InterruptStatusId>::value;
    dataArray[regCount] = (0xFFFFFFFF & moduleNumber);
    ++regCount;
    addrArray[regCount] = Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::InterruptEventNumber>::value;
    dataArray[regCount] = (0x000003FF & this->moduleData->interruptEventCount[moduleNumber]);
    ++regCount;
    addrArray[regCount] = Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::MaxEventsPerBlt>::value;
    dataArray[regCount] = (0x00000FFF & this->moduleData->maxEventsPerBLT[moduleNumber]);
    ++regCount;

    //call the write
    CAENComm_ErrorCode overallErr = CAENComm_MultiWrite32(this->digitizerHandle,
                                                          addrArray, regCount,
                                                          dataArray, cycleErrsArray);
    
    //test for errors in the individual cycles
    for(int i=0; i<regCount; ++i)
    {
        if(cycleErrsArray[i] < 0)
        {
            BOOST_LOG_SEV(lg, Error) << "ACQ Thread: Error Writing To Address: 0x" << std::hex << std::setw(4) << std::setfill('0') << addrArray[i] << std::dec;
            this->writeErrorAndThrow(cycleErrsArray[i]);
        }
    }
    //test for an overall error
    if(overallErr < 0)
    {
        BOOST_LOG_SEV(lg, Error) << "ACQ Thread: Overall Error In Writing Common Addresses for Digitizer #" << moduleNumber;
        this->writeErrorAndThrow(overallErr);
    }


    //perform a readback to be certain of integrity
    for(int i=0; i<regCount; ++i)
    {
        rdbkArray[i] = 0x12345678UL;
    }
    overallErr = CAENComm_MultiRead32(this->digitizerHandle,
                                      addrArray, regCount,
                                      rdbkArray, cycleErrsArray);
    //test for errors in the individual cycles
    for(int i=0; i<regCount; ++i)
    {
        if(cycleErrsArray[i] < 0)
        {
            BOOST_LOG_SEV(lg, Error) << "ACQ Thread: Error In Read Back From Address: 0x" << std::hex << std::setw(4) << std::setfill('0') << addrArray[i] << std::dec;
            this->writeErrorAndThrow(cycleErrsArray[i]);
        }
    }
    
    //test for an overall error
    if(overallErr < 0)
    {
        BOOST_LOG_SEV(lg, Error) << "ACQ Thread: Overall Error In Readback Of Common Addresses for Digitizer #" << moduleNumber;
        this->writeErrorAndThrow(overallErr);
    }
    
    //give the readback results
    BOOST_LOG_SEV(lg, Information) << "ACQ Thread:Common Register Readback for Digitizer #" << moduleNumber;
    BOOST_LOG_SEV(lg, Information) << "ACQ Thread:   Addr |   Written  |   Read   ";
    for(int i=0; i<regCount; ++i)
    {
        BOOST_LOG_SEV(lg, Information) << "ACQ Thread: 0x" << std::hex << std::setw(4) << std::setfill('0') << addrArray[i] << " | 0x" << std::hex << std::setw(8) << std::setfill('0') << dataArray[i] << " | 0x" << std::hex << std::setw(8) << std::setfill('0') << rdbkArray[i] << std::dec;
    }
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
        addrArray[regCount] = (Vx1730GroupWriteRegistersAddr<Vx1730WriteRegisters::CoupleSelfTriggerLogic>::value +
                               (((i-channelStartInd)/2) * Vx1730GroupWriteRegistersOffs<Vx1730WriteRegisters::CoupleSelfTriggerLogic>::value));
        int temp = this->channelData->coupleTrigLogic[i];
        temp |= ((0x1UL & this->channelData->couplePulseType[i]) << 2);
        dataArray[regCount] = temp;
        ++regCount;
    }
    
    //call the write
    CAENComm_ErrorCode overallErr = CAENComm_MultiWrite32(this->digitizerHandle,
                                                          addrArray, regCount,
                                                          dataArray, cycleErrsArray);
    //test for errors in the individual cycles
    for(int i=0; i<regCount; ++i)
    {
        if(cycleErrsArray[i] < 0)
        {
            BOOST_LOG_SEV(lg, Error) << "ACQ Thread: Error Writing To Address: 0x" << std::hex << std::setw(4) << std::setfill('0') << addrArray[i] << std::dec;
            this->writeErrorAndThrow(cycleErrsArray[i]);
        }
    }
    //test for an overall error
    if(overallErr < 0)
    {
        BOOST_LOG_SEV(lg, Error) << "ACQ Thread: Overall Error In Writing Group Addresses for Digitizer #" << moduleNumber;
        this->writeErrorAndThrow(overallErr);
    }
    
    //perform a readback to be certain of integrity
    for(int i=0; i<regCount; ++i)
    {
        rdbkArray[i] = 0x12345678UL;
    }
    overallErr = CAENComm_MultiRead32(this->digitizerHandle,
                                      addrArray, regCount,
                                      rdbkArray, cycleErrsArray);
    //test for errors in the individual cycles
    for(int i=0; i<regCount; ++i)
    {
        if(cycleErrsArray[i] < 0)
        {
            BOOST_LOG_SEV(lg, Error) << "ACQ Thread: Error In Read Back From Address: 0x" << std::hex << std::setw(4) << std::setfill('0') << addrArray[i] << std::dec;
            this->writeErrorAndThrow(cycleErrsArray[i]);
        }
    }
    
    //test for an overall error
    if(overallErr < 0)
    {
        BOOST_LOG_SEV(lg, Error) << "ACQ Thread: Overall Error In Readback Of Group Addresses for Digitizer #" << moduleNumber;
        this->writeErrorAndThrow(overallErr);
    }
    
    //give the readback results
    BOOST_LOG_SEV(lg, Information) << "ACQ Thread:Group Register Readback for Digitizer #" << moduleNumber;
    BOOST_LOG_SEV(lg, Information) << "ACQ Thread:   Addr |   Written  |   Read   ";
    for(int i=0; i<regCount; ++i)
    {
        BOOST_LOG_SEV(lg, Information) << "ACQ Thread: 0x" << std::hex << std::setw(4) << std::setfill('0') << addrArray[i] << " | 0x" << std::hex << std::setw(8) << std::setfill('0') << dataArray[i] << " | 0x" << std::hex << std::setw(8) << std::setfill('0') << rdbkArray[i] << std::dec;
    }
}

void Vx1730Digitizer::writeIndividualRegisterData()
{
    using LowLvl::Vx1730WriteRegisters;
    using LowLvl::Vx1730IndivWriteRegistersAddr;
    using LowLvl::Vx1730IndivWriteRegistersOffs;
    int regCount=0;
    int stopInd = this->channelStartInd + this->numChannel;
    for(int i=channelStartInd; i<stopInd; ++i)
    {
        addrArray[regCount] = (Vx1730IndivWriteRegistersAddr<Vx1730WriteRegisters::ChannelDynamicRange>::value +
                               ((i-channelStartInd) * Vx1730IndivWriteRegistersOffs<Vx1730WriteRegisters::ChannelDynamicRange>::value));
        dataArray[regCount] = (this->channelData->largeRange[i] ? 0x00UL : 0x01UL);
        ++regCount;
        addrArray[regCount] = (Vx1730IndivWriteRegistersAddr<Vx1730WriteRegisters::ChannelPulseWidth>::value +
                               ((i-channelStartInd) * Vx1730IndivWriteRegistersOffs<Vx1730WriteRegisters::ChannelPulseWidth>::value));
        dataArray[regCount] = (this->channelData->pulseWidth[i] & 0x007FUL);
        ++regCount;
        addrArray[regCount] = (Vx1730IndivWriteRegistersAddr<Vx1730WriteRegisters::ChannelTriggerThreshold>::value +
                               ((i-channelStartInd) * Vx1730IndivWriteRegistersOffs<Vx1730WriteRegisters::ChannelTriggerThreshold>::value));
        dataArray[regCount] = this->channelData->trigThreshold[i] & 0x02FFF;
        ++regCount;
        addrArray[regCount] = (Vx1730IndivWriteRegistersAddr<Vx1730WriteRegisters::ChannelDcOffset>::value +
                               ((i-channelStartInd) * Vx1730IndivWriteRegistersOffs<Vx1730WriteRegisters::ChannelDcOffset>::value));
        dataArray[regCount] = (this->channelData->dcOffset[i] & 0xFFFFUL);
        ++regCount;
    }
    
    //call the write
    CAENComm_ErrorCode overallErr = CAENComm_MultiWrite32(this->digitizerHandle,
                                                          addrArray, regCount,
                                                          dataArray, cycleErrsArray);
    //test for errors in the individual cycles
    for(int i=0; i<regCount; ++i)
    {
        if(cycleErrsArray[i] < 0)
        {
            BOOST_LOG_SEV(lg, Error) << "ACQ Thread: Error Writing To Address: 0x" << std::hex << std::setw(4) << std::setfill('0') << addrArray[i] << std::dec;
            this->writeErrorAndThrow(cycleErrsArray[i]);
        }
    }
    //test for an overall error
    if(overallErr < 0)
    {
        BOOST_LOG_SEV(lg, Error) << "ACQ Thread: Overall Error In Writing Common Addresses for Digitizer #" << moduleNumber;
        this->writeErrorAndThrow(overallErr);
    }
    
    //perform a readback to be certain of integrity
    for(int i=0; i<regCount; ++i)
    {
        rdbkArray[i] = 0x12345678UL;
    }
    overallErr = CAENComm_MultiRead32(this->digitizerHandle,
                                      addrArray, regCount,
                                      rdbkArray, cycleErrsArray);
    //test for errors in the individual cycles
    for(int i=0; i<regCount; ++i)
    {
        if(cycleErrsArray[i] < 0)
        {
            BOOST_LOG_SEV(lg, Error) << "ACQ Thread: Error In Read Back From Address: 0x" << std::hex << std::setw(4) << std::setfill('0') << addrArray[i] << std::dec;
            this->writeErrorAndThrow(cycleErrsArray[i]);
        }
    }
    
    //test for an overall error
    if(overallErr < 0)
    {
        BOOST_LOG_SEV(lg, Error) << "ACQ Thread: Overall Error In Readback Of Group Addresses for Digitizer #" << moduleNumber;
        this->writeErrorAndThrow(overallErr);
    }
    
    //give the readback results
    BOOST_LOG_SEV(lg, Information) << "ACQ Thread:Individual Register Readback for Digitizer #" << moduleNumber;
    BOOST_LOG_SEV(lg, Information) << "ACQ Thread:    Addr   |   Written  |   Read   ";
    for(int i=0; i<regCount; ++i)
    {
        BOOST_LOG_SEV(lg, Information) << "ACQ Thread: 0x" << std::hex << std::setw(8) << std::setfill('0') << addrArray[i] << " | 0x" << std::hex << std::setw(8) << std::setfill('0') << dataArray[i] << " | 0x" << std::hex << std::setw(8) << std::setfill('0') << rdbkArray[i] << std::dec;
    }
}


unsigned int Vx1730Digitizer::calculateChanEnMaskRegVal()
{
    int stopInd = this->channelStartInd + this->numChannel;
    unsigned int output = 0x00000000;
    for(int i=channelStartInd; i<stopInd; ++i)
    {
        if(channelData->channelEnable[i])
        {
            output |= (0x01UL << channelData->channelNumber[i]);
        }
    }
    return output;
}

unsigned int Vx1730Digitizer::calculateGlblTrigMaskRegVal()
{
    unsigned int output = 0x00000000;
    unsigned int trigMask = 0x00000000;
    int stopInd = this->channelStartInd + this->numChannel;
    for(int i=channelStartInd; i<stopInd; ++i)
    {
        if(this->channelData->channelEnable[i])
        {
            trigMask |= (0x1ULL<<(i-channelStartInd));
        }
    }
    output |= (0xFF & trigMask);
    output |= ((0xFUL & (unsigned int)this->moduleData->majorityCoincidenceWindow[moduleNumber]) << 20);
    output |= ((0x7UL & (unsigned int)this->moduleData->majorityLevel[moduleNumber]) << 24);
    //to enable software trigger
    output |= 0x80000000;
    return output;
}

void Vx1730Digitizer::calculateAcqCtrlRegBase()
{
    acquisitionCtrlRegBase = 0x00000000;
    if(this->moduleData->trigCountMode[moduleNumber] == 1)
    {
        acquisitionCtrlRegBase |= 0x00000008;
    }
    if(this->moduleData->memFullMode[moduleNumber] == 1)
    {
        acquisitionCtrlRegBase |= 0x00000020;
    }
}

unsigned int Vx1730Digitizer::calculateBufferOrginization()
{
    unsigned int eventSamples = this->moduleData->samplesPerEvent[moduleNumber];
    if(eventSamples <= 630)
    {
        return 0xA;
    }
    else if(eventSamples <= 1270)
    {
        return 0x9;
    }
    else if(eventSamples <= 2550)
    {
        return 0x8;
    }
    else if(eventSamples <= 5110)
    {
        return 0x7;
    }
    else if(eventSamples <= 10230)
    {
        return 0x6;
    }
    else if(eventSamples <= 20470)
    {
        return 0x5;
    }
    else if(eventSamples <= 40950)
    {
        return 0x4;
    }
    else if(eventSamples <= 81930)
    {
        return 0x3;
    }
    else if(eventSamples <= 163830)
    {
        return 0x2;
    }
    else if(eventSamples <= 327670)
    {
        return 0x1;
    }
    else if(eventSamples <= 655350)
    {
        return 0x0;
    }
    else
    {
        BOOST_LOG_SEV(lg, Information) << "ACQ Thread: Error Digitizer #" << moduleNumber << " can only support traces up to 655350 samples long";
        std::abort(); //using abort instead of throw may prevent local variable destruction from stack unwinding, making core dumps more useful
    }
}

unsigned int Vx1730Digitizer::calculateBoardConfigRegVal()
{
    //initial value, does not start with zero to reflect required bit sets
    unsigned int output = 0x00000010;
    if(this->moduleData->trigOverlapHandling[moduleNumber] == 1)
    {
        output |= 0x00000002;
    }
    if(this->moduleData->enableTestPattern[moduleNumber])
    {
        output |= 0x00000008;
    }
    if(this->moduleData->selfTrigPolarity[moduleNumber] == 1)
    {
        output |= 0x00000040;
    }
    return output;
}

void Vx1730Digitizer::calibrateDigitizer()
{
    using LowLvl::Vx1730WriteRegisters;
    using LowLvl::Vx1730CommonWriteRegistersAddr;
    //first wait for DAC idle
    int stopInd = this->channelStartInd + this->numChannel;
    for(int i=channelStartInd; i<stopInd; ++i)
    {
        //wait for DAC idle in channel i
        this->waitForChanDacIdle(i-channelStartInd);
    }
    CAENComm_ErrorCode errVal;
    BOOST_LOG_SEV(lg, Information) << "ACQ Thread: Triggering digitizer self calibration in module #: " << moduleNumber;
    errVal = CAENComm_Write32(digitizerHandle, Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::ChannelAdcCalibration>::value, 0x00000000);
    if(errVal < 0)
    {
        BOOST_LOG_SEV(lg, Error) << "ACQ Thread: Error Triggering Calibration For Digitizer #" << moduleNumber;
        this->writeErrorAndThrow(errVal);
    }
    //now wait for dac busy busy clear and then for calibrated flag to be set
    for(int i=channelStartInd; i<stopInd; ++i)
    {
        //wait for dac idle in channel i
        this->waitForChanDacIdle(i-channelStartInd);
        //then wait for calibrated flag in channel i
        this->waitForChanCal(i-channelStartInd);
    }
    
    BOOST_LOG_SEV(lg, Information) << "ACQ Thread: Pausing for digitizer " << moduleNumber << " self calibration stabilization.";
    boost::this_thread::sleep_for(boost::chrono::seconds(5));
}

void Vx1730Digitizer::waitForChanDacIdle(unsigned chNum)
{
    using LowLvl::Vx1730ReadRegisters;
    using LowLvl::Vx1730IndivReadRegistersOffs;
    using LowLvl::Vx1730IndivReadRegistersAddr;
    unsigned channelStatusAddress = Vx1730IndivReadRegistersAddr<Vx1730ReadRegisters::ChannelStatus>::value + 
                                    (chNum * Vx1730IndivReadRegistersOffs<Vx1730ReadRegisters::ChannelStatus>::value);
    const short timeoutInMilliSeconds = 800;
    for(int i = 0; i<timeoutInMilliSeconds; ++i)
    {
        CAENComm_ErrorCode errVal;
        unsigned int retValue;
        errVal = CAENComm_Read32(digitizerHandle, channelStatusAddress, &retValue);
        if(errVal < 0)
        {
            BOOST_LOG_SEV(lg, Error) << "ACQ Thread: Error waiting for non busy SPI bus on digitizer #" << moduleNumber;
            this->writeErrorAndThrow(errVal);
        }
        //if the channel status register says that the SPI bus is still busy, keep going
        if (!(retValue & 0x00000004))
        {
            return;
        }
        boost::this_thread::sleep_for(boost::chrono::milliseconds(1));
    }
    //if we have made it to here then we have timed out
    BOOST_LOG_SEV(lg, Error) << "ACQ Thread: Timeout waiting for non busy SPI bus on digitizer #" << moduleNumber;
    throw std::runtime_error("Vx1730 Error - Timeout waiting for non busy SPI bus");
}

void Vx1730Digitizer::waitForChanCal(unsigned chNum)
{
    using LowLvl::Vx1730ReadRegisters;
    using LowLvl::Vx1730IndivReadRegistersOffs;
    using LowLvl::Vx1730IndivReadRegistersAddr;
    unsigned channelStatusAddress = Vx1730IndivReadRegistersAddr<Vx1730ReadRegisters::ChannelStatus>::value + 
                                    (chNum * Vx1730IndivReadRegistersOffs<Vx1730ReadRegisters::ChannelStatus>::value);
    const short timeoutInMilliSeconds = 800;
    bool wasCal = false;
    for(int i = 0; i<timeoutInMilliSeconds; ++i)
    {
        CAENComm_ErrorCode errVal;
        unsigned int retValue;
        errVal = CAENComm_Read32(digitizerHandle, channelStatusAddress, &retValue);
        if(errVal < 0)
        {
            BOOST_LOG_SEV(lg, Error) << "ACQ Thread: Error waiting for non busy SPI bus on digitizer #" << moduleNumber;
            this->writeErrorAndThrow(errVal);
        }
        //if the channel status register says that we are calibrated we are done
        if (retValue & 0x00000008)
        {
            return;
        }
        boost::this_thread::sleep_for(boost::chrono::milliseconds(1));
    }
    //if we have made it to here then we have timed out
    BOOST_LOG_SEV(lg, Error) << "ACQ Thread: Timeout waiting for calibration of channel"<<chNum<<" on digitizer #" << moduleNumber;
    throw std::runtime_error("Vx1730 Error - Timeout waiting for channel calibration");
}

void Vx1730Digitizer::clearResetAndSync()
{
    using LowLvl::Vx1730WriteRegisters;
    using LowLvl::Vx1730CommonWriteRegistersAddr;
    CAENComm_ErrorCode errVal;
    //hit the software reset to force the registers to default values
    errVal = CAENComm_Write32(digitizerHandle, Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::SoftwareReset>::value, 0x00000000);
    if(errVal < 0)
    {
        BOOST_LOG_SEV(lg, Error) << "ACQ Thread: Error Resetting Digitizer #" << moduleNumber;
        this->writeErrorAndThrow(errVal);
    }
    //now hit the software clear to blank the data
    this->softwareClear();
    //hit the sync for force the phase-lock loop to realign all clock outputs
    errVal = CAENComm_Write32(digitizerHandle, Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::SoftwareClockSync>::value, 0x00000000);
    if(errVal < 0)
    {
        BOOST_LOG_SEV(lg, Error) << "ACQ Thread: Error Forcing Clock Sync #" << moduleNumber;
        this->writeErrorAndThrow(errVal);
    }
}

void Vx1730Digitizer::openDigitizer()
{
    using LowLvl::Vx1730WriteRegisters;
    using LowLvl::Vx1730CommonWriteRegistersAddr;
    CAENComm_ErrorCode errVal;
    InputParser::LinkType lType = moduleData->linkType[moduleNumber];
    if(lType == InputParser::LinkType::DirectOptical)
    {
        BOOST_LOG_SEV(lg, Information) << "ACQ Thread: Opening Digitizer Directly Via Optical Link: Digitizer #" << moduleNumber;
        errVal = CAENComm_OpenDevice(CAENComm_OpticalLink,
                                     moduleData->linkNumber[moduleNumber],
                                     moduleData->daisyChainNumber[moduleNumber],
                                     0, &(this->digitizerHandle));
    }
    else
    {
        BOOST_LOG_SEV(lg, Information) << "ACQ Thread: No support for non-optical link, digitizers";
        throw std::runtime_error("Vx1730 Error - Unsupported Connection Type");
    }
    /*else if(lType == InputParser::LinkType::USB)
    {
        BOOST_LOG_SEV(lg, Information) << "ACQ Thread: Opening VME Card Via USB: Digitizer #" << moduleNumber;
        errVal = CAENComm_OpenDevice(CAENComm_USB,
                                     moduleData->linkNumber[moduleNumber],
                                     0, moduleData->vmeBaseAddr[moduleNumber],
                                     &(this->digitizerHandle));
    }
    else
    {
        BOOST_LOG_SEV(lg, Information) << "ACQ Thread: Opening VME Card Via Optical Link, Digitizer #" << moduleNumber;
        errVal = CAENComm_OpenDevice(CAENComm_OpticalLink,
                                     moduleData->linkNumber[moduleNumber],
                                     moduleData->daisyChainNumber[moduleNumber],
                                     moduleData->vmeBaseAddr[moduleNumber],
                                     &(this->digitizerHandle));
    }*/
    //check for an error in opening the digitizer
    if (errVal < 0)
    {
        this->writeErrorAndThrow(errVal);
    }
    else
    {
        BOOST_LOG_SEV(lg, Information) << "ACQ Thread: Successfully Opened Digitizer #" << moduleNumber;
        digitizerOpen = true;
    }
}

}
