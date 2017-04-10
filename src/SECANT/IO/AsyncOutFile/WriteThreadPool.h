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
** @details Holds the defintion of the singleton class that coordinates the pool
** of threads that perform file writes. Also holds the write mode enumeration
**
********************************************************************************
*******************************************************************************/
#ifndef SECANT_SRC_SECANT_IO_ASYNCOUTFILE_WRITETHREADPOOL_H
#define SECANT_SRC_SECANT_IO_ASYNCOUTFILE_WRITETHREADPOOL_H
namespace SECANT
{
namespace IO
{
namespace AsyncFile
{
/**
 * @ingroup SecantIoModule SecantAsyncFile
 * @brief Represents the possible states of write thread pool
 * 
 * There are four possible situations for the asynchronous file writing system
 * in three of them WriteThreadPoolMode::Greedy mode is appropriate. In the
 * other, WriteThreadPoolMode::Austere is appropriate. Below is a table show how
 * to select which mode you want.
 * | Mode Selection   | Peak Write Speed Less Than<br/>or Equal To DiskSpeed| Peak Write Speed<br/>Greater Than Disk Speed|
 * |:----------------:|:----------------------------:|:---------------------------:|
 * |Number Of Write Threads<br/>Greater Than Number of Files|WriteThreadPoolMode::Greedy<br/>is appropriate|WriteThreadPoolMode::Greedy<br/>is appropriate|
 * |Number Of Write Threads Less<br/>Than or Equal To Number Files|WriteThreadPoolMode::Greedy<br/>is appropriate|WriteThreadPoolMode::Austere<br/>is appropriate|
 */
enum class WriteThreadPoolMode : char 
{
    Greedy, ///<Write threads keep popping writes for a file until the file's write queue is empty
    Austere, ///<Write threads pop one write from a queue before moving on to another file write queue
};


}
}
}
#endif //SECANT_SRC_SECANT_IO_ASYNCOUTFILE_WRITETHREADPOOL_H
