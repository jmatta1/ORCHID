/*******************************************************************************
ORCHID  -  ORnl baCkground at HfIr Daq
A program for directly acquiring data from the array of detectors serving at the
HFIR background monitoring wall.
*******************************************************************************/

// includes for C system headers
// includes for C++ system headers
#include<string>
#include<algorithm>
// includes from other libraries
#include<boost/thread.hpp>
// includes from ORCHID
// ORCHID utilities
#include"Utility/TitleString.h"
#include"Utility/ParseAndValidate.h"
// ORCHID interprocess communication objects
#include"InterThreadComm/Data/SlowData.h"
#include"InterThreadComm/Data/RateData.h"
// ORCHID threads
#include"Threads/UIThread.h"

int main(int argc, char* argv[])
{
    std::string inputFileName;
    if (argc == 2)
    {
        inputFileName = std::string(argv[1]);
    }
    else
    {
        std::cout << "Usage:\n\t" << argv[0] << " InputFileName" << std::endl;
        return 1;
    }

    std::cout << Resources::titleString;

    //parse the input file and ancillary csv files
    InputParser::InputParameters params;
    InputParser::MpodChannelData mpodChannelData;
    InputParser::MpodModuleData mpodModuleData;
    //TODO: Add reading of digitizer module and channel csv files
    //TODO: Also add the reading of the mapping csv which maps digitizer channel to voltage channel
    if(!Utility::parseAndValidateInput(params, inputFileName) ||
       !Utility::parseAndValidateMpodModule(mpodModuleData, params.powerBlock->perModuleParameterFile) ||
       !Utility::parseAndValidateMpodChannel(mpodChannelData, params.powerBlock->perChannelParameterFile) )
    {
        return 1;
    }

    mpodModuleData.sort();
    mpodChannelData.sort();
    std::cout << std::flush << "\n\nInput Parameters\n";
    std::cout << "================================================================================\n\n";
    std::cout << "--------------------------------------------------------------------------------\n";
    std::cout << "Base Parameter Input File\n";
    std::cout << params << "\n";
    std::cout << "--------------------------------------------------------------------------------\n";
    std::cout << "--------------------------------------------------------------------------------\n";
    std::cout << "MPOD Module Input File\n";
    std::cout << mpodModuleData << "\n";
    std::cout << "--------------------------------------------------------------------------------\n";
    std::cout << "--------------------------------------------------------------------------------\n";
    std::cout << "MPOD Channel Input File\n";
    std::cout << mpodChannelData << "\n";
    std::cout << "--------------------------------------------------------------------------------\n";
    //TODO: output the data from the digitizer csvs and the mapping csv
    std::cout << "\nReady to start!" << std::endl;
    std::cout << "Press enter to continue\n" <<std::endl;
    std::cin.get();

    /*
     * Build the InterThread Statistics structures
     */
    //count the number of voltage channels
    std::cout << "Building statistics accumulators" << std::endl;
    int numVoltageChannels = std::count(mpodChannelData.online.begin(),
                                        mpodChannelData.online.end(), true);
    //count the number of temperature channels
    int numTemperatureChannels = 4;
    //Make the slow controls data structure
    InterThread::SlowData* slowData = new InterThread::SlowData(numVoltageChannels,
                                                                 numTemperatureChannels);
    //count the number of digitizer channels
    int numDigitizerChannels = 4;
    //Make the rate data structure
    InterThread::RateData* rateData = new InterThread::RateData(numDigitizerChannels);
    //Make the data structure for file information
    InterThread::FileData* fileData = new InterThread::FileData();
    fileData->setRunTitle(params.generalBlock->runTitle);
    
    /*
     * Build the InterThread Control structures
     */
    std::cout << "Building thread control data structures" << std::endl;
    // For controlling DigitizerThread
    
    // For controlling SlowControlsThread
    
    // For controlling FileOutputThread
    
    // For controlling EventProcThread(s)
    
    /*
     * Build the Device Control structures
     */
    std::cout << "Building device control data structures" << std::endl;
    // For Controlling the MPOD (The temperature system needs no control)
    
    
    // For Controlling the digitizer
    
    /*
     * Build the InterThread Buffer/Data queues
     */
    std::cout << "Building interthread data queues" << std::endl;
    // For transferring full data buffers from the digitizer
    // reader to the event processing threads
    
    // For transferring empty data buffers from the event
    // processing threads to the digitizer reader
    
    // For transferring full event buffers from the event
    // processing threads to the file thread
    
    // For transferring empty event buffers from the file
    // thread to the event processing threads
    
    // For transferring full slow controls buffers from the
    // slow control threads to the file thread
    
    // For transferring empty slow controls buffers from the
    // file thread to the slow control thread
    
    /*
     * Build the callable objects for boost::thread
     */
    std::cout << "Building callable objects" << std::endl;
    //make the UI thread callable
    Threads::UIThread* uiThreadCallable =
            new Threads::UIThread(slowData, rateData, fileData,
                      //Future:   digitizerThreadControl, slowControlsThreadControl,
                      //Future:   fileThreadControl, eventProcThreadControl,
                      //Future:   digitizerControl, mpodControl
                                  params.generalBlock->updateFrequency);
    //make the file thread callable
    //Threads::FileThread* fileThreadCallable = new FileThread(...);
    
    //make the slow controls callable
    //Threads::SlowControlsThread slowControlsThreadCallable =
    //          new SlowControlsThread(...);
    
    //make the digitizer reader callable
    //Threads::DigitizerThread* digitizerThreadCallable =
    //          new DigitizerThread(...);
    
    //make the event processing callables
    //Threads::EventProcessingThread** evProcThreadCallable =
    //          new EventProcessingThread*[numProcThreads];
    //for(int i=0; i < numProcThreads; ++i)
    //{
    //  evProcThreadCallable[i] = new EventProcessingThread(i, ...);
    //}
    

    /*
     * Handle the threads
     */
    std::cout << "Building threads\n" << std::endl;
    // Make the threads
    boost::thread uiThread(*uiThreadCallable);
    //boost::thread fileThread(fileThreadCallable);
    //boost::thread slowControlsThread(slowControlsThreadCallable);
    //boost::thread digitizerThread(digitizerThreadCallable);
    //boost::thread_group eventProcessingThreads;
    /*for(int i = 0; i < numProcThreads; ++i)
    {
        eventProcessingThreads.create_thread(*(evProcThreadCallable[i]));
    }*/
    
    
    // Detach all threads except the IO thread
    

    //Wait to join the IO thread
    uiThread.join();

    //the IO thread has joined proceed with shutdown
    std::cout << "UI thread has rejoined main the thread." << std::endl;
    std::cout << "Other threads should have terminated.\n" << std::endl;
    std::cout << "Deleting thread callable objects" << std::endl;
    //delete the thread callable objects
    delete uiThreadCallable;

    //delete the data queues
    std::cout << "Deleting interthread data queues" << std::endl;
    

    //delete the device control structures
    std::cout << "Deleting device controls" << std::endl;
    
    //delete the thread control structures
    std::cout << "Deleting thread controls" << std::endl;
    
    std::cout << "Deleting statistics accumulators\n" << std::endl;
    //delete shared objects generated for interprocess communication
    delete slowData;
    delete rateData;
    delete fileData;


    std::cout << "Closing ORCHID, have a nice day! :-)\n\n" << std::endl;

    return 0;
}

