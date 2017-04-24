/***************************************************************************//**
********************************************************************************
**
** @author James Till Matta
** @date 24 April, 2017
** @brief Holds the defintion of the class that wraps fstream with mutexes /
** locks
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
********************************************************************************
*******************************************************************************/
#ifndef SECANT_SRC_SECANT_IO_ASYNCOUTFILE_CONCURRENTFSTREAMWRAPPER_H
#define SECANT_SRC_SECANT_IO_ASYNCOUTFILE_CONCURRENTFSTREAMWRAPPER_H
// includes for C system headers
// includes for C++ system headers
#include<fstream>
#include<atomic>
// includes from other libraries
#include<boost/thread.hpp>
// includes from ORCHID

namespace SECANT
{
namespace IO
{
namespace AsyncFile
{
class ConcurrentOfstreamWrapper
{
public:
    /**
     * @brief Waits for lock of the file, closes the file and opens the new one
     * @param[in] fileNumber the index of the file whose name is to be changed
     * @param[in] filePath a constant reference to the string that contains the path to be written
     * 
     * This function will block the calling thread until the thread can lock the
     * file to change the file name etc. If multiple threads are writing to the
     * same file, you may not like the result if they all try to use this
     * function simultaneously... maybe. This function will also reset the
     * writeCount and fileSize for the file to zero
     */
    void changeFileName(const std::string& filePath);
    
    /**
     * @brief Attempts to acquire a lock on the fileMutex for writes, if you do not already have the lock
     * @return True if the lock was obtained, false otherwise
     * 
     * Unfortunately, as of now, I am not clever enough to make a clever to own
     * the lock for a while, with 'try' behavior and automatic unlocking, so
     * in order to use the unsafeWrite function you need to use this function first
     * and then later call unlock. This exists so that you can do multiple writes
     * to a file and not have to constantly obtain and release a lock. For the
     * individual writes needed for austere mode this is less useful but it does
     * still allow the "Fail if lock is held elsewhere" behavior
     */
    bool tryForLock(){return fileMutex.try_lock();}
    
    /**
     * @brief Release the lock on the fileMutex, if you held it, otherwise undefined
     * 
     * As with tryForLock this function is only intended to be used with the unsafeWrite
     * function. Any other functions in this class will obtain locks on the mutex
     * internally using much safer, unlock on deconstruction objects
     */
    void unlock(){fileMutex.unlock();}
    
    /**
     * @brief Writes the first writeSize bytes out of buffer to the file
     * @param[in] buffer 
     * @param[in] writeSize
     * 
     * This function is just a ofstream.write command which does not lock the
     * mutex nor exercise any other protection for the ofstream. The user is
     * expected to bracket a set of calls to this function with a call to
     * tryForLock that returned true and, afterwards, a call to unlock
     */
    void unsafeWrite(char* buffer, int writeSize);

    int getWriteCount(){return writeCount.load();}
    
    unsigned long long getWrittenSize(){return fileSize.load();}
private:
    bool initialized=false;///<Stores if the file has been opened yet
    std::atomic_int writeCount=0;///<Stores the number of completed writes to the file
    std::atomic_ullong fileSize=0;///<Stores the number of bytes written to the file
    
    boost::mutex fileMutex;///<Used to lock the fstream for usage
    std::ofstream file;///<The actual fstream object
    
};

}
}
}
#endif //SECANT_SRC_SECANT_IO_ASYNCOUTFILE_CONCURRENTFSTREAMWRAPPER_H
