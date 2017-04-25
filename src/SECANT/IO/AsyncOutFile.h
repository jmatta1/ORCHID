/***************************************************************************//**
********************************************************************************
**
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
** @details Holds the defintion of the reentrant class that is used to write a
** file in a sequential but asynchronous manner.
**
********************************************************************************
*******************************************************************************/
#ifndef SECANT_SRC_IO_ASYNCOUTFILE_H
#define SECANT_SRC_IO_ASYNCOUTFILE_H
// includes for C system headers
// includes for C++ system headers
// includes from other libraries
// includes from ORCHID
#include"AsyncOutFile/WriteMultiQueue.h"
#include"AsyncOutFile/ConcurrentOfstreamCollection.h"
#include"AsyncOutFile/WrapperClearingHouse.h"

namespace SECANT
{
namespace IO
{

/**
 * @class AsyncOutFile
 * @ingroup SecantIoModule SecantAsyncFile
 * @brief Interface through which other code access the AsyncOutFile system
 * @author James Till Matta
 * 
 * This class is an interface to the underlying mechanisms of the asynchronous write
 * system. It exposes only what is necessary for other objects to keep an async
 * ofstream of their own
 */
class AsyncOutFile
{
public:
    /**
     * @brief Constructs an AsyncOutFile, grabbing singleton references as needed
     */
    AsyncOutFile():
        fileIndex(AsyncFile::WrapperClearingHouse::getInstance().getNewFileIndex()),
        writeQueue(AsyncFile::WriteMultiQueue::getInstance()),
        collection(AsyncFile::ConcurrentOfstreamCollection::getInstance())
    {}
    
    /**
     * @brief Changes the files name, blocks until it can acquire the lock
     * @param[in] filePath new file name
     */
    void changeFileName(const std::string& filePath){collection.getFileReference(fileIndex).changeFileName(filePath);}
    
    /**
     * @brief Gets an empty buffer for filling
     * @return Pointer to an empty buffer
     */
    char* getEmptyBuffer(){return writeQueue.popEmptyBuffer();}
    
    /**
     * @brief Returns an empty buffer to the writeQueue so that the queue can dealloc it on deconstruction
     * @param[in] emptyBuffer Pointer to the empty buffer to be returned
     */
    void pushEmptyBuffer(char* emptyBuffer){writeQueue.pushEmptyBuffer(emptyBuffer);}
    
    /**
     * @brief Places a buffer on the queue for writing
     * @param[in] buffer char pointer containing the data to be written
     * @param[in] writeSize Number of bytes to be written
     */
    void performWrite(char* buffer, int writeSize){writeQueue.pushFileWrite(fileIndex, buffer, writeSize);}
    
private:
    int fileIndex; ///<stores the file index for writes using this file
    AsyncFile::WriteMultiQueue& writeQueue; ///<Reference to the write queue through which everything passes
    AsyncFile::ConcurrentOfstreamCollection& collection; ///<Reference to the collection of fstream wrappers
};

}
}
#endif //SECANT_SRC_IO_ASYNCOUTFILE_OUTFILE_H
