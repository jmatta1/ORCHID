/***************************************************************************//**
********************************************************************************
**
** @author James Till Matta
** @date 27 Apr, 2016
** @brief Definition for AcquisitionData
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** @details Holds the definition of the class that is used to accumulate read
** sizes from the digitizers
** 
********************************************************************************
*******************************************************************************/
#ifndef SECANT_SRC_SECANT_OPERATIONDATA_ACQUISITIONDATA_H
#define SECANT_SRC_SECANT_OPERATIONDATA_ACQUISITIONDATA_H
// includes for C system headers
// includes for C++ system headers
#include<atomic>
// includes from other libraries
// includes from ORCHID

namespace Secant
{

namespace OperationData
{
/*!
 * @class AcquisitionData
 * @ingroup SecantData
 * @brief This class holds atomic variables to store acquisition information in a threadsafe way
 * @author James Till Matta
 * 
 * This class holds raw acquisition data sizes and buffer counts as well as
 * functions to add to those values and to clear those values. All operation of
 * this class is thread safe, though ordering is not strictly enforced for incrementing
 */
struct AcquisitionData
{
    /**
     * @brief AcquisitionData Default Constructor
     * @param[in] numMods The number of modules we need to keep track of
     * 
     * Create an AcquisitionData object set up for numMods modules
     */
    AcquisitionData(int numMods);
    
    /**
     * @brief AcquisitionData Destructor
     * 
     * Simply delete the arrays of atomic integers
     */
    ~AcquisitionData(){delete[] dataSizes; delete[] bufferCount;}

    /**
     * @brief Clears the list of data sizes
     * 
     * Sets all the elements of dataSizes to 0
     */
    void clearData();
    
    /**
     * @brief Increases the amount of data read for moduleNum by amount
     * @param[in] moduleNum The module number to add to the size of read data for
     * @param[in] amount The amount of data to add to the size of read data
     * 
     * Increases cell moduleNum of the array dataSizes by amount, there is no safety
     * mechanism to prevent indexing past the end of the array
     */
    void addData(int moduleNum, unsigned amount)
    {
        bufferCount[moduleNum].fetch_add(1, std::memory_order_relaxed);
        dataSizes[moduleNum].fetch_add(amount, std::memory_order_relaxed);
    }

    /**
     * @brief Returns the number of modules
     * @return The number of acquisition modules
     */
    int getNumModules(){return numModules;}

    /**
     * @brief Gets the total data read for that module
     * @param[in] moduleNum Which module number you want this data for
     * @return The size in bytes of the data the acquisition system has read
     * since the last call to clearData()
     */
    unsigned long long getDataRead(int moduleNum){return dataSizes[moduleNum].load(std::memory_order_relaxed);}
    
    /** @brief Gets the total number of bufferCount read for that module
     * @param[in] moduleNum Which module number you want this data for
     * @return The number of buffers the acquisition system has read since the
     * last call to clearData()
     */
    unsigned int getNumModules(int moduleNum){return dataSizes[moduleNum].load(std::memory_order_relaxed);}

private:
    std::atomic_ullong* dataSizes; ///<Holds data sizes read from acq sources
    std::atomic_uint* bufferCount; ///<Holds the number of times dataSizes was increased (# of buffers)
    int numModules; ///<This integer holds the number of acquisition modules
};

}
}
#endif //SECANT_SRC_SECANT_OPERATIONDATA_ACQUISITIONDATA_H
