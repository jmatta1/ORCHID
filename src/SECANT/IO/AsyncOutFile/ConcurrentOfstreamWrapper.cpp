/***************************************************************************//**
********************************************************************************
**
** @author James Till Matta
** @date 24 April, 2017
** @brief Holds the implementation of the class that wraps fstream with mutexes
** / locks as needed
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
********************************************************************************
*******************************************************************************/
#include"ConcurrentOfstreamWrapper.h"
// includes for C system headers
// includes for C++ system headers
// includes from other libraries
// includes from ORCHID
namespace SECANT
{
namespace IO
{
namespace AsyncFile
{

void ConcurrentOfstreamWrapper::changeFileName(const std::string& filePath)
{
    boost::unique_lock fileLock(fileMutex);
    if(initialized.load())
    {
        file.close();
        file.open(filePath, std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
    }
    else
    {
        file.open(filePath, std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
        initialized.store(true);
    }
    writeCount.store(0);
    fileSize.store(0);
    //fileMutex unlocks on deconstruction of fileLock
}

}
}
}
