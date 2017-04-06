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
** @ingroup SecantData 
** 
********************************************************************************
*******************************************************************************/
#ifndef SECANT_SRC_SECANT_INTERNAL_DATA
#define SECANT_SRC_SECANT_INTERNAL_DATA
// includes for C system headers
// includes for C++ system headers
#include<atomic>
// includes from other libraries
// includes from ORCHID

namespace Secant
{

namespace InterThreadData
{
/*!
 * @class AcquisitionData
 * 
 * @ingroup SecantInterThreadData
 * 
 * @brief This class holds atomic variables to store acquisition data sizes
 * 
 * This class has a few public members to hold raw acquisition data sizes as
 * well as functions to add to those values and to clear those values
 * 
 * @author Author: James Till Matta
 */
struct AcquisitionData
{
    /** @brief AcquisitionData Default Constructor
    * 
    * @param[in] numMods The number of modules we need to keep track of
    * 
    * Create an AcquisitionData object set up for numMods modules
    */
    AcquisitionData(int numMods);
    
    /** @brief AcquisitionData Destructor
    * 
    * Simply delete the array of atomic integers
    */
    ~AcquisitionData(){delete[] dataSizes; delete[] buffers;}

    /** @brief Clears the list of data sizes
     * 
     * Sets all the elements of dataSizes to 0
     */
    void clearData();
    
    /** @brief Increases the amount of data read for moduleNum by amount
     * 
     * @param[in] moduleNum The module number to add to the size of read data for
     * 
     * @param[in] amount The amount of data to add to the size of read data
     * 
     * Increases cell moduleNum of the array dataSizes by amount, there is no safety
     * mechanism to prevent indexing past the end of the array
     */
    void addData(int moduleNum, unsigned amount)
    {
        buffers[moduleNum].fetch_add(1, std::memory_order_relaxed);
        dataSizes[moduleNum].fetch_add(amount, std::memory_order_relaxed);
    }

    /**
     * @brief dataSizes
     * 
     * This array stores the atomic ints that are increased when data is added
     */
    std::atomic_ullong* dataSizes;
    
    /**
     * @brief dataCounts
     * 
     * This array stores the atomic ints that are incremented when data sizes
     * are increased
     */
    std::atomic_uint* buffers;
    
    /**
     * @brief numModules
     * 
     * This integer holds the number of modules
     */
    int numModules;
};

}
}
#endif //ORCHID_SRC_DIGILIB_RATEDATA_H
