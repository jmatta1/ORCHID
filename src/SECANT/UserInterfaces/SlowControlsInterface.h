/***************************************************************************//**
********************************************************************************
**
** @file SlowControlsInterface.h
** @author James Till Matta
** @date 17 Dec, 2016
** @brief
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** @details Definition file for the slow controls interface that slow controls
** classes must implement in order to be used by the slow controls thread
**
** \defgroup SecantInterfaces Secant User Interfaces
** 
********************************************************************************
*******************************************************************************/
#ifndef SECANT_SRC_THREADS_SLOWCONTROLSTHREADINTERFACE_H
#define SECANT_SRC_THREADS_SLOWCONTROLSTHREADINTERFACE_H


namespace Secant
{

namespace Interfaces
{

/**
 * @class SlowControlsInterface
 * @ingroup SecantInterfaces
 * @brief This class defines the interface for slow controls hardware classes
 * @author James Till Matta
 * 
 * This class provides a common interface that all slow controls hardware classes
 * must present if the user wishes to register them with the slow controls
 * monitoring thread
 * 
 */
class SlowControlsInterface
{
public:
    /**
     * @brief Virtual Destructor
     * 
     * Ensure the correct deletion of derived classes
     */
    virtual ~SlowControlsInterface(){}
    
    /**
     * @brief Gets the max size necessary for the event output buffer
     * @return The maximum size in bytes this class needs for output
     * 
     * This function is used by the slow controls thread to get the maximum
     * amount of space that needs to be allocated by the slow controls thread
     * for writing events produced by this hardware
     */
    virtual unsigned sizeOfOutputInBytes() = 0;
    
    /**
     * @brief Probe the slow controls hardware and write data to the buffer
     * @param[out] outputBuffer This buffer holds the data that is to be output after the query
     * @param[out] readSize This integer holds the number of bytes recieved from the device in the query
     * @return This value should be the total number of bytes used for the write
     * 
     * This function is used by the slow controls thread to query the buffer the
     * slow controls object represents and to write the recieved data into the
     * character buffer passed. The buffer has no garaunteed alignment and will
     * be at least the size of returned by getSizeOfOutputInBytes
     */
    virtual unsigned queryHardwareForData(unsigned char* outputBuffer, unsigned& readSize) = 0;
};

}

}
#endif //SECANT_SRC_THREADS_SLOWCONTROLSTHREADINTERFACE_H
