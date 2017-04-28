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
#ifndef SECANT_SRC_IO_SECANTFILEWRITER_H
#define SECANT_SRC_IO_SECANTFILEWRITER_H

// includes for C system headers
// includes for C++ system headers
#include<string>
#include<mutex>
// includes from other libraries
#include<boost/crc.hpp>
// includes from ORCHID
#include"AsyncOutFile.h"
#include"SECANT/OperationData/FileData.h"
#include"SECANT/Utility/SecantLogger.h"
namespace SECANT
{

namespace IO
{

/**
 * @class SecantFileWriter
 * @ingroup SecantIoModule
 * @brief Class that allows output of binary event data into the SECANT file format
 * @author James Till Matta
 * 
 * This class takes raw incoming event data, and adds it to a buffer with a specially
 * formatted header, if the data to put in the buffer exceeds the space remaining in
 * the buffer, the empty segment of the buffer is filled with zeros, a set of CRC32s
 * are calculated for the data and their checksums are placed in the last part of the
 * header. The buffer is then sent for writing and a new buffer is retrieved
 * 
 * @remarks This class is reentrant. It is completely safe for two threads to access
 * *seperate* instances of this class. If two threads want to access the same
 * instance of this class then access should be guarded through some kind of
 * lock/synchronization
 */
class SecantFileWriter
{
public:
    SecantFileWriter(OperationData::FileData* fileDat, const std::string& fileID,
                     const std::string& baseOutputDirectory);
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
    
    /**
     * @brief Makes sure that base directory exists, if it doesn't it creates it
     */
    void prepBaseDirectory();
    
    /**
     * @brief Creates a folder for a run within the base directory if one does not already exist
     */
    void prepNewRunFolder();
    
    /**
     * @brief Calculates the file name and path from the file naming information
     */
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
    
    
    //File writing machinery
    AsyncOutFile outFile; ///<Asynchronous write system for dumping buffers
    char* currentBuffer; ///<Pointer to the current write buffer we are filling
    int buffInd; ///<index for the first unwritten byte in the buffer
    int bufferNumber; ///<Current buffer number of the file (not counting 4kB file header)
    int eventCount; ///<Number of events written to the current buffer
    
    //File naming information known ahead of time
    std::string baseDirectory; ///<Main directory that holds all sub directories
    std::string fileIdent; ///<Identifier for the file, (something like P00, P01, etc for the processing threads or SCT for the slow controls thread)
    std::string runTitle; ///<The run title
    int runNumber; ///<The run number for the current run
    int sequenceNumber; ///<The sequence number for the file
    //Derived file naming information
    std::string writeDirectory; ///<Full directory path for current writing outDirectory/runTitle
    std::string fileName; ///<The file name RunTitle_FileIdent_RunNumber.dat.SequenceNumber
    std::string filePath; ///<The full file path outDirectory/runTitle/fileName
    OperationData::FileData* fileData; ///<Stored file statistics, path, size, etc
    LoggerType& lg; ///<Reference to the logger
    /**
     * @brief The CRC32 computer object for embedded CRCs
     * 
     * CRC32 computation object, polynomial chosen from https://users.ece.cmu.edu/~koopman/crc/crc32.html
     * notes on how to interpret it can be found at https://users.ece.cmu.edu/~koopman/crc/notes.html
     * the boost crc polynomial demands 1+X notation, the site above gives the polynomial in X+1 notation
     * therefore the 0x32583499 becomes 0x992c1a4c with a full form notation of 0x132583499
     * written out, this polynomial has the form x^32 +x^29 +x^28 +x^25 +x^22 +x^20 +x^19 +x^13 +x^12 +x^10 +x^7 +x^4 +x^3 +1
     * for an input of 32738 bits or less this polynomial misses zero 5 bit and
     * smaller errors of any kind, including within the outputted crc itself, above
     * that input size it starts to miss more kinds of errors, however it is guaranteed
     * to find any 32-bit or less burst error
     */
    boost::crc_optimal<32, 0x32583499, 0x00000000, 0x00000000, false, false> crcComputer;
    
    /**
     * @brief Lock to prevent multiple instances of this class from simultaneously modifying the file system.
     * 
     * It is static so all instances of SecantFileWriter share the same instance of fileSystemLock
     */
    static std::mutex fileSystemLock;
};

}
}
#endif //SECANT_SRC_IO_SECANTFILEWRITER_H
