/*******************************************************************************
ORCHID  -  ORnl baCkground at HfIr Daq
A program for directly acquiring data from the array of detectors serving at the
HFIR background monitoring wall.
*******************************************************************************/

// includes for C system headers
// includes for C++ system headers
#include<iostream>
#include<string>
// includes from other libraries
// includes from ORCHID
#include"TitleString.h"
#include"InputLib/InputLib.h"



bool parseAndValidateInput(InputParser::InputParameters& params,
                           const std::string& inputFileName);

bool parseAndValidateMpodChannel(InputParser::MpodChannelData& mpodChannelData,
                                 const std::string& inputFileName);

bool parseAndValidateMpodModule(InputParser::MpodModuleData& mpodModuleData,
                                const std::string& inputFileName);
                                

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage:\n\t" << argv[0] << " InputFileName" << std::endl;
        return 1;
    }
    
    //read the input file
    std::cout << titleString;
    std::string inputFileName(argv[1]);
    InputParser::InputParameters params;
    InputParser::MpodChannelData mpodChannelData;
    InputParser::MpodModuleData mpodModuleData;
    if(!parseAndValidateInput(params, inputFileName) ||
       !parseAndValidateMpodModule(mpodModuleData, params.powerBlock->perModuleParameterFile) ||
       !parseAndValidateMpodChannel(mpodChannelData, params.powerBlock->perChannelParameterFile) )
    {
        return 1;
    }
    
    std::cout << std::flush << "\n\nInput Parameters\n\n";
    std::cout << "==========================================================\n";
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
    
    return 0;
}

bool parseAndValidateInput(InputParser::InputParameters& params,
                           const std::string& inputFileName)
{
    bool parseSuccess = true;
    std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
    std::cout << "Reading Primary Input from the file: " << inputFileName << "\n";
    parseSuccess = InputParser::parseBlockInputFile(&params, inputFileName);
    std::cout << "Input File Parsing: " << (parseSuccess?"Succeeded":"Failed") << "\n";
    
    if(!parseSuccess)
    {
        std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
        return false;
    }
    if (!params.validateInputParameters())
    {
        std::cout << "\nInput File Validation Failed\n";
        params.printValidationProblems();
        std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
        return false;
    }
    else
    {
        std::cout << "Input File Validation Succeeded\n";
        std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
    }
    std::cout << "\n";
    return true;
}

bool parseAndValidateMpodModule(InputParser::MpodModuleData& mpodModuleData,
                                const std::string& inputFileName)
{
    bool parseSuccess = true;
    std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
    std::cout << "Reading MPOD Module Data from the file: " << inputFileName << "\n";
    parseSuccess = InputParser::parseMpodModuleFile(&mpodModuleData, inputFileName);
    std::cout << "MPOD Module Data File Parsing: " << (parseSuccess?"Succeeded":"Failed") << "\n";
    
    if(!parseSuccess)
    {
        std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
        return false;
    }
    if (!mpodModuleData.validate())
    {
        std::cout << "MPOD Module Data Validation Failed\n";
        mpodModuleData.printValidationErrors();
        std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
        return false;
    }
    else
    {
        std::cout << "MPOD Module Data Validation Succeeded\n";
        std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
    }
    std::cout << "\n";
    return true;
}

bool parseAndValidateMpodChannel(InputParser::MpodChannelData& mpodChannelData,
                                 const std::string& inputFileName)
{
    bool parseSuccess = true;
    std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
    std::cout << "Reading MPOD Channel Data from the file: " << inputFileName << "\n";
    parseSuccess = InputParser::parseMpodChannelFile(&mpodChannelData, inputFileName);
    std::cout << "MPOD Channel Data File Parsing: " << (parseSuccess?"Succeeded":"Failed") << "\n";
    
    if(!parseSuccess)
    {
        std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
        return false;
    }
    if (!mpodChannelData.validate())
    {
        std::cout << "MPOD Channel File Validation Failed\n";
        mpodChannelData.printValidationErrors();
        std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
        return false;
    }
    else
    {
        std::cout << "MPOD Channel File Validation Succeeded\n";
        std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
    }
    std::cout << "\n";
    return true;
}

