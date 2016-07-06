/***************************************************************************//**
********************************************************************************
**
** @file MpodReader.h
** @author James Till Matta
** @date 31 May, 2016
** @brief
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** @details Definition file for a class which has functions to explicitly
**   set voltages and the like
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHID_SRC_SLOWCONTROLS_HVLIB_MPODREADER_H
#define ORCHID_SRC_SLOWCONTROLS_HVLIB_MPODREADER_H

// includes for C system headers
// includes for C++ system headers
// includes from other libraries
// includes from ORCHID
#include"SnmpUtilControl.h"
#include"VoltageData.h"
#include"InterThreadComm/Data/SlowData.h"
#include"InputLib/Blocks/MpodChannelData.h"

namespace SlowControls
{
class MpodReader
{
public:
    MpodReader(SnmpUtilControl* snmpCtrl,
               InputParser::MpodChannelData* mpodChannelData):
        snmpControl(snmpCtrl), voltageData(mpodChannelData->board.size()){}
    ~MpodReader(){}
    
    VoltageData voltageData;
    
    void readAll();
    
    void walkTerminalVoltages(){
        this->voltageData.loadTerminalVoltages(
            this->snmpControl->snmpChannelWalk(MpodChannelGetParam::TerminalVoltage));}
    void walkSenseVoltages(){
        this->voltageData.loadSenseVoltages(
            this->snmpControl->snmpChannelWalk(MpodChannelGetParam::SenseVoltage));}
    void walkSetVoltages(){
        this->voltageData.loadSetVoltages(
            this->snmpControl->snmpChannelWalk(MpodChannelGetParam::SetVoltage));}
    void walkCurrents(){
        this->voltageData.loadCurrents(
            this->snmpControl->snmpChannelWalk(MpodChannelGetParam::Current));}
    void walkRampUpRates(){
        this->voltageData.loadRampUpRates(
            this->snmpControl->snmpChannelWalk(MpodChannelGetParam::RampUp));}
    void walkRampDownRates(){
        this->voltageData.loadRampDownRates(
            this->snmpControl->snmpChannelWalk(MpodChannelGetParam::RampDown));}
    void walkMaxCurrents(){
        this->voltageData.loadMaxCurrents(
            this->snmpControl->snmpChannelWalk(MpodChannelGetParam::MaxCurrent));}
    void walkMaxVoltages(){
        this->voltageData.loadMaxVoltages(
            this->snmpControl->snmpChannelWalk(MpodChannelGetParam::MaxTerminalVoltage));}
    void walkCurrentTripTimes(){
        this->voltageData.loadCurrentTripTimes(
            this->snmpControl->snmpChannelWalk(MpodChannelGetParam::MaxCurrentTripTime));}
    void walkTemperatures(){
        this->voltageData.loadTemperatures(
            this->snmpControl->snmpChannelWalk(MpodChannelGetParam::Temperature));}
    void walkOutputSwitches(){
        this->voltageData.loadOutputSwitches(
            this->snmpControl->snmpChannelWalk(MpodChannelGetParam::OutputSwitch));}
    void walkChannelStatuses(){
        this->voltageData.loadChannelStatuses(
            this->snmpControl->snmpChannelWalk(MpodChannelGetParam::OutputStatus));}
    void getCrateStatus(){
        this->voltageData.loadCrateStatus(
            this->snmpControl->snmpGlobalGet(MpodGlobalGetParam::SysStatus));}
    
private:
    SnmpUtilControl* snmpControl;
    InterThread::SlowData* slowData;
};

}
#endif
