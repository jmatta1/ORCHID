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
#include"SlowControls/SlowData.h"
#include"DigiLib/RateData.h"
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
    std::cout << "Press enter to continue" <<std::endl;
    std::cout << "\n" << std::endl;
    std::cin.get();

    //build interthread queues and data strutures
    //count the number of voltage channels
    int numVoltageChannels = std::count(mpodChannelData.online.begin(),
                                        mpodChannelData.online.end(), true);
    int numTemperatureChannels = 0;
    SlowControls::SlowData* slowData = new SlowControls::SlowData(numVoltageChannels,
                                                                   numTemperatureChannels);
    int numDigitizerChannels = 0;
    FastData::RateData* rateData = new FastData::RateData(numDigitizerChannels);

    //create the various thread callable objects
    Threads::UIThread* ui = new Threads::UIThread(slowData,
                                                  rateData,
                                                  params.generalBlock->updateFrequency);

    //start the threads except the IO thread and detach them

    //start the IO thread and join it
    boost::thread uiThread(*ui);
    uiThread.join();

    //the IO thread has joined proceed with shutdown
    std::cout << "IO thread has rejoined main the thread.\nDeleting thread callable objects" << std::endl;
    //delete the thread callable objects
    delete ui;


    std::cout << "Deleting the interprocess communication shared objects" << std::endl;
    //delete shared objects generated for interprocess communication
    delete slowData;


    std::cout << "Closing ORCHID, have a nice day! :-)\n\n" << std::endl;

    return 0;
}

