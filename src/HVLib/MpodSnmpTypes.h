/***************************************************************************//**
********************************************************************************
**
** @file MpodSnmpTypes.h
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
#ifndef ORCHID_SRC_COMM_MPODSNMPTYPES_H
#define ORCHID_SRC_COMM_MPODSNMPTYPES_H

// includes for C system headers
// includes for C++ system headers
// includes from other libraries
// includes from ORCHID
#include"MpodSnmpUtil.h"


typedef SnmpReadCmd<CrateSubTrees::System, SystemVals, SystemVals::SysMainSwitch>   ReadSysMainSwitch;
typedef SnmpReadCmd<CrateSubTrees::System, SystemVals, SystemVals::SysStatus>       ReadSysStatus;

#endif //ORCHID_SRC_COMM_MPODSNMPTYPES_H
