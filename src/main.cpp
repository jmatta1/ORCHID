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
#define BOOST_LOG_DYN_LINK 1
#include<boost/log/utility/setup.hpp>
#include<boost/log/utility/setup/file.hpp>
#include<boost/log/sinks/text_file_backend.hpp>
#include<boost/log/utility/setup/common_attributes.hpp>
#include <boost/core/null_deleter.hpp>
// includes from ORCHID
// ORCHID utilities
#include"Utility/TitleString.h"
#include"Utility/ParseAndValidate.h"
#include"Utility/OrchidLogger.h"
// ORCHID interprocess communication data objects
#include"InterThreadComm/Data/SlowData.h"
#include"InterThreadComm/Data/RateData.h"
#include"InterThreadComm/Data/FileData.h"
// ORCHID interprocess communication control objects
#include"InterThreadComm/Control/SlowControlsThreadController.h"
// ORCHID device objects
#include"SlowControls/HVLib/MpodController.h"
#include"SlowControls/HVLib/SnmpUtilCommands.h"
// ORCHID threads
#include"Threads/UIThread.h"
#include"Threads/SlowControlsThread.h"

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

    //initialize the logging file
    boost::log::register_simple_formatter_factory< LogSeverity, char >("Severity");
    boost::log::add_file_log(
                boost::log::keywords::file_name = "orchid_%N.log",          //file name format
                boost::log::keywords::rotation_size = (1*1024*1024),        //rotate to a new file every megabyte
                boost::log::keywords::time_based_rotation = boost::log::sinks::file::rotation_at_time_point(0, 0, 0),  //or rotate at midnight
                //boost::log::keywords::format = "[%TimeStamp%]  (%LineID%) <%Severity%>: %Message%");  //give every message a timestamp
                boost::log::keywords::format = "[%TimeStamp%] <%Severity%>: %Message%");  //give every message a timestamp and severity
    //set up to dump to the console as well
    //boost::log::add_console_log();
    boost::log::add_common_attributes();
    
    boost::shared_ptr<std::ostream> coutStream(&std::cout, boost::null_deleter());
    typedef boost::log::sinks::synchronous_sink<boost::log::sinks::text_ostream_backend> text_sink;
    boost::shared_ptr<text_sink> coutSink = boost::make_shared<text_sink>();
    coutSink->locked_backend()->add_stream(coutStream);
    boost::log::core::get()->add_sink(coutSink);
    
    boost::log::sources::severity_logger_mt<LogSeverity>& lg = OrchidLog::get();
    BOOST_LOG_SEV(lg, Trace) << Resources::titleString;

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
    BOOST_LOG_SEV(lg, Information)  << "\n\nInput Parameters";
    BOOST_LOG_SEV(lg, Information)  << "================================================================================\n";
    BOOST_LOG_SEV(lg, Information)  << "--------------------------------------------------------------------------------";
    BOOST_LOG_SEV(lg, Information)  << "Base Parameter Input File";
    BOOST_LOG_SEV(lg, Information)  << params;
    BOOST_LOG_SEV(lg, Information)  << "--------------------------------------------------------------------------------";
    BOOST_LOG_SEV(lg, Information)  << "--------------------------------------------------------------------------------";
    BOOST_LOG_SEV(lg, Information)  << "MPOD Module Input File";
    BOOST_LOG_SEV(lg, Information)  << mpodModuleData ;
    BOOST_LOG_SEV(lg, Information)  << "--------------------------------------------------------------------------------";
    BOOST_LOG_SEV(lg, Information)  << "--------------------------------------------------------------------------------";
    BOOST_LOG_SEV(lg, Information)  << "MPOD Channel Input File";
    BOOST_LOG_SEV(lg, Information)  << mpodChannelData ;
    BOOST_LOG_SEV(lg, Information)  << "--------------------------------------------------------------------------------";
    //TODO: output the data from the digitizer csvs and the mapping csv
    BOOST_LOG_SEV(lg, Trace)  << "\nReady to start!" << std::flush;
    BOOST_LOG_SEV(lg, Trace)  << "Press enter to continue\n" <<std::flush;
    std::cin.get();
    BOOST_LOG_SEV(lg, Trace)  << "Proceeding" << std::flush;

    /*
     * Build the InterThread Statistics structures
     */
    //count the number of voltage channels
    BOOST_LOG_SEV(lg, Debug)  << "Building statistics accumulators" << std::flush;
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
    BOOST_LOG_SEV(lg, Debug)  << "Building thread control data structures" << std::flush;
    // For controlling DigitizerThread
    
    // For controlling SlowControlsThread
    InterThread::SlowControlsThreadController* sctController = new InterThread::SlowControlsThreadController();
    
    // For controlling FileOutputThread
    
    // For controlling EventProcThread(s)
    
    /*
     * Build the Device Control structures
     */
    BOOST_LOG_SEV(lg, Debug)  << "Building device control data structures" << std::flush;
    // For Controlling the MPOD (The temperature system needs no control)
    SlowControls::SnmpUtilControl* mpodSnmpController = new SlowControls::SnmpUtilControl(params.powerBlock->mpodIpAddress,
                                                                                          params.powerBlock->weinerMibFileDirectory);
    SlowControls::MpodController* mpodController = new SlowControls::MpodController(mpodSnmpController, &mpodChannelData, &mpodModuleData);
    SlowControls::MpodReader* mpodReader = new SlowControls::MpodReader(mpodSnmpController, &mpodChannelData);
    
    // For Controlling the digitizer
    
    /*
     * Build the InterThread Buffer/Data queues
     */
    BOOST_LOG_SEV(lg, Debug)  << "Building interthread data queues" << std::flush;
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
    BOOST_LOG_SEV(lg, Debug)  << "Building callable objects" << std::flush;
    //make the UI thread callable
    Threads::UIThread* uiThreadCallable =
            new Threads::UIThread(slowData, rateData, fileData,
                      //Future:   digitizerThreadControl,
                                  sctController,
                      //Future:   fileThreadControl, eventProcThreadControl,
                      //Future:   digitizerControl
                                  mpodController,
                                  params.generalBlock->updateFrequency,
                                  params.powerBlock->pollingRate);

    //make the file thread callable
    //Threads::FileThread* fileThreadCallable = new FileThread(...);
    
    //make the slow controls callable
    Threads::SlowControlsThread* scThreadCallable =
            new Threads::SlowControlsThread(mpodReader, slowData, sctController,
                                //Future    voltageEventOutQueue, voltageEventInQueue,
                                            params.powerBlock->pollingRate);
    
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
    BOOST_LOG_SEV(lg, Debug)  << "Building threads\n" << std::flush;
    // Make the threads
    boost::thread scThread(*scThreadCallable);
    BOOST_LOG_SEV(lg, Debug)  << "Starting UI Thread, stopping console logging\n" << std::flush;
    boost::log::core::get()->remove_sink(coutSink);
    boost::thread uiThread(*uiThreadCallable);
    //boost::thread fileThread(fileThreadCallable);
    //boost::thread slowControlsThread(slowControlsThreadCallable);
    //boost::thread digitizerThread(digitizerThreadCallable);
    //boost::thread_group eventProcessingThreads;
    /*for(int i = 0; i < numProcThreads; ++i)
    {
        eventProcessingThreads.create_thread(*(evProcThreadCallable[i]));
    }*/
    

    //Wait to join the IO thread
    uiThread.join();
    boost::log::core::get()->add_sink(coutSink);
    //the IO thread has joined proceed with shutdown
    BOOST_LOG_SEV(lg, Trace)  << "UI thread has rejoined main the thread. Resuming console logging." << std::flush;
    BOOST_LOG_SEV(lg, Trace)  << "Other threads should have terminated.\n" << std::flush;
    BOOST_LOG_SEV(lg, Debug)  << "Deleting thread callable objects" << std::flush;
    //delete the thread callable objects
    delete uiThreadCallable;
    delete scThreadCallable;

    //delete the data queues
    BOOST_LOG_SEV(lg, Debug)  << "Deleting interthread data queues" << std::flush;
    

    //delete the device control structures
    BOOST_LOG_SEV(lg, Debug)  << "Deleting device controls" << std::flush;
    delete mpodController;
    delete mpodReader;
    delete mpodSnmpController;
    
    //delete the thread control structures
    BOOST_LOG_SEV(lg, Debug)  << "Deleting thread controls" << std::flush;
    delete sctController;
    
    BOOST_LOG_SEV(lg, Debug)  << "Deleting statistics accumulators\n" << std::flush;
    //delete shared objects generated for interprocess communication
    delete slowData;
    delete rateData;
    delete fileData;


    BOOST_LOG_SEV(lg, Debug)  << "ORCHID has successfully shut down, have a nice day! :-)\n\n" << std::flush;

    return 0;
}

