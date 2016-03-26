/*******************************************************************************
ORCHID  -  ORnl baCkground at HfIr Daq
A program for directly acquiring data from the array of detectors serving at the
HFIR background monitoring wall.
*******************************************************************************/

// includes for C system headers
// includes for C++ system headers
#include<iostream>
// includes from other libraries
// includes from ORCHID
#include"InputLib/InputLib.h"

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage:\n\t" << argv[0] << " InputFileName" << std::endl;
        return 1;
    }

    std::string inputFileName(argv[1]);
    std::cout << "----------------------------------------------------------\n";
    std::cout << "Reading input from the file: " << inputFileName << "\n\n";

    inparser::InputParameters params;

    bool success = inparser::parseBlockInputFile(&params, inputFileName);

    std::cout << "Parsing: " << (success?"Succeeded":"Failed") << "\n";
    if (!params.validateInputParameters())
    {
        std::cout << "Parameter Validation Failed" << "\n";
        params.printValidationProblems();
    }
    std::cout << "----------------------------------------------------------\n";
    std::cout << params << "\n";
    std::cout << "----------------------------------------------------------\n";
    return 0;
}
