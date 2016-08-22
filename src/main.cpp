/*******************************************************************************
ORCHID  -  ORnl baCkground at HfIr Daq
A program for directly acquiring data from the array of detectors serving at the
HFIR background monitoring wall.
*******************************************************************************/

// includes for C system headers
// includes for C++ system headers
#include<string>
#include<algorithm>
#include<iostream>
#include<iomanip>
// includes from other libraries
#include<boost/thread.hpp>
#include<boost/lockfree/queue.hpp>
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
#include"Utility/MpodMapper.h"
#include"Utility/CommonTypeDefs.h"
// ORCHID interprocess communication data objects
#include"InterThreadComm/Data/SlowData.h"
#include"InterThreadComm/Data/AcquisitionData.h"
#include"InterThreadComm/Data/FileData.h"
#include"InterThreadComm/InterThreadQueueSizes.h"
#include"InterThreadComm/MultiQueuePair.h"
#include"InterThreadComm/QueuePair.h"
#include"Events/SlowControlsEvent.h"
// ORCHID interprocess communication control objects
#include"InterThreadComm/Control/SlowControlsThreadController.h"
#include"InterThreadComm/Control/FileOutputThreadController.h"
// ORCHID device objects
#include"Hardware//HVLib/MpodController.h"
#include"Hardware/HVLib/SnmpUtilCommands.h"
#include"Hardware/Digitizer/Vx1730Digitizer.h"
// ORCHID threads
#include"Threads/UIThread.h"
#include"Threads/SlowControlsThread.h"
#include"Threads/FileOutputThread.h"
#include"Threads/ThreadWrapper.h"

#include<fstream>

//pre declare event interface so we can use pointers to it as a type
class EventInterface;

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
                boost::log::keywords::rotation_size = (8*1024*1024),        //rotate to a new file every megabyte
                boost::log::keywords::time_based_rotation = boost::log::sinks::file::rotation_at_time_point(0, 0, 0),  //or rotate at midnight
                boost::log::keywords::auto_flush = true,
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
    InputParser::DigitizerModuleData digitizerModuleData;
    InputParser::DigitizerChannelData digitizerChannelData;
    
    //TODO: Add reading of digitizer module and channel csv files
    //TODO: Also add the reading of the mapping csv which maps digitizer channel to voltage channel
    if(!Utility::parseAndValidateInput(params, inputFileName) ||
       !Utility::parseAndValidateMpodModule(mpodModuleData, params.powerBlock->perModuleParameterFile) ||
       !Utility::parseAndValidateMpodChannel(mpodChannelData, params.powerBlock->perChannelParameterFile) ||
       !Utility::parseAndValidateDigitizerModule(digitizerModuleData, params.digitizerBlock->perModuleParameterFile) ||
       !Utility::parseAndValidateDigitizerChannel(digitizerChannelData, params.digitizerBlock->perChannelParameterFile) )
    {
        return 1;
    }

    mpodModuleData.sort();
    mpodChannelData.sort();
    digitizerModuleData.sort();
    digitizerChannelData.sort();
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
    BOOST_LOG_SEV(lg, Information)  << "--------------------------------------------------------------------------------";
    BOOST_LOG_SEV(lg, Information)  << "Digitizer Module Input File";
    BOOST_LOG_SEV(lg, Information)  << digitizerModuleData ;
    BOOST_LOG_SEV(lg, Information)  << "--------------------------------------------------------------------------------";
    BOOST_LOG_SEV(lg, Information)  << "--------------------------------------------------------------------------------";
    BOOST_LOG_SEV(lg, Information)  << "Digitizer Module Input File";
    BOOST_LOG_SEV(lg, Information)  << digitizerChannelData ;
    BOOST_LOG_SEV(lg, Information)  << "--------------------------------------------------------------------------------";
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
    //TODO: Stop Spoofing temperature channels
    int numTemperatureChannels = 0;
    //Make the slow controls data structure
    InterThread::SlowData* slowData = new InterThread::SlowData(numVoltageChannels,
                                                                 numTemperatureChannels);
    //count the number of digitizer channels
    int numDigitizerChannels = 4;
    //Make the rate data structure
    InterThread::RateData* rateData = new InterThread::RateData(numDigitizerChannels);
    //Make the data structure for file information
    InterThread::FileData* fileData = new InterThread::FileData();
    
    /*
     * Build the InterThread Control structures
     */
    BOOST_LOG_SEV(lg, Debug)  << "Building thread control data structures" << std::flush;
    // For controlling DigitizerThread
    
    // For controlling SlowControlsThread
    InterThread::SlowControlsThreadController* sctController = new InterThread::SlowControlsThreadController();
    
    // For controlling FileOutputThread
    InterThread::FileOutputThreadController* fotController = new InterThread::FileOutputThreadController();
    
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
    Utility::MpodMapper* mpodMapper = new Utility::MpodMapper();
    mpodMapper->loadFromData(&mpodChannelData);
    
    // For Controlling the digitizer
    int numDigitizers = digitizerModuleData.linkType.size();
    Digitizer::Vx1730Digitizer** digitizerList = new Digitizer::Vx1730Digitizer*[numDigitizers];
    for(int i=0; i<numDigitizers; ++i)
    {
        digitizerList[i] = new Digitizer::Vx1730Digitizer(i, &digitizerModuleData, &digitizerChannelData);
        digitizerList[i]->setupDigitizer();
    }
    
    //for debugging, start acqusition of the first digitizer and wait for a single interrupt, read the data and dump to a simple file
    std::ofstream outData;
    outData.open("./tempdata.dat", std::ios_base::binary);
    int bufferSize = digitizerList[0]->getSizeOfReadBufferIn32BitInts();
    BOOST_LOG_SEV(lg, Debug)  << "Buffer Size is: " << bufferSize;
    unsigned int* tempBuffer = new unsigned int[bufferSize];
    digitizerList[0]->startAcquisition();
    unsigned int dataRead = digitizerList[0]->getData(tempBuffer);
    BOOST_LOG_SEV(lg, Debug)  << "Read " << dataRead << " From Interrupt";
    digitizerList[0]->stopAcquisition();
    if(dataRead != 0)
    {
        outData.write(reinterpret_cast<char*>(tempBuffer), 4*dataRead);
    }
    dataRead = digitizerList[0]->performFinalReadout(tempBuffer);
    BOOST_LOG_SEV(lg, Debug)  << "Read " << dataRead << " From Final Readout";
    if(dataRead != 0)
    {
        outData.write(reinterpret_cast<char*>(tempBuffer), 4*dataRead);
    }
    outData.close();

#if 0    
    /*
     * Build the InterThread Buffer/Data queues
     */
    BOOST_LOG_SEV(lg, Debug)  << "Building interthread data queues" << std::flush;
    // For transferring full data buffers from the digitizer
    // reader to the event processing threads
    
    // For transferring empty data buffers from the event
    // processing threads to the digitizer reader
    
    // transfers full slow controls events from the slow controls thread to the
    // file output thread. Also transfers empty sc events back to sc thread. Also
    // transfers full data events from the processing threads to the file output
    // thread, also transfers empty data events from the file output thread to
    // the processing threads
    Utility::ToFileMultiQueue* toFileQueues = new Utility::ToFileMultiQueue;
    //TODO: Load to file queue with empty dpp psd events
    /*for(int i=0; i < InterThread::getEnumVal(InterThread::QueueSizes::SlowControlToFile); ++i)
    {
        //we use comsumer push since consumers push to the empty object return
        toFileQueues->consumerPush<Utility::ProcessingQueueIndex>(new Events::TriggerEvent());
    }*/
    //here we load the queue with empty slow controls events
    for(int i=0; i < InterThread::getEnumVal(InterThread::QueueSizes::SlowControlToFile); ++i)
    {
        //we use consumer push since consumers push to the empty object return
        toFileQueues->consumerPush<Utility::SlowControlsQueueIndex>(new Events::SlowControlsEvent(numVoltageChannels, numTemperatureChannels));
    }
    

    /*
     * Build the callable objects for boost::thread
     */
    BOOST_LOG_SEV(lg, Debug)  << "Building callable objects and their wrappers" << std::flush;
    //make the UI thread callable
    Threads::UIThread* uiThreadCallable =
            new Threads::UIThread(slowData, rateData, fileData, mpodMapper,
                      //Future:   digitizerControl,
                                  sctController, fotController,
                      //Future:   eventProcThreadControl, digitizerThreadControl,
                                  toFileQueues, mpodController,
                                  params.generalBlock->updateFrequency,
                                  params.powerBlock->pollingRate);
    Threads::ThreadWrapper<Threads::UIThread>* uiThreadWrapper = 
            new Threads::ThreadWrapper<Threads::UIThread>(uiThreadCallable);

    //make the file thread callable
    Threads::FileOutputThread* fileThreadCallable =
            new Threads::FileOutputThread(toFileQueues, fileData, fotController, params.generalBlock);
    Threads::ThreadWrapper<Threads::FileOutputThread>* fileThreadWrapper =
            new Threads::ThreadWrapper<Threads::FileOutputThread>(fileThreadCallable);
    
    //make the slow controls callable
    Threads::SlowControlsThread* scThreadCallable =
            new Threads::SlowControlsThread(mpodReader, slowData, sctController,
                                            toFileQueues, params.powerBlock->pollingRate);
    Threads::ThreadWrapper<Threads::SlowControlsThread>* scThreadWrapper = 
            new Threads::ThreadWrapper<Threads::SlowControlsThread>(scThreadCallable);
    
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
    boost::thread scThread(*scThreadWrapper);
    boost::thread fileThread(*fileThreadWrapper);
    //boost::thread digitizerThread(*digitizerThreadCallable);
    //boost::thread_group eventProcessingThreads;
    /*for(int i = 0; i < numProcThreads; ++i)
    {
        eventProcessingThreads.create_thread(*(evProcThreadCallable[i]));
    }*/
    //make the UI thread last
    BOOST_LOG_SEV(lg, Debug)  << "Starting UI Thread, stopping console logging\n" << std::flush;
    boost::log::core::get()->remove_sink(coutSink);
    coutSink->flush();
    boost::thread uiThread(*uiThreadWrapper);

    //Wait to join the IO thread
    uiThread.join();
    boost::log::core::get()->add_sink(coutSink);

    //the IO thread has joined proceed with shutdown
    BOOST_LOG_SEV(lg, Trace)  << "UI thread has rejoined main the thread. Resuming console logging." << std::flush;
    BOOST_LOG_SEV(lg, Trace)  << "Other threads should have terminated.\n" << std::flush;
    BOOST_LOG_SEV(lg, Debug)  << "Deleting thread callable objects" << std::flush;
    //delete the thread callable objects
    delete uiThreadWrapper;
    delete uiThreadCallable;
    delete scThreadWrapper;
    delete scThreadCallable;
    delete fileThreadWrapper;
    delete fileThreadCallable;
    
    //delete the data queues
    BOOST_LOG_SEV(lg, Debug)  << "Deleting interthread data queues" << std::flush;
    //TODO: delete empty events from queue
    /*for(int i=0; i < InterThread::getEnumVal(InterThread::QueueSizes::SlowControlToFile); ++i)
    {
        Events::EventInterface* temp;
        //we use producer pop because that pulls empty events from the queue index
        toFileQueues->producerPop<Utility::ProcessingQueueIndex>(temp);
    }*/
    //here we load the queue with empty slow controls events
    for(int i=0; i < InterThread::getEnumVal(InterThread::QueueSizes::SlowControlToFile); ++i)
    {
        Events::EventInterface* temp;
        //we use producer pop because that pulls empty events from the queue index
        toFileQueues->producerPop<Utility::SlowControlsQueueIndex>(temp);
        delete temp;
    }
    delete toFileQueues;
#endif
    //delete the device control structures
    BOOST_LOG_SEV(lg, Debug)  << "Deleting device controls" << std::flush;
    delete mpodMapper;
    delete mpodController;
    delete mpodReader;
    delete mpodSnmpController;
    
    for(int i=0; i<numDigitizers; ++i)
    {
        delete digitizerList[i];
    }
    delete[] digitizerList;
    
    //delete the thread control structures
    BOOST_LOG_SEV(lg, Debug)  << "Deleting thread controls" << std::flush;
    delete sctController;
    delete fotController;
    
    BOOST_LOG_SEV(lg, Debug)  << "Deleting statistics accumulators\n" << std::flush;
    //delete shared objects generated for interprocess communication
    delete slowData;
    delete rateData;
    delete fileData;

    BOOST_LOG_SEV(lg, Debug)  << "ORCHID has successfully shut down, have a nice day! :-)\n\n" << std::flush;

    return 0;
}

