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
#ifndef SECANT_SRC_SECANT_IO_ASYNCOUTFILE_WRITETHREAD_H
#define SECANT_SRC_SECANT_IO_ASYNCOUTFILE_WRITETHREAD_H
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
//predeclarations to allow references to these in the class declaration without their inclusion
class WriteThreadControl;
class WriteMultiQueue;
class ConcurrentOfstreamCollection;

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
 * |Number Of Write Threads<br/>Greater Than Number of Files|WriteThreadMode::Greedy<br/>is appropriate|WriteThreadMode::Greedy<br/> *might* be appropriate|
 * |Number Of Write Threads Less<br/>Than or Equal To Number Files|WriteThreadMode::Greedy<br/>is appropriate|WriteThreadMode::Austere<br/>is appropriate|
 * 
 * @remarks If running in greedy mode and the peak write speed is greater than
 * the disk speed there may be difficulty changing file name if writes are
 * coming from more than one thread (i.e. writes keep being enqueued even as
 * another thread is waiting for a lock to change the file name. If there is only
 * a single thread writing then trying to change the file name, regardless of the
 * peak write speed the queue will eventually empty and the lock will be granted
 * to the thread trying to change the name instead of the write thread (which
 * will hold the lock for as along as there is data to write to that file).
 * However, when there is a thread continuing to enqueue writes for that file
 * then the thread doing the writing may never stop, in which case it will never
 * release the file lock and so the thread trying to change the name will never
 * get custody of the lock to change the name.
 */
enum class WriteThreadMode : char 
{
    Greedy, ///<Write threads keep popping writes for a file until the file's write queue is empty
    Austere ///<Write threads pop one write from a queue before moving on to another file write queue
};

/**
 * @class WriteThreadPool
 * @ingroup SecantIoModule SecantAsyncFile
 * @brief This is the callable used to run individual write threads
 * @author James Till Matta
 * 
 * This class executes a get data, write data to appropriate file, loop.
 * If it is in Greedy mode it will continue to pull data to
 * write for a single file until there is no more data for that file before
 * it moves on. If it is in Austere mode, it will write a single buffer for
 * a file before it moves to the next file it needs to write for. In Austere
 * mode it is assured that progress will be made on writing to all files in
 * the case that the write speed to disk is less than the speed at which
 * data is being sent for writing. In that situation, if there are more files to
 * write to than there are write threads, if the threads are set to greedy, then
 * all the writing will occur for a few files rather than all of them
 */
class WriteThread
{
public:
    /**
     * @brief Constructor for WriteThread
     * @param[in] mode Sets if the thread will operate in Greedy or Austere mode
     * 
     * This constructs a WriteThread callable (to be used by boost::thread) for
     * the WriteThreadPool. The thread is permanently placed in mode on
     * construction.
     */
    WriteThread(WriteThreadMode mode);
    
    /**
     * @brief Make WriteThread 'Callable' for Boost::Thread to run it
     */
    void operator()();

private:
    /**
     * @brief Execution loop for Greedy mode
     */
    void greedyWriteLoop();
    
    /**
     * @brief Execute an Austere write cycle
     * 
     * The austere write cycle passes through each queue, taking a single write
     * buffer (if available) from each
     */
    void austereWriteLoop();
    
    /**
     * @brief Used to run the inner loop of the writing in greedy mode
     */
    void doGreedyWriteCycle();
    
    /**
     * @brief Used to run the inner loop of the writing in austere mode
     */
    void doAustereWriteCycle();
    
    /**
     * @brief Used when leaving run mode to clear any writes remaining
     * 
     * This loop runs in a greedy way, clearing all writes for a given file
     * prior to moving on to the next file. This should not matter since, in
     * theory we only get to this state when writing of files is shutting down
     * for at least a little while
     */
    void clearWriteBufferLoop(){doGreedyWriteCycle();}
    
    /**
     * @brief Does the heavy lifting of popping a buffer, writing data, and pushing the empty buffer
     * @param[in] fileNum The index of the file to write
     */
    void doPopWritePushAction(int fileNum, ConcurrentOfstreamWrapper& currFile);
    
    WriteThreadMode writeMode; ///<Holds the mode to be used to call the appropriate private function at execution start
    WriteMultiQueue& writeQueue; ///<Holds reference to the WriteMultiQueue singleton
    WriteThreadControl& controller; ///<Holds reference to the WriteThreadController singleton
    ConcurrentOfstreamCollection& files; ///<Holds reference to the list of ofstream files
};

}
}
}
#endif //SECANT_SRC_SECANT_IO_ASYNCOUTFILE_WRITETHREAD_H
