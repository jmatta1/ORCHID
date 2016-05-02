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
#include"HVLib/SnmpUtilControl.h"
#include<thread>
#include<chrono>

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
    
    std::cout << "\n\n" << std::endl;
    
    SnmpUtilControl mpodControl(params.powerBlock->mpodIpAddress,
                                params.powerBlock->weinerMibFileDirectory);
    
    std::chrono::seconds longDuration(8);
    std::chrono::seconds shortDuration(1);
    
    std::cout << "Starting the MPOD Crate" <<std::endl;
    
    std::cout << mpodControl.snmpGlobalSet(MpodGlobalSetParam::SysMainSwitch, 1) << std::endl;
    
    std::this_thread::sleep_for(longDuration);
    
    std::cout << "Setting Voltages" << std::endl;
    
    for(int i=0; i<mpodChannelData.board.size(); ++i)
    {
        if(mpodChannelData.online[i])
        {
            std::cout << mpodControl.snmpChannelSet(MpodChannelSetParam::SetVoltage,
                                                    mpodChannelData.board[i],
                                                    mpodChannelData.channel[i],
                                                    mpodChannelData.voltage[i]) << std::flush;
        }
    }
    
    std::this_thread::sleep_for(longDuration);
    
    std::cout << "Switching channels to on" << std::endl;
    
    for(int i=0; i<mpodChannelData.board.size(); ++i)
    {
        if(mpodChannelData.online[i])
        {
            std::cout << mpodControl.snmpChannelSet(MpodChannelSetParam::OutputSwitch,
                                                    mpodChannelData.board[i],
                                                    mpodChannelData.channel[i],
                                                    1) << std::endl;
        }
    }
    
    std::cout << "Watching ramp up" << std::endl;
    
    for(int j=0; j<45; ++j)
    {
        
        std::cout << "Ramp Up step: " << j << std::endl;
        std::cout << "Voltages: " << j << std::endl;
        std::cout << mpodControl.snmpChannelWalk(MpodChannelGetParam::TerminalVoltage) << std::endl;
        std::cout << "Statuses: " << j << std::endl;
        std::cout << mpodControl.snmpChannelWalk(MpodChannelGetParam::OutputStatus) << std::endl;
        
        std::this_thread::sleep_for(shortDuration);
    }
    
    std::this_thread::sleep_for(longDuration);
    
    std::cout << "Switching channels to off" << std::endl;
    
    for(int i=0; i<mpodChannelData.board.size(); ++i)
    {
        if(mpodChannelData.online[i])
        {
            std::cout << mpodControl.snmpChannelSet(MpodChannelSetParam::OutputSwitch,
                                                    mpodChannelData.board[i],
                                                    mpodChannelData.channel[i],
                                                    0) << std::flush;
        }
    }
    
    std::cout << "Watching ramp down" << std::endl;
    
    for(int j=0; j<45; ++j)
    {
        
        std::cout << "Ramp down step: " << j << std::endl;
        std::cout << "Voltages: " << j << std::endl;
        std::cout << mpodControl.snmpChannelWalk(MpodChannelGetParam::TerminalVoltage) << std::endl;
        std::cout << "Statuses: " << j << std::endl;
        std::cout << mpodControl.snmpChannelWalk(MpodChannelGetParam::OutputStatus) << std::endl;
        
        std::this_thread::sleep_for(shortDuration);
    }
    
    std::cout << "Resetting Voltages" << std::endl;
    
    for(int i=0; i<mpodChannelData.board.size(); ++i)
    {
        if(mpodChannelData.online[i])
        {
            std::cout << mpodControl.snmpChannelSet(MpodChannelSetParam::SetVoltage,
                                                    mpodChannelData.board[i],
                                                    mpodChannelData.channel[i],
                                                    0.0) << std::flush;
        }
    }
    
    std::this_thread::sleep_for(longDuration);
    
    std::cout << "Shutting Down MPOD Crate" <<std::endl;
    
    std::cout << mpodControl.snmpGlobalSet(MpodGlobalSetParam::SysMainSwitch, 0) << std::endl;
    
    return 0;
}

