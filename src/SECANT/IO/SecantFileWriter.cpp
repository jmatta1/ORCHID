/***************************************************************************//**
********************************************************************************
**
** @file SecantFileWriter.cpp
** @author James Till Matta
** @date 06 June, 2016
** @brief
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** @details Holds the implementation of the class that takes event data and
** places it into the structured and CRCed file structure common to SECANT
** applications
**
********************************************************************************
*******************************************************************************/

#include"SecantFileWriter.h"
// includes for C system headers
// includes for C++ system headers
#include<ctime>
#include<chrono>
#include<sstream>
#include<cstdlib>
// includes from other libraries
#define BOOST_FILESYSTEM_NO_DEPRECATED
#include<boost/filesystem.hpp>
// includes from ORCHID
#include"SECANT/Utility/SecantConfig.h"
#include"SECANT/Utility/SecantTime.h"
namespace SECANT
{

namespace IO
{

namespace BufferProperties
{
static const unsigned int SizeInBytes=2097152;
static const unsigned int OverHead=SizeInBytes/256;
static const unsigned int MetaDataSize=32;
static const unsigned int CrcStartIndex=OverHead/256;
static const unsigned int CrcInputSize = 1024;
static const unsigned int CrcOutputSize = 4;
static const unsigned int DataSizeIndex = 8;
static const unsigned int EventCountIndex = 12;
static const unsigned int StopTimeIndex = 24;
static const unsigned int SecantBufferStartSeparator = 0xF0F0F0F0;
}

static const unsigned int MaxBuffersPerFile=1000;


namespace HeaderProperties
{
static const unsigned int SecantFileRevisionNumber = 0x00000001;
static const unsigned long long int SecantByteOrderMagicNumber = 0x0102040810204080ULL;
static const unsigned long long int SecantFileHeaderTerminator = 0xF0F0F0F0F0F0F0F0ULL;
}

SecantFileWriter::SecantFileWriter(InterThreadData::FileData *fileDat,
                                   const std::string& fileID,
                                   const std::string& baseOutputDirectory):
    currentBuffer(nullptr), buffInd(0), bufferNumber(0),
    eventCount(0), baseDirectory(baseOutputDirectory), writeDirectory(""),
    fileName(""), filePath(""), runTitle(""), fileIdent(fileID), runNumber(0),
    sequenceNumber(0), fileData(fileDat), lg(SecantLog::get())
{
    //make the base output directory
    this->prepBaseDirectory();
}

SecantFileWriter::~SecantFileWriter()
{
    //push an empty buffer back on to the async file and everything else will destruct itself
    outFile.pushEmptyBuffer(currentBuffer);
}

void SecantFileWriter::setNewRunParameters(const std::string& runName, int runNumber)
{
    //first store the run name and runNumber
    this->runTitle = runName;
    this->runNumber = runNumber;
    //reset the sequence number to 0
    this->sequenceNumber = 0;
    //next prepare the output directory
    this->prepNewRunFolder();
    //build the name of the first file
    this->buildFileName();
    //create the new file
    this->makeNewFile();
    //finally grab a buffer, and prep it for when events start flowing in
    this->getNextBuffer();
    this->writeBufferHeader();
}

void SecantFileWriter::writeEvent(char *event, int size)
{
    //TODO implement compression when we write data to the buffer
    //first check if there is enough space for the event
    if((size + this->buffInd) > BufferProperties::SizeInBytes)
    {//if we are here this is not enough space, finalize the buffer, write it
        //to disk and then see if we need to make a new file
        this->finalizeDataBuffer();
        this->writeBufferToDisk(BufferProperties::SizeInBytes);
        ++(this->bufferNumber);
        if(this->bufferNumber >= MaxBuffersPerFile)
        {//the file now contains whatever the maximum number of data buffers is
            //therefore we need close out this file and make and prep a new file
            this->outFile->closeFile();
            //increment sequence number and set up the new file
            ++(this->sequenceNumber);
            this->buildFileName();
            //create the new file
            this->makeNewFile();
        }
        //if we are here, regardless of if we had to make a new file we need to
        //grab a buffer and write the buffer header to it
        //finally grab a buffer, and prep it for when events start flowing in
        this->getNextBuffer();
        this->writeBufferHeader();
    }
    //if we are here then we can transfer the event to the buffer
    //wether the buffer is new, or the file is new, or whatever, we can transfer
    //the buffer now
    std::copy(event, (event + size), &(this->currentBuffer[this->buffInd]));
    ++(this->eventCount);
    this->buffInd += size;
}

void SecantFileWriter::endRun()
{
    //to close out a run the first thing we do is finalize then write the buffer
    this->finalizeDataBuffer();
    this->writeBufferToDisk(BufferProperties::SizeInBytes);
    //now that the buffer is written tell the asynchronus file to close itself
    this->outFile->closeFile();
}

void SecantFileWriter::prepBaseDirectory()
{
    //make a lock to block the mutex so only one thread hits this function at
    //any given instant
    std::lock_guard<std::mutex> lockFileSystem(this->fileSystemLock);
    
    //make an actual path of the directory string we were given
    boost::filesystem::path writePath(this->baseDirectory);
    boost::system::error_code errCode;
    //check if the base directory already exists, if it does, do not try to create it
    if(!boost::filesystem::is_directory(writePath))
    {
        BOOST_LOG_SEV(lg, Information) << "SecantFileWriter: Preparing new base output folder";
        boost::filesystem::create_directories(writePath, errCode);
        if(!boost::filesystem::is_directory(writePath, errCode))
        {
            BOOST_LOG_SEV(lg, Critical) << "SecantFileWriter: Could not create base output directory";
            std::abort(); //using abort instead of throw may prevent local variable destruction from stack unwinding, making core dumps more useful
            //throw std::runtime_error("Could not create data directory in file thread");
        }
    }
    //the lock releases the mutex on deconstruction
}

void SecantFileWriter::prepNewRunFolder()
{
    //make a lock to block the mutex so only one thread hits this function at
    //any given instant
    std::lock_guard<std::mutex> lockFileSystem(this->fileSystemLock);
    
    //make an actual path of the directory string we were given
    boost::filesystem::path writePath(this->baseDirectory);
    boost::system::error_code errCode;
    //append the run title to the base output directory
    writePath.append(this->runTitle);
    //check if the run directory already exists, if it does, don't try to create it
    if(!boost::filesystem::is_directory(writePath, errCode))
    {
        BOOST_LOG_SEV(lg, Information) << "SecantFileWriter: Preparing new run output subfolder";
        boost::filesystem::create_directories(writePath, errCode);
        if(!boost::filesystem::is_directory(writePath, errCode))
        {
            BOOST_LOG_SEV(lg, Critical) << "SecantFileWriter: Could not create run directory in file thread";
            std::abort(); //using abort instead of throw may prevent local variable destruction from stack unwinding, making core dumps more useful
            //throw std::runtime_error("Could not create run directory in file thread");
        }
    }
    this->writeDirectory = writePath.native();
    BOOST_LOG_SEV(lg, Information) << "SecantFileWriter: Folder at: " << this->writeDirectory << " is prepared.";
    //when the lock guard goes out of scope the mutex is unlocked so all is well
}

void SecantFileWriter::buildFileName()
{
    BOOST_LOG_SEV(lg, Information) << "SecantFileWriter: Starting new file";
    bool buildFlag = true;
    while(buildFlag)
    {//RunTitle_FileIdent_RunNumber_FileNumber.dat.SequenceNumber
        std::ostringstream builder;
        builder << runTitle << "_" << fileIdent << "_";
        builder << std::setw(4) << std::setfill('0') << this->runNumber << ".dat.";
        builder << std::setw(4) << std::setfill('0') << this->sequenceNumber ;
        this->fileName = builder.str();
        boost::filesystem::path fileOutPath(this->writeDirectory);
        fileOutPath.append(this->fileName);
        //check for file existence
        if ( !boost::filesystem::exists( fileOutPath ) )
        {
            buildFlag = false;
            this->filePath = fileOutPath.native();
        }
        else
        {
            BOOST_LOG_SEV(lg, Information) << "SecantFileWriter: " << this->fileName << ": File already exists, incrementing sequence number";
            ++(this->sequenceNumber);
        }
    }
    BOOST_LOG_SEV(lg, Information) << "SecantFileWriter: New file name is: " << this->fileName;
}

void SecantFileWriter::makeNewFile()
{
    //set the file data parameters to match our internal ones
    this->fileData->setFileName(this->filePath);
    this->fileData->setSequenceNumber(this->sequenceNumber);
    this->fileData->setSize(0);
    //open the file and then write the header and such
    this->outFile->newFile(this->filePath);
    this->bufferNumber = 0;
    //write the header for the file
    this->prepAndWriteFileHeader();
}

void SecantFileWriter::prepAndWriteFileHeader()
{
    //if we are here there should be no buffer currently so get one
    this->getNextBuffer();
    //directly manipulate the buffer to contain the header and send it to disk
    //write the 64bit byte order value
    reinterpret_cast<unsigned long long*>(&(this->currentBuffer[this->buffInd]))[0] = HeaderProperties::SecantByteOrderMagicNumber;
    this->buffInd += 8;
    //write the secant file format version
    reinterpret_cast<unsigned int*>(&(this->currentBuffer[this->buffInd]))[0] = HeaderProperties::SecantFileRevisionNumber;
    this->buffInd += 4;
    //write the major version of secant
    reinterpret_cast<unsigned short*>(&(this->currentBuffer[this->buffInd]))[0] = SECANT_MAJOR_VERSION;
    this->buffInd += 2;
    //write the minor version of secant
    reinterpret_cast<unsigned short*>(&(this->currentBuffer[this->buffInd]))[0] = SECANT_MINOR_VERSION;
    this->buffInd += 2;
    //write the patch version of secant
    reinterpret_cast<unsigned short*>(&(this->currentBuffer[this->buffInd]))[0] = SECANT_PATCH_VERSION;
    this->buffInd += 2;
    //write the number of microseconds after the unix epoch when this file header was written
    reinterpret_cast<long long*>(&(this->currentBuffer[this->buffInd]))[0] = Secant::Time::getTimeSinceEpoch();
    this->buffInd += 8;
    //write the ascii string representing the local time when this file was written
    std::string fileTimeString = boost::posix_time::to_iso_extended_string(boost::posix_time::microsec_clock::local_time());
    int fTimeLength = fileTimeString.size();
    std::copy(fileTimeString.c_str(), fileTimeString.c_str() + fTimeLength, &this->currentBuffer[this->buffInd]);
    this->buffInd += fTimeLength;
    //fill any extra characters of the 30 reserved for the time string with 0
    std::fill_n(reinterpret_cast<char*>(&(this->currentBuffer[this->buffInd])), 30-fTimeLength, 0);
    this->buffInd += (30-fTimeLength);
    //write the first 99 characters of the run title string (with the 100th being a null terminator)
    if(this->runTitle.size() < 100)
    {
        std::copy(this->runTitle.c_str(), this->runTitle.c_str() + this->runTitle.size(), &this->currentBuffer[this->buffInd]);
        this->buffInd += this->runTitle.size();
        std::fill_n(&this->currentBuffer[this->buffInd], 100-this->runTitle.size(), '\0');
        this->buffInd += (100-this->runTitle.size());
    }
    else
    {
        std::copy(this->runTitle.c_str(), this->runTitle.c_str() + 99, &this->currentBuffer[this->buffInd]);
        this->buffInd += 99;
        this->currentBuffer[this->buffInd] = '\0';
        this->buffInd += 1;
    }
    //write the run number
    reinterpret_cast<unsigned int*>(&(this->currentBuffer[this->buffInd]))[0] = this->runNumber;
    this->buffInd += 4;
    //write the sequence number of the file
    reinterpret_cast<unsigned int*>(&(this->currentBuffer[this->buffInd]))[0] = this->sequenceNumber;
    this->buffInd += 4;
    //reserve 3924 bytes in the buffer header for whatever
    std::fill_n(reinterpret_cast<unsigned int*>(&this->currentBuffer[this->buffInd]), 981, 0);
    this->buffInd += 3924;
    //write the file header terminator
    reinterpret_cast<unsigned long long*>(&(this->currentBuffer[this->buffInd]))[0] = HeaderProperties::SecantFileHeaderTerminator;
    this->buffInd += 8;
    //send this 4kB header to the file
    this->writeBufferToDisk(this->buffInd);
}

void SecantFileWriter::getNextBuffer()
{
    this->bufferQueue.pop(this->currentBuffer);
    this->buffInd = 0;
}

void SecantFileWriter::writeBufferToDisk(int bufferSize)
{
    this->outFile->writeBuf(this->currentBuffer, bufferSize);
    this->fileData->increaseSize(bufferSize);
    //now we do not 'have' the buffer anymore, so make current buffer nullptr
    this->currentBuffer = nullptr;
    //now there are no events in memory, so make the event count zero
    this->eventCount = 0;
}

void SecantFileWriter::writeBufferHeader()
{
    this->buffInd =0;
    //first we write the buffer separator
    reinterpret_cast<unsigned int*>(&(this->currentBuffer[this->buffInd]))[0] = BufferProperties::SecantBufferStartSeparator;
    this->buffInd += 4;
    //then we write which buffer number this is in the file
    reinterpret_cast<unsigned int*>(&(this->currentBuffer[this->buffInd]))[0] = this->bufferNumber;
    this->buffInd += 4;
    //write a stand in for the number of bytes in the data section of the buffer
    reinterpret_cast<unsigned int*>(&(this->currentBuffer[this->buffInd]))[0] = 0;
    this->buffInd += 4;
    //then we write a stand in for the number of events in the data section of the buffer
    reinterpret_cast<unsigned int*>(&(this->currentBuffer[this->buffInd]))[0] = 0;
    this->buffInd += 4;
    //then we write the time this buffer is started
    reinterpret_cast<long long*>(&(this->currentBuffer[this->buffInd]))[0] = Secant::Time::getTimeSinceEpoch();
    this->buffInd += 8;
    //now we write a stand in for the time the buffer is finished
    reinterpret_cast<long long*>(&(this->currentBuffer[this->buffInd]))[0] = 0;
    this->buffInd += 8;
    //then we write zero to the remainder of the 8kb, since we are aligned to an 8byte boundary go in 8 byte chunks
    std::fill_n(reinterpret_cast<unsigned long long*>(&(this->currentBuffer[this->buffInd])),
                ((BufferProperties::OverHead - BufferProperties::MetaDataSize)/8), 0ULL);
    this->buffInd = BufferProperties::OverHead;
}

void SecantFileWriter::finalizeDataBuffer()
{
    //first we load the last of the buffer with zeros
    std::fill_n(&(this->currentBuffer[this->buffInd]), BufferProperties::SizeInBytes - this->buffInd, 0);
    //here we go back to the beginning of the buffer and write the number of bytes in the data section
    reinterpret_cast<unsigned int*>(&(this->currentBuffer[BufferProperties::DataSizeIndex]))[0] = (this->buffInd - BufferProperties::OverHead);
    //then we write the number of events into the header
    reinterpret_cast<unsigned int*>(&(this->currentBuffer[BufferProperties::EventCountIndex]))[0] = this->eventCount;
    //then we write the time this buffer is finalized
    reinterpret_cast<long long*>(&(this->currentBuffer[BufferProperties::StopTimeIndex]))[0] = Secant::Time::getTimeSinceEpoch();
    //now skip past the first 32 bytes of the buffer and start writing the 4 byte 32 bit crcs
    unsigned int writeInd   = BufferProperties::CrcStartIndex;
    unsigned int readInd    = BufferProperties::OverHead;
    unsigned int blockCount = (BufferProperties::SizeInBytes - BufferProperties::OverHead)/BufferProperties::CrcInputSize;
    //initialize the crc object
    crcComputer.reset();
    //now iterate through the buffer calculating crc 32 values
    for(int i=0; i<blockCount; ++i)
    {
        crcComputer.process_block(&(this->currentBuffer[readInd]),
                                  &(this->currentBuffer[readInd + BufferProperties::CrcInputSize]));
        reinterpret_cast<unsigned int*>(&(this->currentBuffer[writeInd]))[0] = crcComputer.checksum();
        crcComputer.reset();
        writeInd += BufferProperties::CrcOutputSize;
        readInd += BufferProperties::CrcInputSize;
    }
}

}
}
