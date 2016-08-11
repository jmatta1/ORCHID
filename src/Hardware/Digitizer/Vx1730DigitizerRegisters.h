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

// includes for C system headers
// includes for C++ system headers
#include<map>
#include<utility>
// includes from other libraries
#include<boost/assign/list_of.hpp>
using boost::assign::map_list_of;
// includes from ORCHID

namespace Digitizer
{

namespace LowLvl
{

typedef unsigned short ushort;

enum class Vx1730ReadRegisters : unsigned short
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

static const std::map<Vx1730ReadRegisters, std::pair<ushort, ushort> > VX1730_COUPLE_READ_REGISTERS =
    map_list_of
        (Vx1730ReadRegisters::RecordLength,         std::make_pair(0x1020, 0x0200))
        (Vx1730ReadRegisters::EventsPerAggregate,   std::make_pair(0x1034, 0x0200))
        (Vx1730ReadRegisters::LocalTrgManage,       std::make_pair(0x1084, 0x0200))
        (Vx1730ReadRegisters::TriggerValMask,       std::make_pair(0x8180, 0x0004));

static const std::map<Vx1730ReadRegisters, ushort> VX1730_COMMON_READ_REGISTERS =
    map_list_of
        (Vx1730ReadRegisters::BoardConfig,          0x8000)
        (Vx1730ReadRegisters::AggregateOrg,         0x800C)
        (Vx1730ReadRegisters::ChannelCal,           0x809C)
        (Vx1730ReadRegisters::AcquisitionCtrl,      0x8100)
        (Vx1730ReadRegisters::AcqusitionStatus,     0x8104)
        (Vx1730ReadRegisters::GlobalTrgMask,        0x810C)
        (Vx1730ReadRegisters::TrgOutEnMask,         0x8110)
        (Vx1730ReadRegisters::LvdsData,             0x8118)
        (Vx1730ReadRegisters::FrontIoCtrl,          0x811C)
        (Vx1730ReadRegisters::ChanEnMask,           0x8120)
        (Vx1730ReadRegisters::RocFrmwRev,           0x8124)
        (Vx1730ReadRegisters::SetMonitorDac,        0x8138)
        (Vx1730ReadRegisters::BoardInfo,            0x8140)
        (Vx1730ReadRegisters::MonitorDacMode,       0x8144)
        (Vx1730ReadRegisters::EventSize,            0x814C)
        (Vx1730ReadRegisters::TimeBombCount,        0x8158)
        (Vx1730ReadRegisters::FanSpeedCtrl,         0x8168)
        (Vx1730ReadRegisters::MemBuffAlmtFullLvl,   0x816C)
        (Vx1730ReadRegisters::RunStrtStpDelay,      0x8170)
        (Vx1730ReadRegisters::BoardFailStatus,      0x8178)
        (Vx1730ReadRegisters::DisableExtTrig,       0x817C)
        (Vx1730ReadRegisters::FrontLvdsIoNew,       0x81A0)
        (Vx1730ReadRegisters::ChannelShutdown,      0x81C0)
        (Vx1730ReadRegisters::ReadoutCtrl,          0xEF00)
        (Vx1730ReadRegisters::ReadoutStatus,        0xEF04)
        (Vx1730ReadRegisters::BoardId,              0xEF08)
        (Vx1730ReadRegisters::McstBaseAddrCtrl,     0xEF0C)
        (Vx1730ReadRegisters::RelocationAddr,       0xEF10)
        (Vx1730ReadRegisters::InterruptStatID,      0xEF14)
        (Vx1730ReadRegisters::InterruptEventNum,    0xEF18)
        (Vx1730ReadRegisters::AggregateNumPerBlt,   0xEF1C)
        (Vx1730ReadRegisters::Scratch,              0xEF20)
        (Vx1730ReadRegisters::ConfigRomChcksm,      0xF000)
        (Vx1730ReadRegisters::ConfigRomChcksmL2,    0xF004)
        (Vx1730ReadRegisters::ConfigRomChcksmL1,    0xF008)
        (Vx1730ReadRegisters::ConfigRomChcksmL0,    0xF00C)
        (Vx1730ReadRegisters::ConfigRomConst2,      0xF010)
        (Vx1730ReadRegisters::ConfigRomConst1,      0xF014)
        (Vx1730ReadRegisters::ConfigRomConst0,      0xF018)
        (Vx1730ReadRegisters::ConfigRomCcode,       0xF01C)
        (Vx1730ReadRegisters::ConfigRomRcode,       0xF020)
        (Vx1730ReadRegisters::ConfigRomManuId2,     0xF024)
        (Vx1730ReadRegisters::ConfigRomManuId1,     0xF028)
        (Vx1730ReadRegisters::ConfigRomManuId0,     0xF02C)
        (Vx1730ReadRegisters::ConfigRomBrdVer,      0xF030)
        (Vx1730ReadRegisters::ConfigRomBrdFf,       0xF034)
        (Vx1730ReadRegisters::ConfigRomBrdNum1,     0xF038)
        (Vx1730ReadRegisters::ConfigRomBrdNum0,     0xF03C)
        (Vx1730ReadRegisters::ConfigRomRevis3,      0xF040)
        (Vx1730ReadRegisters::ConfigRomRevis2,      0xF044)
        (Vx1730ReadRegisters::ConfigRomRevis1,      0xF048)
        (Vx1730ReadRegisters::ConfigRomPcbRev,      0xF04C)
        (Vx1730ReadRegisters::ConfigRomFlshTp,      0xF050)
        (Vx1730ReadRegisters::ConfigRomBrdSn1,      0xF080)
        (Vx1730ReadRegisters::ConfigRomBrdSn0,      0xF084)
        (Vx1730ReadRegisters::ConfigRomVxcoTp,      0xF088);

static const std::map<Vx1730ReadRegisters, std::pair<ushort, ushort> > VX1730_INDIV_READ_REGISTERS =
    map_list_of
        (Vx1730ReadRegisters::InputDynamicRange,    std::make_pair(0x1028, 0x0100))
        (Vx1730ReadRegisters::PreTrg,               std::make_pair(0x1038, 0x0100))
        (Vx1730ReadRegisters::CfdSettings,          std::make_pair(0x103C, 0x0100))
        (Vx1730ReadRegisters::ForcedDataFlush,      std::make_pair(0x1040, 0x0100))
        (Vx1730ReadRegisters::ShortGate,            std::make_pair(0x1054, 0x0100))
        (Vx1730ReadRegisters::LongGate,             std::make_pair(0x1058, 0x0100))
        (Vx1730ReadRegisters::GateOffset,           std::make_pair(0x105C, 0x0100))
        (Vx1730ReadRegisters::TrgThreshold,         std::make_pair(0x1060, 0x0100))
        (Vx1730ReadRegisters::FixedBaseline,        std::make_pair(0x1064, 0x0100))
        (Vx1730ReadRegisters::ShapedTrgWidth,       std::make_pair(0x1070, 0x0100))
        (Vx1730ReadRegisters::TrgHoldOff,           std::make_pair(0x1074, 0x0100))
        (Vx1730ReadRegisters::PsdCutThreshold,      std::make_pair(0x1078, 0x0100))
        (Vx1730ReadRegisters::DppAlgorithmCtrl,     std::make_pair(0x1080, 0x0100))
        (Vx1730ReadRegisters::ChannelStatus,        std::make_pair(0x1088, 0x0100))
        (Vx1730ReadRegisters::AmcFirmwareRev,       std::make_pair(0x108C, 0x0100))
        (Vx1730ReadRegisters::DcOffset,             std::make_pair(0x1098, 0x0100))
        (Vx1730ReadRegisters::AdcTemperature,       std::make_pair(0x10A8, 0x0100))
        (Vx1730ReadRegisters::IndivSoftwTrig,       std::make_pair(0x10C0, 0x0100))
        (Vx1730ReadRegisters::VetoExtension,        std::make_pair(0x10D4, 0x0100));

enum class Vx1730WriteRegisters : unsigned short
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

static const std::map<Vx1730WriteRegisters, std::pair<ushort, ushort> > VX1730_COMMON_WRITE_REGISTERS =
    map_list_of
        (Vx1730WriteRegisters::BoardConfig,         0x8000)
        (Vx1730WriteRegisters::AggregateOrg,        0x809C)
        (Vx1730WriteRegisters::AcquisitionCtrl,     0x8100)
        (Vx1730WriteRegisters::SoftwareTrg,         0x8108)
        (Vx1730WriteRegisters::GlobalTrgMask,       0x810C)
        (Vx1730WriteRegisters::TrgOutEnMask,        0x8110)
        (Vx1730WriteRegisters::LvdsData,            0x8118)
        (Vx1730WriteRegisters::FrontIoCtrl,         0x811C)
        (Vx1730WriteRegisters::ChanEnMask,          0x8120)
        (Vx1730WriteRegisters::SetMonitorDac,       0x8138)
        (Vx1730WriteRegisters::SoftwClckSync,       0x813C)
        (Vx1730WriteRegisters::MonitorDacMode,      0x8144)
        (Vx1730WriteRegisters::FanSpeedCtrl,        0x8168)
        (Vx1730WriteRegisters::MemBuffAlmtFullLvl,  0x816C)
        (Vx1730WriteRegisters::RunStrtStpDelay,     0x8170)
        (Vx1730WriteRegisters::DisableExtTrig,      0x817C)
        (Vx1730WriteRegisters::FrontLvdsIoNew,      0x81A0)
        (Vx1730WriteRegisters::ChannelShutdown,     0x81C0)
        (Vx1730WriteRegisters::ReadoutCtrl,         0xEF00)
        (Vx1730WriteRegisters::BoardId,             0xEF08)
        (Vx1730WriteRegisters::McstBaseAddrCtrl,    0xEF0C)
        (Vx1730WriteRegisters::RelocationAddr,      0xEF10)
        (Vx1730WriteRegisters::InterruptStatID,     0xEF14)
        (Vx1730WriteRegisters::InterruptEventNum,   0xEF18)
        (Vx1730WriteRegisters::AggregateNumPerBlt,  0xEF1C)
        (Vx1730WriteRegisters::Scratch,             0xEF20)
        (Vx1730WriteRegisters::SoftwReset,          0xEF24)
        (Vx1730WriteRegisters::SoftwClear,          0xEF28)
        (Vx1730WriteRegisters::ConfigReload,        0xEF34);

static const std::map<Vx1730WriteRegisters, std::pair<ushort, ushort> > VX1730_INDV_WRITE_REGISTERS =
    map_list_of
        (Vx1730WriteRegisters::InputDynamicRange,   std::make_pair(0x1028, 0x0100))
        (Vx1730WriteRegisters::PreTrg,              std::make_pair(0x1038, 0x0100))
        (Vx1730WriteRegisters::CfdSettings,         std::make_pair(0x103C, 0x0100))
        (Vx1730WriteRegisters::ForcedDataFlush,     std::make_pair(0x1040, 0x0100))
        (Vx1730WriteRegisters::ShortGate,           std::make_pair(0x1054, 0x0100))
        (Vx1730WriteRegisters::LongGate,            std::make_pair(0x1058, 0x0100))
        (Vx1730WriteRegisters::GateOffset,          std::make_pair(0x105C, 0x0100))
        (Vx1730WriteRegisters::TrgThreshold,        std::make_pair(0x1060, 0x0100))
        (Vx1730WriteRegisters::FixedBaseline,       std::make_pair(0x1064, 0x0100))
        (Vx1730WriteRegisters::ShapedTrgWidth,      std::make_pair(0x1070, 0x0100))
        (Vx1730WriteRegisters::TrgHoldOff,          std::make_pair(0x1074, 0x0100))
        (Vx1730WriteRegisters::PsdCutThreshold,     std::make_pair(0x1078, 0x0100))
        (Vx1730WriteRegisters::DppAlgorithmCtrl,    std::make_pair(0x1080, 0x0100))
        (Vx1730WriteRegisters::DcOffset,            std::make_pair(0x1098, 0x0100))
        (Vx1730WriteRegisters::IndivSoftwTrig,      std::make_pair(0x10C0, 0x0100))
        (Vx1730WriteRegisters::VetoExtension,       std::make_pair(0x10D4, 0x0100));

static const std::map<Vx1730WriteRegisters, ushort> VX1730_IBROADCAST_WRITE_REGISTERS =
    map_list_of
        (Vx1730WriteRegisters::InputDynamicRange,   0x8028)
        (Vx1730WriteRegisters::PreTrg,              0x8038)
        (Vx1730WriteRegisters::CfdSettings,         0x803C)
        (Vx1730WriteRegisters::ForcedDataFlush,     0x8040)
        (Vx1730WriteRegisters::ShortGate,           0x8054)
        (Vx1730WriteRegisters::LongGate,            0x8058)
        (Vx1730WriteRegisters::GateOffset,          0x805C)
        (Vx1730WriteRegisters::TrgThreshold,        0x8060)
        (Vx1730WriteRegisters::FixedBaseline,       0x8064)
        (Vx1730WriteRegisters::ShapedTrgWidth,      0x8070)
        (Vx1730WriteRegisters::TrgHoldOff,          0x8074)
        (Vx1730WriteRegisters::PsdCutThreshold,     0x8078)
        (Vx1730WriteRegisters::DppAlgorithmCtrl,    0x8080)
        (Vx1730WriteRegisters::DcOffset,            0x8098)
        (Vx1730WriteRegisters::IndivSoftwTrig,      0x80C0)
        (Vx1730WriteRegisters::VetoExtension,       0x80D4);

static const std::map<Vx1730WriteRegisters, std::pair<ushort, ushort> > VX1730_GROUP_WRITE_REGISTERS =
    map_list_of
        (Vx1730WriteRegisters::RecordLength,        std::make_pair(0x1020, 0x0200))
        (Vx1730WriteRegisters::EventsPerAggregate,  std::make_pair(0x1034, 0x0200))
        (Vx1730WriteRegisters::LocalTrgManage,      std::make_pair(0x1084, 0x0200))
        (Vx1730WriteRegisters::TriggerValMask,      std::make_pair(0x8180, 0x0004));

static const std::map<Vx1730WriteRegisters, ushort> VX1730_GBROADCAST_WRITE_REGISTERS =
    map_list_of
        (Vx1730WriteRegisters::RecordLength,        0x8020)
        (Vx1730WriteRegisters::EventsPerAggregate,  0x8034)
        (Vx1730WriteRegisters::LocalTrgManage,      0x8084);
}

}

#endif //ORCHID_SRC_HARDWARE_DIGITIZER_DIGITIZERREGISTERS_H
