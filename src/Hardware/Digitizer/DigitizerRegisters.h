/***************************************************************************//**
********************************************************************************
**
** @file DigitizerRegisters.h
** @author James Till Matta
** @date 27 June, 2016
** @brief
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** @details Holds enumerations for the DPP_PSD registers
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHID_SRC_HARDWARE_DIGITIZER_DIGITIZERREGISTERS_H
#define ORCHID_SRC_HARDWARE_DIGITIZER_DIGITIZERREGISTERS_H

namespace Data
{

namespace LowLvl
{

enum class ReadRegisters : short
  { RecordLength,       InputDynamicRange,  EventsPerAggregate, PreTrg,
    CfdSettings,        ForcedDataFlush,    ShortGate,          LongGate,
    GateOffset,         TrgThreshold,       FixedBaseline,      ShapedTrgWidth,
    TrgHoldOff,         PsdCutThreshold,    DppAlgorithmCtrl,   LocalTrgManage,
    ChannelStatus,      AmcFirmwareRev,     DcOffset,           AdcTemperature,
    IndivSoftwTrig,     VetoExtension,      BoardConfig,        AggregateOrg,
    ChannelCal,         AcquisitionCtrl,    AcqusitionStatus,   GlobalTrgMask,
    TrgOutEnMask,       LvdsData,           FrontIoCtrl,        ChanEnMask,
    RocFrmwRev,         SetMonitorDac,      BoardInfo,          MonitorDacMode,
    EventSize,          TimeBombCount,      FanSpeedCtrl,       MemBuffAlmtFullLvl,
    RunStrtStpDelay,    BoardFailStatus,    DisableExtTrig,     TriggerValMask,
    FrontLvdsIoNew,     ChannelShutdown,    ReadoutCtrl,        ReadoutStatus,
    BoardId,            McstBaseAddrCtrl,   RelocationAddr,     InterruptStatID,
    InterruptEventNum,  AggregateNumPerBlt, Scratch,            ConfigRomChcksm,
    ConfigRomChcksmL2,  ConfigRomChcksmL1,  ConfigRomChcksmL0,  ConfigRomConst2,
    ConfigRomConst1,    ConfigRomConst0,    ConfigRomCcode,     ConfigRomRcode,
    ConfigRomManuId2,   ConfigRomManuId1,   ConfigRomManuId0,   ConfigRomBrdVer,
    ConfigRomBrdFf,     ConfigRomBrdNum1,   ConfigRomBrdNum0,   ConfigRomRevis3,
    ConfigRomRevis2,    ConfigRomRevis1,    ConfigRomPcbRev,    ConfigRomFlshTp,
    ConfigRomBrdSn1,    ConfigRomBrdSn0,    ConfigRomVxcoTp};

enum class WriteRegisters : short
  { RecordLength,       InputDynamicRange,  EventsPerAggregate, PreTrg,
    CfdSettings,        ForcedDataFlush,    ShortGate,          LongGate,
    GateOffset,         TrgThreshold,       FixedBaseline,      ShapedTrgWidth,
    TrgHoldOff,         PsdCutThreshold,    DppAlgorithmCtrl,   LocalTrgManage,
    DcOffset,           IndivSoftwTrig,     VetoExtension,      BoardConfig,
    AggregateOrg,       AcquisitionCtrl,    SoftwareTrg,        GlobalTrgMask,
    TrgOutEnMask,       LvdsData,           FrontIoCtrl,        ChanEnMask,
    SetMonitorDac,      SoftwClckSync,      MonitorDacMode,     FanSpeedCtrl,
    MemBuffAlmtFullLvl, RunStrtStpDelay,    DisableExtTrig,     TriggerValMask,
    FrontLvdsIoNew,     ChannelShutdown,    ReadoutCtrl,        BoardId,
    McstBaseAddrCtrl,   RelocationAddr,     InterruptStatID,    InterruptEventNum,
    AggregateNumPerBlt, Scratch,            SoftwReset,         SoftwClear,
    ConfigReload};
}



}

#endif //ORCHID_SRC_HARDWARE_DIGITIZER_DIGITIZERREGISTERS_H
