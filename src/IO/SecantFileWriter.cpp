/***************************************************************************//**
********************************************************************************
**
** @file SecantFile.cpp
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

#include"TemplarFileWriter.h"
// includes for C system headers
// includes for C++ system headers
#include<ctime>
#include<chrono>
// includes from other libraries
// includes from ORCHID

namespace IO
{

static const std::chrono::system_clock::time_point epoch = std::chrono::system_clock::from_time_t(std::tm{0,0,0,1,0,70,4,0,0});

SecantFileWriter::TemplarFileWriter(InterThread::FileData *fileDat,
                                    LoggerType &logger, int fNumber,
                                    const std::string &baseOutputDirectory):
    fileNumber(fNumber), currentBuffer(nullptr), bufferInd(0), bufferNumber(0),
    eventCount(0), baseDirectory(baseOutputDirectory), writeDirectory(""),
    currentFileName(""), currentRunTitle(""), fileNumber(0), runNumber(0),
    sequenceNumber(0), fileData(fileDat), lg(logger)
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

}
