/*******************************************************************************
ORCHID  -  ORnl baCkground at HfIr Daq
A program for directly acquiring data from the array of detectors serving at the
HFIR background monitoring wall.
*******************************************************************************/

// includes for C system headers
// includes for C++ system headers
#include<iostream>
#include<iomanip>
#include<string>
#include<thread>
#include<future>
#include<locale>
// includes from other libraries
// includes from ORCHID
#include"Utility/TitleString.h"
#include"Utility/ParseAndValidate.h"
#include"Utility/SortPermutation.h"
#include"InputLib/InputLib.h"

int main(int argc, char* argv[])
{
    std::string inputFileName;
    if (argc == 1)
    {
        std::cout << "Enter the configuration file name" << std::endl;
        std::cin >> inputFileName;
    }
    else if (argc == 2)
    {
        inputFileName = std::string(argv[1]);
    }
    else
    {
        std::cout << "Usage:\n\t" << argv[0] << " [InputFileName]" << std::endl;
        return 1;
    }
    
    //read the input file
    std::cout << Resources::titleString;
    InputParser::InputParameters params;
    InputParser::MpodChannelData mpodChannelData;
    InputParser::MpodModuleData mpodModuleData;
    if(!Utility::parseAndValidateInput(params, inputFileName) ||
       !Utility::parseAndValidateMpodModule(mpodModuleData, params.powerBlock->perModuleParameterFile) ||
       !Utility::parseAndValidateMpodChannel(mpodChannelData, params.powerBlock->perChannelParameterFile) )
    {
        return 1;
    }
    mpodModuleData.sort();
    mpodChannelData.sort();
    std::cout << std::flush << "\n\nInput Parameters\n";
    std::cout << "==========================================================\n\n";
    std::cout << "----------------------------------------------------------\n";
    std::cout << "Base Parameter Input File\n";
    std::cout << params << "\n";
    std::cout << "----------------------------------------------------------\n";
    std::cout << "----------------------------------------------------------\n";
    std::cout << "MPOD Module Input File\n";
    std::cout << mpodModuleData << "\n";
    std::cout << "----------------------------------------------------------\n";
    std::cout << "----------------------------------------------------------\n";
    std::cout << "MPOD Channel Input File\n";
    std::cout << mpodChannelData << "\n";
    std::cout << "----------------------------------------------------------\n";
    
    std::cout << "\n" << std::endl;
    bool quit_flag = false;
    std::locale loc;
    while(!quit_flag)
    {
        std::cout << "\nWaiting for input... " << std::endl;
        auto input = std::async(std::launch::async, []{std::string s; if(std::cin >> s) return s;});

        while(input.wait_for(std::chrono::seconds(2))!=std::future_status::ready)
        {
            std::cout << "Still Waiting..." << std::endl;
        }
        std::string temp = input.get();
        std::cout << "Input was: " << temp << std::endl;
        for(std::string::size_type i = 0; i<temp.length(); ++i) temp[i] = std::tolower(temp[i], loc);
        std::cout << "Lowered input is: " << temp << std::endl;
        if(temp == "quit")
        {
            quit_flag = true;
        }
    }

    return 0;
}

