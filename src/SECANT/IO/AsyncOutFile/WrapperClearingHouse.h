/***************************************************************************//**
********************************************************************************
**
** @author James Till Matta
** @date 09 April, 2017
** @brief
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** @details Holds a class allows AsyncOutFiles to construct themselves with a
** reference to a unique index of the underlying file
**
********************************************************************************
*******************************************************************************/
#ifndef SECANT_SRC_SECANT_IO_ASYNCOUTFILE_WRAPPERCLEARINGHOUSE_H
#define SECANT_SRC_SECANT_IO_ASYNCOUTFILE_WRAPPERCLEARINGHOUSE_H
// includes for C system headers
// includes for C++ system headers
// includes from other libraries
#include<boost/thread.hpp>
// includes from ORCHID

namespace SECANT
{
namespace IO
{
namespace AsyncFile
{

/**
 * @class WrapperClearingHouse
 * @ingroup SecantIoModule SecantAsyncFile
 * @brief Serializes the AsyncOutFile constructor when it gets its file index
 * @author James Till Matta
 */
class WrapperClearingHouse
{
public:
    
    /**
     * @brief Forces the constructors of AsyncOutFile to serialize in getting which index they will be using
     * @return The index of the ofstream that instance of AsyncOutFile will be using for the rest of its lifetime
     */
    int getNewFileIndex()
    {
        boost::unique_lock lock(grabMutex);
        int retValue = -1;
        if(currFile < numberFiles)
        {
            retValue = currFile;
            ++currFile;
        }   
        return retValue;
    }
    
    /**
     * @brief Gets a reference to the global class instance
     * @param[in] numFiles The number of output files to be managed, defaults to 4
     */
    static WrapperClearingHouse& getInstance(int numFiles=4)
    {
        static WrapperClearingHouse clearingHouse(numFiles);
        return clearingHouse;
    }
private:
    WrapperClearingHouse(int numFiles=4):numberFiles(numFiles){}
    
    boost::mutex grabMutex;
    int currFile=0;
    int numberFiles;
public: //Deleted methods go in this section because I want the enhanced error
    //messages of public deleted methods, but don't want to 'gum up' my actual
    //function declarations
    /**
     * @brief Deleted copy constructor because this is a singleton
     */
    WrapperClearingHouse(WrapperClearingHouse const&) = delete;
    /**
     * @brief Deleted assignment operator because this is a singleton
     */
    void operator=(WrapperClearingHouse const&) = delete;
    
};

}
}
}
#endif //SECANT_SRC_SECANT_IO_ASYNCOUTFILE_WRAPPERCLEARINGHOUSE_H
