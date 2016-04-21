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
#include"InputLib/InputLib.h"

std::string orchidVersion = "0.0.1";

std::string titleString =
"============================================================================\n"
"============================================================================\n"
"=                                                                          =\n"
"=                                                                          =\n"
"=     OOOO     RR RRR          CCCCCC  HH      HH  IIIIIIIIII  DDDDD       =\n"
"=   OOO  OOO   RRR   RR      CCC       HH      HH  IIIIIIIIII  DD  DDD     =\n"
"=  OO      OO  RR      RR   CCC        HH      HH      II      DD    DDD   =\n"
"=  OO      OO  RR     RR   CCC         HH      HH      II      DD      DD  =\n"
"=  OO      OO  RR   RR     CCC         HHHHHHHHHH      II      DD      DD  =\n"
"=  OO      OO  RRRRRR      CCC         HHHHHHHHHH      II      DD      DD  =\n"
"=  OO      OO  RR   RR     CCC         HH      HH      II      DD      DD  =\n"
"=  OO      OO  RR    RR     CCC        HH      HH      II      DD    DDD   =\n"
"=   OOO  OOO   RR     RR     CCC       HH      HH  IIIIIIIIII  DD  DDD     =\n"
"=     OOOO     RR      RR      CCCCCC  HH      HH  IIIIIIIIII  DDDDD       =\n"
"=                                                                          =\n"
"=                                                                          =\n"
"============================================================================\n"
"============================================================================\n"
"\n";


int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage:\n\t" << argv[0] << " InputFileName" << std::endl;
        return 1;
    }

    std::cout << titleString;
    std::cout << "    Version: "<< orchidVersion << "\n";
    std::string inputFileName(argv[1]);
    std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
    std::cout << "Reading input from the file: " << inputFileName << "\n\n";
    InputParser::InputParameters params;
    bool success = InputParser::parseBlockInputFile(&params, inputFileName);
    std::cout << "Parsing: " << (success?"Succeeded":"Failed") << "\n";
    std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
    if (!params.validateInputParameters())
    {
        std::cout << "\nParameter Validation Failed" << "\n";
        params.printValidationProblems();
    }
    else
    {
        std::cout << "\nParameter Validation Succeeded" << "\n";
        std::cout << "------------------------------------------------------\n";
        std::cout << params << "\n";
        std::cout << "------------------------------------------------------\n";
    }
    return 0;
}
