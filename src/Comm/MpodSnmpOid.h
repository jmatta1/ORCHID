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
** @details Holds the template system that builds the command strings
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHID_SRC_COMM_MPODSNMPOID_H
#define ORCHID_SRC_COMM_MPODSNMPOID_H

// includes for C system headers
// includes for C++ system headers
// includes from other libraries
// includes from ORCHID
#include"Utility/TmpStrConcat.h"

enum class CrateSubTrees : char
{        System = 1,       Input = 2,  Output = 3, Sensor = 4,
  Communication = 5, PowerSupply = 6, FanTray = 7,   Rack = 8,
         Signal = 9};

enum class SystemVals : short
{     SysMainSwitch =    1,                     SysStatus =    2,   SysVmeSysReset =    3, SysHardwareReset =    4,
 SysFactoryDefaults =    5, SysConfigDoMeasurementCurrent =   10, SysOperatingTime =   11,  SysDebugMemory8 = 1024,
   SysDebugMemory16 = 1025,              SysDebugMemory32 = 1026,         SysDebug = 1027,  SysDebugDisplay = 1028,
       SysDebugBoot = 1029};

enum class OutputVals : 
{   OutputNumber = 1, OutputTable = 2, GroupsNumber = 3, GroupsTable = 4,
    ModuleNumber = 5, ModuleTable = 6}; 

static const char* WienerCrateOid = ".1.3.6.1.4.1.19947.1";

#endif //ORCHID_SRC_COMM_MPODSNMPOID_H
