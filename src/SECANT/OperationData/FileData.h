/***************************************************************************//**
********************************************************************************
**
** @file FileInfo.h
** @author James Till Matta
** @date 26 May, 2016
** @brief
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** @details Holds the definition of the class that is used to show file data to
** the UI thread
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHID_SRC_FILE_FILEINFO_H
#define ORCHID_SRC_FILE_FILEINFO_H

// includes for C system headers
// includes for C++ system headers
#include<string>
#include<atomic>
// includes from other libraries
#include<boost/thread.hpp>
// includes from ORCHID
namespace Secant
{

namespace OperationData
{

/*!
 * @class FileData
 * 
 * @ingroup SecantData
 * 
 * @brief This class holds file information in a thread safe way
 * 
 * This class has a few public members to hold raw acquisition data sizes as
 * well as functions to add to those values and to clear those values
 * 
 * @author Author: James Till Matta
 */
class FileData
{
public:
    /** @brief FileData Default Constructor
    * 
    * Create an FileData object for a single SECANT file
    */
    FileData();

    /** @brief FileData Destructor
    */
    ~FileData(){}
    
    /** @brief Sets the path of the file name
     * 
     * @param[in] fName New file path to set
     * 
     * Locks the string mutex with a unique lock (Writer's Lock), sets the path
     * then unlocks the mutex
     */
    void setFileName(const std::string& fName);
    
    /** @brief Sets the sequence number of the file
     * 
     * @param[in] sn New sequence number to be set
     * 
     * Sets the file sequence number so it can be determined without parsing the file path
     */
    void setSequenceNumber(int sn){sequenceNumber.store(sn);}
    
    /** @brief Sets the size of the file
     * 
     * @param[in] s New size to be set
     * 
     * Sets the file size in bytes
     */
    void setSize(long long s){size.store(s);}
    
    /** @brief Increments the sequence number
     */
    void incrementSequenceNumber(){++sequenceNumber;}
    
    /** @brief Increases the sequence number by a set amount
     * 
     * @param[in] diff The amount to increase the sequence number by
     * 
     * Increases the sequence number by diff
     */
    void increaseSequenceNumber(int diff){sequenceNumber.fetch_add(diff);}
    
    /** @brief Increases the size by a set amount
     * 
     * @param[in] diff The amount to increase the size by
     * 
     * Increases the stored file size by by diff
     */
    void increaseSize(long long diff){size.fetch_add(diff);}
    
    /** @brief Retrieves the sequence number
     * 
     * @return The sequence number of the file whose information is stored
     */
    int getSequenceNumber(){return sequenceNumber.load();}
    
    /** @brief Retrieves the file size in bytes
     * 
     * @return The size in bytes of the file whose information is stored
     */
    long long getSize(){return size.load();}
    
    /** @brief Retrieves the file path
     * 
     * @param[out] fName The string to store the file path into
     * 
     * Acquires a shared lock (reader's lock) on the mutex, copies the file path
     * into fName and then releases the lock
     */
    void getFileName(std::string& fName);
    
    
    //tester function
    //why repeatedly lock and unlock mutexes when we can simply check
    //an atomic boolean and see if it is true or not
    //these should not change often so testing the boolean is efficient

    /** @brief Checks if the file name has been modified since the last time it was checked with getFileName
     * 
     * @return True if the name has changed since the last use of getFileName, else False
     */
    bool fileNameChangeSinceLastGet(){return fileNameTest.load();}
    
private:
    //access guard for strings since they are not atomic
    /**
     * @brief Used to provide a "Read/Write Lock" for access to fileName
     */
    boost::shared_mutex nonAtomicAccess;
    /**
     * @brief Stores the path of the file
     */
    std::string fileName;
    
    /**
     * @brief Stores the sequence number of the file
     */
    std::atomic_int sequenceNumber;
    
    /**
     * @brief Stores the size of the file in bytes
     */
    std::atomic_llong size;
    
    /**
     * @brief Have modified the file name since it was last checked with getFileName
     */
    std::atomic_bool fileNameTest;
};

}
}
#endif //ORCHID_SRC_FILE_FILEINFO_H
