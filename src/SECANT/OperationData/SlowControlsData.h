/***************************************************************************//**
********************************************************************************
**
** @author James Till Matta
** @date 27 Apr, 2016
** @brief
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** @details Holds the definition of the struct that stores the data garnished
** from the slow controls like voltage and temperature
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHID_SRC_SECANT_OPERATIONDATA_SLOWCONTROLSDATA_H
#define ORCHID_SRC_SECANT_OPERATIONDATA_SLOWCONTROLSDATA_H

// includes for C system headers
// includes for C++ system headers
#include<atomic>
// includes from other libraries
// includes from ORCHID

namespace Secant
{

namespace OperationData
{

/**
 * @class SlowControlsData
 * @ingroup SecantData
 * @brief This class holds data about the slow controls polling
 * @author James Till Matta
 * 
 * This class holds the sizes of data read by the slow controls system, the time
 * of the latest read in seconds since epoch, and the number of reads of each
 * device the slow controls system is monitoring
 */
class SlowControlsData
{
public:
    /**
     * @brief SlowControlsData Constructor
     * @param[in] numScDevices The number of devices to be polled by the SC thread
     */
    SlowControlsData(int numScDevices);
    
    /** 
     * @brief Destructor, deletes arrays and allows the rest of the object to be destroyed
     */
    ~SlowControlsData(){delete[] readTimes; delete[] readCount; delete[] readSizes;}
    
    /**
     * @brief Sets all read times, counts, and sizes to 0
     */
    void clearData();
    
    /**
     * @brief Sets the time of the last read, increases the read total by size, and increments readCount
     * @param[in] deviceNumber The index of the device for which this applies
     * @param[in] time The time in seconds since the epoch that the read occurred at
     * @param[in] size The total number of bytes read from the device
     */
    void setReadData(int deviceNumber, unsigned long long time, unsigned long long size)
    {
        readTimes[deviceNumber].store(time,std::memory_order_relaxed);
        readCount[deviceNumber].fetch_add(1,std::memory_order_relaxed);
        readSizes[deviceNumber].fetch_add(size,std::memory_order_relaxed);
    }
    
    /**
     * @brief This function retrieves the time of the most recent read for deviceNumber
     * @param[in] deviceNumber The index of the device to get the read time for
     * @return The time in seconds since the Epoch of the last read of the device
     */
    unsigned long long getReadTime(int deviceNumber){return readTimes[deviceNumber].load(std::memory_order_relaxed);}
    
    /**
     * @brief Retrieves the number of bytes read from the device since the last call to clearData()
     * @param[in] deviceNumber The index of the device to get the read time for
     * @return The amount of data in bytes that has been read from the device since the last clear
     */
    unsigned long long getReadSize(int deviceNumber){return readSizes[deviceNumber].load(std::memory_order_relaxed);}
    
    /**
     * @brief Retrieves the number of times the device has been read since the last call to clearData()
     * @param[in] deviceNumber The index of the device to get the read time for
     * @return The number of times the device has been read sinc the last clear
     */
    int getReadCount(int deviceNumber){return readCount[deviceNumber].load(std::memory_order_relaxed);}
    
    /**
     * @brief Returns the number of devices this class was initialized for
     * @return The number of devices
     */
    int getNumDevices(){return numSlowControlsDevices;}
    
private:
    std::atomic_ullong* readTimes; ///<Holds the epoch time of the latest read of the device
    std::atomic_uint* readCount; ///<Holds the number of times the device has been read
    std::atomic_ullong* readSizes; ///<Holds the total amount of data read from the device
    int numSlowControlsDevices; ///<Holds the number of devices the slow controls system reads
};
}
}

#endif //ORCHID_SRC_SECANT_OPERATIONDATA_SLOWCONTROLSDATA_H
