/***************************************************************************//**
********************************************************************************
**
** @file ParseAndValidate.cpp
** @author James Till Matta
** @date 27 Apr, 2016
** @brief
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** @details Holds the function implementations for the functions called by
** int main(...) to actually parse and validate the various data files
**
********************************************************************************
*******************************************************************************/

#include"ParseAndValidate.h"
// includes for C system headers
// includes for C++ system headers
// includes from other libraries
// includes from ORCHID
namespace Utility
{

bool parseAndValidateInput(InputParser::InputParameters& params,
                           const std::string& inputFileName)
{
    bool parseSuccess = true;
    std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
    std::cout << "Reading Primary Input from the file: " << inputFileName << "\n";
    parseSuccess = InputParser::parseBlockInputFile(&params, inputFileName);
    std::cout << "Input File Parsing: " << (parseSuccess?"Succeeded":"Failed") << "\n";
    
    if(!parseSuccess)
    {
        std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
        return false;
    }
    if (!params.validateInputParameters())
    {
        std::cout << "\nInput File Validation Failed\n";
        params.printValidationProblems();
        std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
        return false;
    }
    else
    {
        std::cout << "Input File Validation Succeeded\n";
        std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
    }
    std::cout << "\n";
    return true;
}

bool parseAndValidateMpodChannel(InputParser::MpodChannelData& mpodChannelData,
                                 const std::string& inputFileName)
{
    bool parseSuccess = true;
    std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
    std::cout << "Reading MPOD Channel Data from the file: " << inputFileName << "\n";
    parseSuccess = InputParser::parseMpodChannelFile(&mpodChannelData, inputFileName);
    std::cout << "MPOD Channel Data File Parsing: " << (parseSuccess?"Succeeded":"Failed") << "\n";
    
    if(!parseSuccess)
    {
        std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
        return false;
    }
    if (!mpodChannelData.validate())
    {
        std::cout << "MPOD Channel File Validation Failed\n";
        mpodChannelData.printValidationErrors();
        std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
        return false;
    }
    else
    {
        std::cout << "MPOD Channel File Validation Succeeded\n";
        std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
    }
    std::cout << "\n";
    return true;
}

bool parseAndValidateMpodModule(InputParser::MpodModuleData& mpodModuleData,
                                const std::string& inputFileName)
{
    bool parseSuccess = true;
    std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
    std::cout << "Reading MPOD Module Data from the file: " << inputFileName << "\n";
    parseSuccess = InputParser::parseMpodModuleFile(&mpodModuleData, inputFileName);
    std::cout << "MPOD Module Data File Parsing: " << (parseSuccess?"Succeeded":"Failed") << "\n";
    
    if(!parseSuccess)
    {
        std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
        return false;
    }
    if (!mpodModuleData.validate())
    {
        std::cout << "MPOD Module Data Validation Failed\n";
        mpodModuleData.printValidationErrors();
        std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
        return false;
    }
    else
    {
        std::cout << "MPOD Module Data Validation Succeeded\n";
        std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
    }
    std::cout << "\n";
    return true;
}

bool parseAndValidateDigitizerChannel(InputParser::DigitizerChannelData& digitizerChannelData,
                                      const std::string& inputFileName)
{
    bool parseSuccess = true;
    std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
    std::cout << "Reading MPOD Module Data from the file: " << inputFileName << "\n";
    parseSuccess = InputParser::parseDigitizerChannelFile(&digitizerChannelData, inputFileName);
    std::cout << "MPOD Module Data File Parsing: " << (parseSuccess?"Succeeded":"Failed") << "\n";
    
    if(!parseSuccess)
    {
        std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
        return false;
    }
    if (!digitizerChannelData.validate())
    {
        std::cout << "Digitizer Module Data Validation Failed\n";
        digitizerChannelData.printValidationErrors();
        std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
        return false;
    }
    else
    {
        std::cout << "Digitizer Module Data Validation Succeeded\n";
        std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
    }
    std::cout << "\n";
    return true;
}

bool parseAndValidateDigitizerModule(InputParser::DigitizerModuleData& digitizerModuleData,
                                     const std::string& inputFileName)
{
    bool parseSuccess = true;
    std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
    std::cout << "Reading MPOD Module Data from the file: " << inputFileName << "\n";
    parseSuccess = InputParser::parseDigitizerModuleFile(&digitizerModuleData, inputFileName);
    std::cout << "MPOD Module Data File Parsing: " << (parseSuccess?"Succeeded":"Failed") << "\n";
    
    if(!parseSuccess)
    {
        std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
        return false;
    }
    if (!digitizerModuleData.validate())
    {
        std::cout << "Digitizer Module Data Validation Failed\n";
        digitizerModuleData.printValidationErrors();
        std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
        return false;
    }
    else
    {
        std::cout << "Digitizer Module Data Validation Succeeded\n";
        std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
    }
    std::cout << "\n";
    return true;
}


}
