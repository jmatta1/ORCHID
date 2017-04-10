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
** @details Holds the definition of the write thread controller
**
********************************************************************************
*******************************************************************************/
#ifndef SECANT_SRC_SECANT_IO_ASYNCOUTFILE_WRITETHREADCONTROL_H
#define SECANT_SRC_SECANT_IO_ASYNCOUTFILE_WRITETHREADCONTROL_H

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
    /**
     * @brief Gets a reference to the global class instance
     * @return A reference to the global instance of the class
     */
    static WriteThreadControl& getInstance()
    {
        //the constructor is only called the first time so things are only initialized once
        static WriteThreadControl ctrl;
        return ctrl;
    }
    
private:
    /**
     * @brief Private default constructor to make the WriteThreadPool when needed
     */
    WriteThreadControl();
    
public://Deleted methods go in this section because I want the enhanced error
    //messages of public deleted methods, but don't want to 'gum up' my actual
    //function declarations
    /**
     * @brief Deleted copy constructor because this is a singleton
     */
    WriteThreadControl(WriteThreadControl const&) = delete;
    /**
     * @brief Deleted assignment operator because this is a singleton
     */
    void operator=(WriteThreadControl const&) = delete;
};


}
}
}
#endif //SECANT_SRC_SECANT_IO_ASYNCOUTFILE_WRITETHREADCONTROL_H
