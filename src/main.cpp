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

static const std::string titleString = "\n\n"
"  OOOO           kk              RRRR            dd                         \n"
"OO    OO         kk              RR  RR   ii     dd         eeee            \n"
"OO    OO   aa    kk  kk          RR  RR          dd        ee  ee           \n"
"OO    OO aa  aa  kkkk            RRRR     ii  ddddd  ggggg eeeee            \n"
"OO    OO aa  aa  kk  kk          RR RR    ii dd  dd gg  gg ee               \n"
"  OOOO     aa aa kk   kk         RR  RR   ii  ddddd  ggggg  eeeee           \n"
"                                                        gg                  \n"
"                                                      gggg                  \n"
"                                                                            \n"
"bb               CCCC kk                                               dd   \n"
"bb              CC    kk                                               dd   \n"
"bbbb     aa    CC     kk  kk         rrrrr    oo   uu  uu  nn nn     dddd   \n"
"bb  bb aa  aa  CC     kkkk     ggggg rr  rr oo  oo uu  uu  nnn  nn dd  dd   \n"
"bb  bb aa  aa   CC    kk  kk  gg  gg rr     oo  oo uu  uu  nn   nn dd  dd   \n"
"bbbb     aa aa   CCCC kk   kk  ggggg rr       oo     uu uu nn   nn   dddd   \n"
"                                  gg                                        \n"
"                                gggg                                        \n"
"                                                                            \n"
"          tt           HH  HH IIIIII   fff                                  \n"
"          tt           HH  HH   II    ff ff                                 \n"
"  aa    tttttt         HHHHHH   II    ff    rrrrr                           \n"
"aa  aa    tt           HH  HH   II   ffff   rr  rr                          \n"
"aa  aa    tt           HH  HH   II    ff    rr                              \n"
"  aa aa   tt           HH  HH IIIIII  ff    rr                              \n"
"                                                                            \n"
"DDDD                                                                        \n"
"DD  DD                                                                      \n"
"DD   DD   aa      qq                                                        \n"
"DD   DD aa  aa  qq  qq                                                      \n"
"DD  DD  aa  aa  qq  qq                                                      \n"
"DDDD      aa aa   qqqq                                                      \n"
"                    qq qq                                                   \n"
"                    qqqq                                                    \n"
"                                                                            \n"
"============================================================================\n"
"============================================================================\n"
"=                                                                          =\n"
"=                                                                          =\n"
"=     OOOO     RRRRRR          CCCCCC  HH      HH  IIIIIIIIII  DDDDD       =\n"
"=   OOO  OOO   RR   RRR      CCC       HH      HH  IIIIIIIIII  DD  DDD     =\n"
"=  OO      OO  RR      RR   CCC        HH      HH      II      DD    DDD   =\n"
"=  OO      OO  RR      RR  CCC         HH      HH      II      DD      DD  =\n"
"=  OO      OO  RR   RRR    CCC         HHHHHHHHHH      II      DD      DD  =\n"
"=  OO      OO  RRRRRR      CCC         HHHHHHHHHH      II      DD      DD  =\n"
"=  OO      OO  RR   RR     CCC         HH      HH      II      DD      DD  =\n"
"=  OO      OO  RR    RR     CCC        HH      HH      II      DD    DDD   =\n"
"=   OOO  OOO   RR     RR     CCC       HH      HH  IIIIIIIIII  DD  DDD     =\n"
"=     OOOO     RR      RR      CCCCCC  HH      HH  IIIIIIIIII  DDDDD       =\n"
"=                                                                          =\n"
"=                                                                          =\n"
"============================================================================\n"
"============================================================================\n"
"    Version:                                                                \n"
"             0.0.1\n\n";


int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage:\n\t" << argv[0] << " InputFileName" << std::endl;
        return 1;
    }

    std::cout << titleString;
    std::string inputFileName(argv[1]);
    std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
    std::cout << "Reading input from the file: " << inputFileName << "\n\n";
    InputParser::InputParameters params;
    bool success = InputParser::parseBlockInputFile(&params, inputFileName);
    std::cout << "Input File Parsing: " << (success?"Succeeded":"Failed") << "\n";
    std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
    if(!success)
    {
        return 1;
    }
    if (!params.validateInputParameters())
    {
        std::cout << "\nParameter Validation Failed" << "\n";
        params.printValidationProblems();
        return 1;
    }
    else
    {
        std::cout << "\nParameter Validation Succeeded" << "\n";
        std::cout << "------------------------------------------------------\n";
        std::cout << params << "\n";
        std::cout << "------------------------------------------------------\n";
    }
    
    //now that we have successfully read the block input file, now read the
    // csv file with the MPOD data
  /*  
    std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
    std::cout << "Reading MPOD CSV from the file: " << params.powerBlock->perChannelParameterFile << "\n\n";
    InputParser::MpodCsvData mpodData;
    bool csvSuccess = InputParser::parseMpodChannelFile(&mpodData, params.powerBlock->perChannelParameterFile);
    std::cout << "MPOD CSV Parsing: " << (csvSuccess?"Succeeded":"Failed") << "\n";
    std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
    if(!csvSuccess)
    {
        return 1;
    }
    if (!mpodData.validate())
    {
        std::cout << "\nParameter Validation Failed" << "\n";
        mpodData.printValidationErrors();
        return 1;
    }
    else
    {
        std::cout << "\nParameter Validation Succeeded" << "\n";
        std::cout << "------------------------------------------------------\n";
        std::cout << mpodData << "\n";
        std::cout << "------------------------------------------------------\n";
    }*/
    
    return 0;
}
