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
#include"Utility/TitleString.h"
#include"Utility/ParseAndValidate.h"
#include"InputLib/InputLib.h"
#include"HVLib/MpodSnmpUtil.h"
#include"Comm/SnmpConnection.h"

void snmpPrint(std::string result, int num)
{
    std::cout << "result " << result << std::endl;
    std::cout << "num " << num << std::endl;
}

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
    
    std::cout << "\n\nTesting template metaprogramming: \n"
              << SnmpReadCmd<CrateSubTrees::System, SystemVals, SystemVals::SysMainSwitch>::cmd()
              << "\n\n" << std::endl;
    
    boost::asio::io_service io_service;
    SnmpConnection snmpConn(io_service);
    std::cout << "Build snmp con" << std::endl;
    snmpConn.connect(SNMP_VERSION_2c,string("192.168.11.11"), string("guru"));
    std::cout << "connected" << std::endl;
    snmpConn.asyncSnmpGet(string(".1.3.6.1.4.1.19947.1.1.1.0"), boost::bind(snmpPrint,_1,3));
    std::cout << "running call" << std::endl;
    io_service.run();
    std::cout << "done" << std::endl;
    return 0;
}

