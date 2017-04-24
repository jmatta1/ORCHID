/***************************************************************************//**
********************************************************************************
**
** @author James Till Matta
** @date 24 April, 2017
** @brief Holds the defintion of the singleton class allows write threads to
** access multiple underlying ConcurrentOfstreamWrappers
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
********************************************************************************
*******************************************************************************/
#ifndef SECANT_SRC_SECANT_IO_ASYNCOUTFILE_CONCURRENTFSTREAMCOLLECTION_H
#define SECANT_SRC_SECANT_IO_ASYNCOUTFILE_CONCURRENTFSTREAMCOLLECTION_H
// includes for C system headers
// includes for C++ system headers
// includes from other libraries
// includes from ORCHID
#include"ConcurrentOfstreamWrapper.h"
namespace SECANT
{
namespace IO
{
namespace AsyncFile
{

/**
 * @class ConcurrentOfstreamCollection
 * @ingroup SecantIoModule SecantAsyncFile
 * @brief The ConcurrentOfstreamCollection class maintians a list of ConcurrentOfstreamWrapper objects managed by the async io system
 * @author James Till Matta
 * 
 * This class holds a list of the ConcurrentOfstreamWrapper objects so that they
 * can be retrieved by write threads when needed
 */
class ConcurrentOfstreamCollection
{
public:
    ~ConcurrentOfstreamCollection(){delete[] collection;}
    
    /**
     * @brief Gets a reference to the Wrapper for the specified files
     * @param[in] fileNumber The index of the file whose reference we are to retrieve
     * @return A reference to the appropriate file
     */
    ConcurrentOfstreamWrapper& getFileReference(int fileNumber){return collection[fileNumber];}
    
    /**
     * @brief Gets the number of ConcurrentOfstreamWrappers this class is managing
     * @return The number of ConcurrentOfstreamWrappers we have
     */
    int getNumberFiles(){return numberFiles;}
    
    /**
     * @brief Gets a reference to the global class instance
     * @param[in] numFiles The number of output files to be managed, defaults to 4
     * @return A reference to the global instance of the class
     */
    static ConcurrentOfstreamCollection& getInstance(int numFiles=4)
    {
        static ConcurrentOfstreamCollection cofCollection(numFiles);
        return cofCollection;
    }
private:
    /**
     * @brief Private default constructor to make the ConcurrentOfstreamCollection when needed
     */
    ConcurrentOfstreamCollection(int numFiles);
    
    ConcurrentOfstreamWrapper* collection; ///<The actual collection of file wrappers
    int numberFiles; ///<Stores the number of Wrappers managed by this
    
public: //Deleted methods go in this section because I want the enhanced error
    //messages of public deleted methods, but don't want to 'gum up' my actual
    //function declarations
    /**
     * @brief Deleted copy constructor because this is a singleton
     */
    ConcurrentOfstreamCollection(ConcurrentOfstreamCollection const&) = delete;
    /**
     * @brief Deleted assignment operator because this is a singleton
     */
    void operator=(ConcurrentOfstreamCollection const&) = delete;
};

}
}
}

#endif //SECANT_SRC_SECANT_IO_ASYNCOUTFILE_CONCURRENTFSTREAMCOLLECTION_H
