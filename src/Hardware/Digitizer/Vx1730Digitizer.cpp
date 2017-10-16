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
    errVal = CAENComm_Write32(digitizerHandle, Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::AcquisitionCtrl>::value, acqusitionRegister);
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
    errVal = CAENComm_Write32(digitizerHandle, Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::AcquisitionCtrl>::value, acquisitionCtrlRegBase);
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
    this->forceDataFlush();    
    bool eventReady = true;
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
        int bufferSpaceRemaining = (maxSizeOfBoardAggregateBlock - totalSizeRead);
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
    //first calculate the size of a single event, this is calculated by channel pair
    int stopInd = this->channelStartInd + this->numChannel;
    for(int i=channelStartInd; i<stopInd; i+=2)
    {
        int chanPairInd = ((i-channelStartInd)/2);
        sizePerEvent[chanPairInd] = 0;
        //each event has record length * 8 samples
        //2 samples take 1 lword so 8/2 = 4
        if((this->moduleData->recordWaveforms[moduleNumber]))
        {
            sizePerEvent[chanPairInd] = (this->channelData->recordLength[i] * 4);
        }
        //each event has a time trigger tag with 1 lword and the two integrals
        //which also take 1 lword (including the pile up rejection flag in the
        //16th bit of Qshort)
        sizePerEvent[chanPairInd] += 2;
        //determine if the extras word is being written, if it is, add 1lword
        if(this->moduleData->recExtrasWord[i])
        {
            sizePerEvent[chanPairInd] += 1;
        }
    }
    //now calculate the max size of a channel pair data aggregate
    for(int i=channelStartInd; i<stopInd; i+=2)
    {
        int chanPairInd = ((i-channelStartInd)/2);
        //each channel pair aggregate has a 2 lword header
        sizePerChanPairAggregate[chanPairInd] = 2;
        //each channel pair aggregate has up to Number of Events per Aggregate
        sizePerChanPairAggregate[chanPairInd] += (sizePerEvent[chanPairInd] * this->channelData->aggregateEvents[i]);
    }
    //each board aggregate has 4 lwords in the header
    maxSizeOfBoardAggregate = 4;
    //each board aggregate has 0 - 1 channel pair aggregates in it
    for(int i=channelStartInd; i<stopInd; i+=2)
    {
        int chanPairInd = ((i-channelStartInd)/2);
        maxSizeOfBoardAggregate += sizePerChanPairAggregate[chanPairInd];
    }
    maxSizeOfBoardAggregateBlock = (maxSizeOfBoardAggregate * this->moduleData->aggregatesPerBlockTransfer[moduleNumber]);
    maxBufferFillForAnotherRead = (maxSizeOfBoardAggregateBlock - maxSizeOfBoardAggregate);
    //the plus two is because of a quirk of the readout system needing a tiny bit of extra room to ensure we get CAENCOMM terminated
    maxSizeForAllocation = (maxSizeOfBoardAggregateBlock + 2);
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
    addrArray[regCount] = Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::AggregateOrg>::value;
    dataArray[regCount] = (this->moduleData->chanBuffPerAgg[moduleNumber] & 0x0000000F);
    ++regCount;
    addrArray[regCount] = Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::AcquisitionCtrl>::value;
    calculateAcqCtrlRegBase();
    dataArray[regCount] = (acquisitionCtrlRegBase);
    ++regCount;
    addrArray[regCount] = Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::GlobalTrgMask>::value;
    dataArray[regCount] = calculateGlblTrigMaskRegVal();
    ++regCount;
    addrArray[regCount] = Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::TrgOutEnMask>::value;
    dataArray[regCount] = calculateTrigOutEnableMaskRegVal();
    ++regCount;
    addrArray[regCount] = Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::FrontIoCtrl>::value;
    dataArray[regCount] = 0x00000003;//set everything to zero except the bit that makes the lvds high impedance and the bit that
                                       //makes the front lemo connectors TTL logic instead of NIM logic
    ++regCount;
    addrArray[regCount] = Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::ChanEnMask>::value;
    dataArray[regCount] = calculateChanEnMaskRegVal();
    ++regCount;
    addrArray[regCount] = Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::SetMonitorDac>::value;
    dataArray[regCount] = 0x00UL;
    ++regCount;
    addrArray[regCount] = Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::MonitorDacMode>::value;
    dataArray[regCount] = 0x00000000UL;
    ++regCount;
    addrArray[regCount] = Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::MemBuffAlmtFullLvl>::value;
    dataArray[regCount] = (this->moduleData->memBuffAlmostFullLevel[moduleNumber] & 0x000003FF);
    ++regCount;
    addrArray[regCount] = Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::RunStrtStpDelay>::value;
    dataArray[regCount] = (this->moduleData->runStartStopDelay[moduleNumber] & 0xFFFFFFFF);
    ++regCount;
    addrArray[regCount] = Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::DisableExtTrig>::value;
    dataArray[regCount] = ( (this->moduleData->useExtTrigger[moduleNumber]) ? 0x00000000 : 0x00000001);
    ++regCount;
    addrArray[regCount] = Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::FrontLvdsIoNew>::value;
    dataArray[regCount] = 0x00000000;
    ++regCount;
    addrArray[regCount] = Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::ReadoutCtrl>::value;
    dataArray[regCount] = 0x00000098;
    ++regCount;
    addrArray[regCount] = Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::InterruptStatID>::value;
    dataArray[regCount] = (0xFFFFFFFF & moduleNumber);
    ++regCount;
    addrArray[regCount] = Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::InterruptEventNum>::value;
    dataArray[regCount] = (0x000003FF & this->moduleData->interruptEventCount[moduleNumber]);
    ++regCount;
    addrArray[regCount] = Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::AggregateNumPerBlt>::value;
    dataArray[regCount] = (0x000000FF & this->moduleData->aggregatesPerBlockTransfer[moduleNumber]);
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
        addrArray[regCount] = (Vx1730IndivWriteRegistersAddr<Vx1730WriteRegisters::InputDynamicRange>::value +
                               ((i-channelStartInd) * Vx1730IndivWriteRegistersOffs<Vx1730WriteRegisters::InputDynamicRange>::value));
        dataArray[regCount] = (this->channelData->largeRange[i] ? 0x00UL : 0x01UL);
        ++regCount;
        addrArray[regCount] = (Vx1730IndivWriteRegistersAddr<Vx1730WriteRegisters::PreTrg>::value +
                               ((i-channelStartInd) * Vx1730IndivWriteRegistersOffs<Vx1730WriteRegisters::PreTrg>::value));
        dataArray[regCount] = (this->channelData->preTrigger[i] & 0x01FFUL);
        ++regCount;
        addrArray[regCount] = (Vx1730IndivWriteRegistersAddr<Vx1730WriteRegisters::CfdSettings>::value +
                               ((i-channelStartInd) * Vx1730IndivWriteRegistersOffs<Vx1730WriteRegisters::CfdSettings>::value));
        dataArray[regCount] = calculateCfdRegSettings(i);
        ++regCount;
        addrArray[regCount] = (Vx1730IndivWriteRegistersAddr<Vx1730WriteRegisters::ShortGate>::value +
                               ((i-channelStartInd) * Vx1730IndivWriteRegistersOffs<Vx1730WriteRegisters::ShortGate>::value));
        dataArray[regCount] = (this->channelData->shortGate[i] & 0x0FFFUL);
        ++regCount;
        addrArray[regCount] = (Vx1730IndivWriteRegistersAddr<Vx1730WriteRegisters::LongGate>::value +
                               ((i-channelStartInd) * Vx1730IndivWriteRegistersOffs<Vx1730WriteRegisters::LongGate>::value));
        dataArray[regCount] = (this->channelData->longGate[i] & 0x0FFFFUL);
        ++regCount;
        addrArray[regCount] = (Vx1730IndivWriteRegistersAddr<Vx1730WriteRegisters::GateOffset>::value +
                               ((i-channelStartInd) * Vx1730IndivWriteRegistersOffs<Vx1730WriteRegisters::GateOffset>::value));
        dataArray[regCount] = (this->channelData->gateOffset[i] & 0x0FFUL);
        ++regCount;
        addrArray[regCount] = (Vx1730IndivWriteRegistersAddr<Vx1730WriteRegisters::TrgThreshold>::value +
                               ((i-channelStartInd) * Vx1730IndivWriteRegistersOffs<Vx1730WriteRegisters::TrgThreshold>::value));
        dataArray[regCount] = (this->channelData->trigThreshold[i] & 0x3FFFUL);
        ++regCount;
        addrArray[regCount] = (Vx1730IndivWriteRegistersAddr<Vx1730WriteRegisters::FixedBaseline>::value +
                               ((i-channelStartInd) * Vx1730IndivWriteRegistersOffs<Vx1730WriteRegisters::FixedBaseline>::value));
        dataArray[regCount] = (this->channelData->fixedBaseline[i] & 0x3FFFUL);
        ++regCount;
        addrArray[regCount] = (Vx1730IndivWriteRegistersAddr<Vx1730WriteRegisters::ShapedTrgWidth>::value +
                               ((i-channelStartInd) * Vx1730IndivWriteRegistersOffs<Vx1730WriteRegisters::ShapedTrgWidth>::value));
        dataArray[regCount] = (this->channelData->shapedTrigWidth[i] & 0x3FFUL);
        ++regCount;
        addrArray[regCount] = (Vx1730IndivWriteRegistersAddr<Vx1730WriteRegisters::TrgHoldOff>::value +
                               ((i-channelStartInd) * Vx1730IndivWriteRegistersOffs<Vx1730WriteRegisters::TrgHoldOff>::value));
        dataArray[regCount] = (this->channelData->trigHoldOff[i] & 0xFFFFUL);
        ++regCount;
        addrArray[regCount] = (Vx1730IndivWriteRegistersAddr<Vx1730WriteRegisters::PsdCutThreshold>::value +
                               ((i-channelStartInd) * Vx1730IndivWriteRegistersOffs<Vx1730WriteRegisters::PsdCutThreshold>::value));
        dataArray[regCount] = (this->channelData->psdThreshold[i] & 0x3FFUL);
        ++regCount;
        addrArray[regCount] = (Vx1730IndivWriteRegistersAddr<Vx1730WriteRegisters::DppAlgorithmCtrl>::value +
                               ((i-channelStartInd) * Vx1730IndivWriteRegistersOffs<Vx1730WriteRegisters::DppAlgorithmCtrl>::value));
        dataArray[regCount] = calculateDppAlgCtrlRegVal(i);
        ++regCount;
        addrArray[regCount] = (Vx1730IndivWriteRegistersAddr<Vx1730WriteRegisters::DcOffset>::value +
                               ((i-channelStartInd) * Vx1730IndivWriteRegistersOffs<Vx1730WriteRegisters::DcOffset>::value));
        dataArray[regCount] = (this->channelData->dcOffset[i] & 0xFFFFUL);
        ++regCount;
        addrArray[regCount] = (Vx1730IndivWriteRegistersAddr<Vx1730WriteRegisters::VetoExtension>::value +
                               ((i-channelStartInd) * Vx1730IndivWriteRegistersOffs<Vx1730WriteRegisters::VetoExtension>::value));
        dataArray[regCount] = (this->channelData->vetoDurationExtension[i] & 0xFFFFUL);
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

unsigned int Vx1730Digitizer::calculateDppAlgCtrlRegVal(int i)
{
    unsigned int output = 0x00000000;
    output |= (channelData->chargeSensitivity[i] & 0x07UL);
    if(channelData->chargePedestalOn[i])
    {
        output |= 0x00000010;
    }
    output |= ((channelData->dppTriggerCounting[i] & 0x01UL) << 5);
    output |= ((channelData->discMode[i] & 0x01UL) << 6);
    
    output |= ((channelData->pulsePolarity[i] & 0x01UL) << 16);
    output |= ((channelData->trigMode[i] & 0x03UL) << 18);
    output |= ((channelData->baselineMean[i] & 0x07UL) << 20);
    if(channelData->disableSelfTrigger[i])
    {
        output |= 0x01000000;
    }
    //pile up rejection goes here
    if(channelData->psdCutBelowThresh[i])
    {
        output |= 0x08000000;
    }
    if(channelData->psdCutAboveThresh[i])
    {
        output |= 0x10000000;
    }
    if(channelData->overRangeRejection[i])
    {
        output |= 0x20000000;
    }
    if(channelData->triggerHysteresis[i])
    {
        output |= 0x40000000;
    }
    return output;
}

unsigned int Vx1730Digitizer::calculateCfdRegSettings(int i)
{
    unsigned int output = 0x00000000;
    output |= (channelData->cfdDelay[i] & 0x000000FF);
    output |= ((channelData->cfdFraction[i] & 0x03UL) << 8);
    return output;
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
    for(int i=channelStartInd; i<stopInd; ++i)
    {
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
    //enable software trigger output
    output |= 0x80000000;
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
    //to enable software trigger
    output |= 0x80000000;
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

void Vx1730Digitizer::calibrateDigitizer()
{
    using LowLvl::Vx1730WriteRegisters;
    using LowLvl::Vx1730CommonWriteRegistersAddr;
    CAENComm_ErrorCode errVal;
    BOOST_LOG_SEV(lg, Information) << "ACQ Thread: Triggering digitizer self calibration in module #: " << moduleNumber;
    errVal = CAENComm_Write32(digitizerHandle, Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::ChannelCal>::value, 0x00000000);
    if(errVal < 0)
    {
        BOOST_LOG_SEV(lg, Error) << "ACQ Thread: Error Triggering Calibration For Digitizer #" << moduleNumber;
        this->writeErrorAndThrow(errVal);
    }
    //now wait for SPI busy clear and then for calibrated flag to be set
    int stopInd = this->channelStartInd + this->numChannel;
    for(int i=channelStartInd; i<stopInd; ++i)
    {
        //wait for SPI in channel i
        this->waitForChanSpiIdle(i-channelStartInd);
        //then wait for calibrated flag in channel i
        this->waitForChanCal(i-channelStartInd);
    }
    
    BOOST_LOG_SEV(lg, Information) << "ACQ Thread: Pausing for digitizer " << moduleNumber << " self calibration stabilization.";
    boost::this_thread::sleep_for(boost::chrono::seconds(5));
}

void Vx1730Digitizer::waitForChanSpiIdle(unsigned chNum)
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
    errVal = CAENComm_Write32(digitizerHandle, Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::SoftwReset>::value, 0x00000000);
    if(errVal < 0)
    {
        BOOST_LOG_SEV(lg, Error) << "ACQ Thread: Error Resetting Digitizer #" << moduleNumber;
        this->writeErrorAndThrow(errVal);
    }
    //now hit the software clear to blank the data
    this->softwareClear();
    //hit the sync for force the phase-lock loop to realign all clock outputs
    errVal = CAENComm_Write32(digitizerHandle, Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::SoftwClckSync>::value, 0x00000000);
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
