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
// includes from other libraries
#include<boost/date_time/posix_time/posix_time.hpp>
#define BOOST_FILESYSTEM_NO_DEPRECATED
#include<boost/filesystem.hpp>
#include<boost/crc.hpp>
// includes from ORCHID
#include"Utility/SecantConfig.h"

namespace IO
{

static const boost::posix_time::ptime Epoch(boost::posix_time::time_from_string("1970-01-01 00:00:00.000"));

static const int SecantFileRevisionNumber = 1;
static const unsigned long long int SecantByteOrderMagicNumber = 0x0102040810204080ULL;

SecantFileWriter::SecantFileWriter(InterThread::FileData *fileDat,
                                   LoggerType &logger, int fNumber,
                                   const std::string &baseOutputDirectory):
    fileNumber(fNumber), currentBuffer(nullptr), bufferInd(0), bufferNumber(0),
    eventCount(0), baseDirectory(baseOutputDirectory), writeDirectory(""),
    fileName(""), ritle(""), fileNumber(0), runNumber(0), sequenceNumber(0),
    fileData(fileDat), lg(logger)
{
    //make directories and prep file name
    this->prepNewRunFolder();
    //load the queue with buffers of the appropriate size
    for(int i=0; i<BufferCount ; ++i)
    {
        char* allocBuffer = new (std::nothrow) char[BufferSizeInBytes];
        if(allocBuffer == nullptr)
        {
            BOOST_LOG_SEV(lg, Critical) << "FO Thread: Error In File Write Buffer Allocation";
            throw std::runtime_error("Error In File Write Buffer Allocation");
        }
        //technically this thread should not push, only pop, but I think that it
        //is legal in this case since concurrent use of the queue has no hope of 
        //start until after this loop
        this->bufferQueue.push(allocBuffer);
    }
    //make the asynchronous output file uninitialized
    this->outFile = new IO::AsyncOutFile<BufferQueue>(&(this->bufferQueue));
}

SecantFileWriter::~SecantFileWriter()
{
    //delete the file stream
    delete this->outFile;
    //delete the buffer that we have popped from the bufferqueue to write to
    delete this->currentBuffer;
    char* allocBuffer;
    //pop each of the buffers in the queue and delete them
    while(this->bufferQueue.pop(allocBuffer))
    {
        delete[] allocBuffer;
    }
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

void SecantFileWriter::prepNewRunFolder()
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
            BOOST_LOG_SEV(lg, Critical) << "SecantFileWriter: Could not create data directory in file thread";
            throw std::runtime_error("Could not create data directory in file thread");
        }
    }
    //append the run title to the base output 
    writePath.append(this->runTitle);
    //check if the run directory already exists, if it does, don't try to create it
    if(!boost::filesystem::is_directory(writePath, errCode))
    {
        BOOST_LOG_SEV(lg, Information) << "SecantFileWriter: Preparing new run output subfolder";
        boost::filesystem::create_directories(writePath, errCode);
        if(!boost::filesystem::is_directory(writePath, errCode))
        {
            BOOST_LOG_SEV(lg, Critical) << "SecantFileWriter: Could not create run directory in file thread";
            throw std::runtime_error("Could not create run directory in file thread");
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
    {
        std::ostringstream builder;
        builder << this->currentRunTitle << "_";
        builder << std::setw(4) << std::setfill('0') << this->runNumber << "_";
        builder << std::setw(2) << std::setfill('0') << this->fileNumber << ".dat.";
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
            BOOST_LOG_SEV(lg, Information) << "SecantFileWriter: " << this->currentFileName << ": File already exists, incrementing sequence number";
            ++(this->sequenceNumber);
        }
    }
    BOOST_LOG_SEV(lg, Information) << "SecantFileWriter: New file name is: " << this->currentFileName;
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
    reinterpret_cast<unsigned long long*>(&(this->currentBuffer[this->buffInd]))[0] = 0x0102040810204080ULL;
    this->buffInd += 8;
    reinterpret_cast<unsigned long*>(&(this->currentBuffer[this->buffInd]))[0] = 0x00000001;
    this->buffInd += 4;
    reinterpret_cast<unsigned short*>(&(this->currentBuffer[this->buffInd]))[0] = SECANT_MAJOR_VERSION;
    this->buffInd += 2;
    reinterpret_cast<unsigned short*>(&(this->currentBuffer[this->buffInd]))[0] = SECANT_MINOR_VERSION;
    this->buffInd += 2;
    reinterpret_cast<unsigned short*>(&(this->currentBuffer[this->buffInd]))[0] = SECANT_PATCH_VERSION;
    this->buffInd += 2;
    this->currFileTime = boost::posix_time::microsec_clock::universal_time();
    boost::posix_time::time_duration epochTime = (this->currFileTime - Epoch);
    reinterpret_cast<long long*>(&(this->currentBuffer[this->buffInd]))[0] = epochTime.total_microseconds();
    this->buffInd += 8;
    std::string fileTimeString = boost::posix_time::to_iso_extended_string(boost::posix_time::microsec_clock::local_time());
    int fTimeLength = fileTimeString.size();
    std::copy(fileTimeString.c_str(), fileTimeString.c_str() + fTimeLength, &this->currentBuffer[this->buffInd]);
    this->buffInd += fTimeLength;
    std::fill_n(reinterpret_cast<char*>(&(this->currentBuffer[this->buffInd])), 30-fTimeLength, 0);
    this->buffInd += (30-fTimeLength);
    if(this->runTitle.size() < 100)
    {
        std::copy(this->runTitle.c_str(), this->runTitle.c_str() + this->runTitle.size(), &this->currentBuffer[this->buffInd]);
        this->buffInd += this->runTitle.size();
        std::fill_n(&this->currentBuffer[this->buffInd], 100-this->runTitle.size(), '\0');
        this->buffInd += (100-this->currentRunTitle.size());
    }
    else
    {
        std::copy(this->runTitle.c_str(), this->runTitle.c_str() + 99, &this->currentBuffer[this->buffInd]);
        this->buffInd += 99;
        this->currentBuffer[this->buffInd] = '\0';
        this->buffInd += 1;
    }
    reinterpret_cast<unsigned long*>(&(this->currentBuffer[this->buffInd]))[0] = this->runNumber;
    this->buffInd += 4;
    reinterpret_cast<unsigned long*>(&(this->currentBuffer[this->buffInd]))[0] = this->sequenceNumber;
    this->buffInd += 4;
    //reserve 3924 bytes in the buffer header for whatever
    std::fill_n(reinterpret_cast<unsigned int*>(&this->currentBuffer[this->buffInd]), 981, 0);
    this->buffInd += 3924;
    reinterpret_cast<unsigned long long*>(&(this->currentBuffer[this->buffInd]))[0] = 0xF0F0F0F0F0F0F0F0ULL;
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
    this->currentBuffer = nullptr;
}

}
