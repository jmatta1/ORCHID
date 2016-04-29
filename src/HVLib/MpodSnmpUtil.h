/***************************************************************************//**
********************************************************************************
**
** @file MpodSnmpOid.h
** @author James Till Matta
** @date 29 Apr, 2016
** @brief
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** @details Holds the template system that builds the snmp command strings
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHID_SRC_COMM_MPODSNMPOID_H
#define ORCHID_SRC_COMM_MPODSNMPOID_H

// includes for C system headers
// includes for C++ system headers
// includes from other libraries
// includes from ORCHID
#include"Utility/TmpStringTricks.h"

//this enumeration stores the list of valid subtrees for the crates
enum class CrateSubTrees : int
{        System = 1,       Input = 2,  Output = 3, Sensor = 4,
  Communication = 5, PowerSupply = 6, FanTray = 7,   Rack = 8,
         Signal = 9};

// here is a list of valid options for the system subtree
enum class SystemVals : int
{     SysMainSwitch =    1,                     SysStatus =    2,   SysVmeSysReset =    3, SysHardwareReset =    4,
 SysFactoryDefaults =    5, SysConfigDoMeasurementCurrent =   10, SysOperatingTime =   11,  SysDebugMemory8 = 1024,
   SysDebugMemory16 = 1025,              SysDebugMemory32 = 1026,         SysDebug = 1027,  SysDebugDisplay = 1028,
       SysDebugBoot = 1029};

//this subtree was reserved but not used in the MIB file, thus its existence here
enum class InputVals : int
{};

// here is a list of the valid options for the options subtree
enum class OutputVals : int
{   OutputNumber = 1, OutputTable = 2, GroupsNumber = 3, GroupsTable = 4,
    ModuleNumber = 5, ModuleTable = 6};

// list of valid options for the sensor subtree
enum class SensorVals : int
{   SensorNumber = 1, SensorTable = 2};

// list of valid options for the communications subtree
enum class CommunicationVals : int
{   SNMP = 1, Can = 2};

// list of valid options for the power supply subtree
enum class PowerSupplyVals : int
{   PsSerialNumber = 2, PsOperatingTime =    3, PsAuxiliaryNumber = 4,
  PsAuxiliaryTable = 5,  PsDirectAccess = 1024};

// list of valid options for the fan subtree
enum class FantrayVals : int
{   FanSerialNumber =  2,  FanOperatingTime =  3, FanAirTemperature =  4,
  FanSwitchOffDelay =  5,   FanNominalSpeed =  6,   FanNumberOfFans =  7,
      FanSpeedTable =  8,       FanMaxSpeed =  9,       FanMinSpeed = 10,
  FanConfigMaxSpeed = 11, FanConfigMinSpeed = 12};

//this subtree was reserved but not used in the MIB file
enum class RackVals : int
{};

enum class SignaVals : int
{   NumberOfAnalogInputs = 1, AnalogInputTable = 2, DigitalInput = 5,
           DigitalOutput = 6};

static constexpr const char WienerCrateOid[] = ".1.3.6.1.4.1.19947.1";

static constexpr const char PtOne[]        = ".1";
static constexpr const char PtTwo[]        = ".2";
static constexpr const char PtThree[]      = ".3";
static constexpr const char PtFour[]       = ".4";
static constexpr const char PtFive[]       = ".5";
static constexpr const char PtSix[]        = ".6";
static constexpr const char PtSeven[]      = ".7";
static constexpr const char PtEight[]      = ".8";
static constexpr const char PtNine[]       = ".9";
static constexpr const char PtTen[]        = ".10";
static constexpr const char PtEleven[]     = ".11";
static constexpr const char PtTwelve[]     = ".12";
static constexpr const char PtThousand24[] = ".1024";
static constexpr const char PtThousand25[] = ".1025";
static constexpr const char PtThousand26[] = ".1026";
static constexpr const char PtThousand27[] = ".1027";
static constexpr const char PtThousand28[] = ".1028";
static constexpr const char PtThousand29[] = ".1029";


template<CrateSubTrees subTree, typename SubTreeType, SubTreeType val>
struct SnmpReadCmd;

template<> struct SnmpReadCmd<CrateSubTrees::System, SystemVals, SystemVals::SysMainSwitch>
{static constexpr const char* cmd() {return AppendStrings<WienerCrateOid, 20, AppendStrings<PtOne, 2, PtOne, 2>::value(), 4>::value();}};
/*
template<> struct SnmpReadCmd<CrateSubTrees::System, SystemVals, SystemVals::SysStatus>
{static constexpr const char* cmd() {return AppendStrings<WienerCrateOid, 20, AppendStrings<PtOne, 2, PtTwo, 2>::value(), 4>::value();}};

template<> struct SnmpReadCmd<CrateSubTrees::System, SystemVals, SystemVals::SysVmeSysReset>
{static constexpr const char* cmd() {return AppendStrings<WienerCrateOid, 20, AppendStrings<PtOne, 2, PtThree, 2>::value(), 4>::value();}};

template<> struct SnmpReadCmd<CrateSubTrees::System, SystemVals, SystemVals::SysHardwareReset>
{static constexpr const char* cmd() {return AppendStrings<WienerCrateOid, 20, AppendStrings<PtOne, 2, PtFour, 2>::value(), 4>::value();}};

template<> struct SnmpReadCmd<CrateSubTrees::System, SystemVals, SystemVals::SysFactoryDefaults>
{static constexpr const char* cmd() {return AppendStrings<WienerCrateOid, 20, AppendStrings<PtOne, 2, PtFive, 2>::value(), 4>::value();}};

template<> struct SnmpReadCmd<CrateSubTrees::System, SystemVals, SystemVals::SysConfigDoMeasurementCurrent>
{static constexpr const char* cmd() {return AppendStrings<WienerCrateOid, 20, AppendStrings<PtOne, 2, PtTen, 3>::value(), 5>::value();}};

template<> struct SnmpReadCmd<CrateSubTrees::System, SystemVals, SystemVals::SysOperatingTime>
{static constexpr const char* cmd() {return AppendStrings<WienerCrateOid, 20, AppendStrings<PtOne, 2, PtEleven, 3>::value(), 5>::value();}};

template<> struct SnmpReadCmd<CrateSubTrees::System, SystemVals, SystemVals::SysDebugMemory8>
{static constexpr const char* cmd() {return AppendStrings<WienerCrateOid, 20, AppendStrings<PtOne, 2, PtThousand24, 5>::value(), 7>::value();}};

template<> struct SnmpReadCmd<CrateSubTrees::System, SystemVals, SystemVals::SysDebugMemory16>
{static constexpr const char* cmd() {return AppendStrings<WienerCrateOid, 20, AppendStrings<PtOne, 2, PtThousand25, 5>::value(), 7>::value();}};

template<> struct SnmpReadCmd<CrateSubTrees::System, SystemVals, SystemVals::SysDebugMemory32>
{static constexpr const char* cmd() {return AppendStrings<WienerCrateOid, 20, AppendStrings<PtOne, 2, PtThousand26, 5>::value(), 7>::value();}};

template<> struct SnmpReadCmd<CrateSubTrees::System, SystemVals, SystemVals::SysDebug>
{static constexpr const char* cmd() {return AppendStrings<WienerCrateOid, 20, AppendStrings<PtOne, 2, PtThousand27, 5>::value(), 7>::value();}};

template<> struct SnmpReadCmd<CrateSubTrees::System, SystemVals, SystemVals::SysDebugDisplay>
{static constexpr const char* cmd() {return AppendStrings<WienerCrateOid, 20, AppendStrings<PtOne, 2, PtThousand28, 5>::value(), 7>::value();}};

template<> struct SnmpReadCmd<CrateSubTrees::System, SystemVals, SystemVals::SysDebugBoot>
{static constexpr const char* cmd() {return AppendStrings<WienerCrateOid, 20, AppendStrings<PtOne, 2, PtThousand29, 5>::value(), 7>::value();}};*/
#endif //ORCHID_SRC_COMM_MPODSNMPOID_H
