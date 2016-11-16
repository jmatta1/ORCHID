/***************************************************************************//**
********************************************************************************
**
** @file SecantFileWriter.h
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
** @details Holds the defintion of the class that takes event data and places it
** into the structured and CRCed file structure common to SECANT applications
**
********************************************************************************
*******************************************************************************/
#ifndef SECANT_SRC_IO_TEMPLARFILE_H
#define SECANT_SRC_IO_TEMPLARFILE_H

// includes for C system headers
// includes for C++ system headers
#include<string>
#include<mutex>
// includes from other libraries
// includes from ORCHID
#include"AsyncOutFile.h"
#include"InterThreadComm/Data/FileData.h"

namespace IO
{
//256MB of memory for buffers, approx 2 second of HDD write
enum {BufferCount=128, BufferSizeInBytes=2097152, BufferOverHead=8192, MaxBuffersPerFile=1000};
typedef boost::log::sources::severity_logger_mt<LogSeverity> LoggerType;
typedef boost::lockfree::spsc_queue<char*, boost::lockfree::capacity<BufferCount> > BufferQueue;

class SecantFileWriter
{
public:
    SecantFileWriter(InterThread::FileData* fileDat, LoggerType& logger,
                     int fNumber, const std::string& baseOutputDirectory);
    ~SecantFileWriter();
    //this function handles getting new parameters, it is called by the thread
    //in charge of the particular file when the thread moves into running mode
    void setNewRunParameters(const std::string& runName, int runNumber);
    //used by the outputting thread to write events to the file
    void writeEvent(char* event, int size);
    //used by the outputting thread to tell the class to output any buffered
    //data to file and close the file
    void endRun();
    
private:
    //make certain that a directory exists, or create it if it doesnt and the like
    void prepNewRunFolder();
    //make the file name based on run info
    void buildFileName();
    //construct a file from data
    void makeNewFile();
    //takes a full buffer, and writes the checksums for its contents into the header then sends it to disk
    void finalizeDataBuffer();
    //write buffers of indeterminate size to disk
    void writeBufferToDisk(int bufferSize);
    //this function generates the file header and calls writeBufferToDisk to write it
    void prepAndWriteFileHeader();
    //this function pops a buffer from the buffer queue
    //the length of the buffer queue is deliberately longer than the internal queue used
    //by the async file to move buffers to its write thread. In this way there is always a buffer in the queue
    void getNextBuffer();
    //this function writes the header of the buffer to the current buffer
    void writeBufferHeader();
    
    /*
     * File Data
     */
    //this is the underlying asynchronous file that we use to dump full buffers
    IO::AsyncOutFile<BufferQueue>* outFile;
    //this is the underlying queue full of buffers to store data to be sent to
    //the file when the time comes
    BufferQueue bufferQueue;
    //current buffer to fill
    char* currentBuffer;
    //index into the current buffer
    int buffInd;
    //stores the current buffer number of the file
    int bufferNumber;
    //stores how many events are in the current buffer
    int eventCount;
    //the directory for all data (sub directories hold different runs)
    std::string baseDirectory;
    //the directory that we are currently writing to (outDirectory/runTitle/filename.dat.#)
    std::string writeDirectory;
    //the file name (RunTitle_RunNumber_FileNumber.dat.SequenceNumber)
    std::string fileName;
    //the full file path
    std::string filePath;
    //the run title
    std::string runTitle;
    //stores which file number this is (IE which file writing thread owns us)
    int fileNumber;
    //the run number
    int runNumber;
    //the sequence number for this file
    int sequenceNumber;
    
    //crc32 computer
    boost::crc_optimal<32, 0x32583499, 0x00000000, 0x00000000, false, false> crcComputer;
    
    //statistics variable
    //this is the variable that stores file statistics
    InterThread::FileData* fileData;
    
    //mutex to lock when modifying the filesystem directory structure
    static std::mutex fileSystemLock;
    
    //logger
    LoggerType& lg;
};

}
#endif //ORCHID_SRC_IO_TEMPLARFILE_H
