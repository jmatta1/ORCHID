/*******************************************************************************
ORCHID  -  ORnl baCkground at HfIr Daq
A program for directly acquiring data from the array of detectors serving at the
HFIR background monitoring wall.
*******************************************************************************/

// includes for C system headers
// includes for C++ system headers
#include<sstream>
#include<iomanip>
#include<string>
// includes from other libraries
#include<ncurses.h>
// includes from ORCHID
#include"Utility/TitleString.h"
#include"Utility/ParseAndValidate.h"

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
    //TODO: output the data from the digitizer csvs and the mapping csv
    std::cout << "\n" << std::endl;
    std::cout << "Ready to start!" << std::endl;
    std::cout << "Press enter to continue" <<std::endl;
    std::cin.get();

    //build interthread queues and data strutures

    //create the various thread callable objects

    //start the threads except the IO thread

    //start the IO thread and join it

    //This thread *is* the IO thread, so we call the routine that sets everything
    //up for work
    /*
    initscr();
    clear();
    std::ostringstream out;
    out << "Starting now" << std::endl;
    printw(out.str().c_str());
    getch();
    printw(out.str().c_str());
    out.str("");
    out.clear();
    bool quit_flag = false;
    std::locale loc;
    while(!quit_flag)
    {
        out << "\nWaiting for input... " << std::endl;
        printw(out.str().c_str());
        out.str("");
        out.clear();
        auto input = std::async(std::launch::async, []{char s[150]; if(getnstr(s, 150) > 0) return std::string(s);});

        while(input.wait_for(std::chrono::seconds(2))!=std::future_status::ready)
        {
            out << "Still Waiting..." << std::endl;
            printw(out.str().c_str());
            out.str("");
            out.clear();
        }
        std::string temp = input.get();
        out << "Input was: " << temp << std::endl;
        for(std::string::size_type i = 0; i<temp.length(); ++i) temp[i] = std::tolower(temp[i], loc);
        out << "Lowered input is: " << temp << std::endl;
        printw(out.str().c_str());
        out.str("");
        out.clear();
        if(temp == "quit")
        {
            quit_flag = true;
        }
    }
    endwin();
    */
    std::cout << "IO thread has returned, shutdown should be complete.\n";
    std::cout << "Exitting ORCHID, have a nice day! :-)" << std::endl;

    return 0;
}

