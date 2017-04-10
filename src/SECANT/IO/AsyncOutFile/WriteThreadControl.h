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
** @details Holds the definition of the write thread controllers
**
********************************************************************************
*******************************************************************************/
#ifndef SECANT_SRC_SECANT_IO_ASYNCOUTFILE_WRITETHREADPOOL_H
#define SECANT_SRC_SECANT_IO_ASYNCOUTFILE_WRITETHREADPOOL_H

// includes for C system headers
// includes for C++ system headers
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

/*!
 * @class WriteThreadControl
 * @ingroup SecantIoModule SecantAsyncFile
 * @brief Control class for the write thread pool
 * @author James Till Matta
 * 
 * This class allows the thread pool to control the write threads
 */
class WriteThreadControl
{
public:

};


}
}
}
#endif //SECANT_SRC_SECANT_IO_ASYNCOUTFILE_WRITETHREADPOOL_H
