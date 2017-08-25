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
#include<utility>
#include<type_traits>
// includes from other libraries
// includes from ORCHID

namespace Digitizer
{

namespace LowLvl
{

typedef unsigned short ushort;

enum class Vx1730ReadRegisters : unsigned short
  { EventReadout, Dummy32, ChannelDynamicRange, ChannelPulseWidth,
    ChannelTriggerThreshold, CoupleSelfTriggerLogic, ChannelStatus,
    AmcFirmwareRevision, ChannelDcOffset, ChannelAdcTemp, BoardConfig,
    BufferOrg, CustomSize, AcqControl, AcqStatus, GlobalTriggerMask,
    FrontPanelTrgOutEnableMask, PostTrigger, LvdsIoData, FrontPanelIoCtrl,
    ChannelEnableMask, RocFirmwareRevision, EventStored, SetMonitorDac,
    BoardInfo, MonitorDacMode, EventSize, FanSpeedCtrl,
    MemoryBufferAlmostFullLevel, RunStartStopDelay, BoardFailureStatus,
    FrontPanelLvdsIoNewFeatures, ReadoutCtrl, ReadoutStatus, BoardId,
    McstBaseAddrAndCtrl, RelocationAddress, InterruptStatusId,
    InterruptEventNumber, MaxEventsPerBlt, Scratch};

template<Vx1730ReadRegisters> struct Vx1730CoupleReadRegistersAddr;
//the offset is 0x0200 for the one group read register
template<Vx1730ReadRegisters> struct Vx1730CoupleReadRegistersOffs { static constexpr ushort value = 0x0200;};
//now define addresses
template<> struct Vx1730CoupleReadRegistersAddr<Vx1730ReadRegisters::CoupleSelfTriggerLogic>        : std::integral_constant<ushort, 0x1084>{};


template<Vx1730ReadRegisters> struct Vx1730CommonReadRegistersAddr;
template<> struct Vx1730CommonReadRegistersAddr<Vx1730ReadRegisters::EventReadout>                  : std::integral_constant<ushort, 0x0000> {};
template<> struct Vx1730CommonReadRegistersAddr<Vx1730ReadRegisters::BoardConfig>                   : std::integral_constant<ushort, 0x8000> {};
template<> struct Vx1730CommonReadRegistersAddr<Vx1730ReadRegisters::BufferOrg>                     : std::integral_constant<ushort, 0x8020> {};
template<> struct Vx1730CommonReadRegistersAddr<Vx1730ReadRegisters::CustomSize>                    : std::integral_constant<ushort, 0x809C> {};
template<> struct Vx1730CommonReadRegistersAddr<Vx1730ReadRegisters::AcqControl>                    : std::integral_constant<ushort, 0x8100> {};
template<> struct Vx1730CommonReadRegistersAddr<Vx1730ReadRegisters::AcqStatus>                     : std::integral_constant<ushort, 0x8104> {};
template<> struct Vx1730CommonReadRegistersAddr<Vx1730ReadRegisters::GlobalTriggerMask>             : std::integral_constant<ushort, 0x810C> {};
template<> struct Vx1730CommonReadRegistersAddr<Vx1730ReadRegisters::FrontPanelTrgOutEnableMask>    : std::integral_constant<ushort, 0x8110> {};
template<> struct Vx1730CommonReadRegistersAddr<Vx1730ReadRegisters::PostTrigger>                   : std::integral_constant<ushort, 0x8114> {};
template<> struct Vx1730CommonReadRegistersAddr<Vx1730ReadRegisters::LvdsIoData>                    : std::integral_constant<ushort, 0x8118> {};
template<> struct Vx1730CommonReadRegistersAddr<Vx1730ReadRegisters::FrontPanelIoCtrl>              : std::integral_constant<ushort, 0x811C> {};
template<> struct Vx1730CommonReadRegistersAddr<Vx1730ReadRegisters::ChannelEnableMask>             : std::integral_constant<ushort, 0x8120> {};
template<> struct Vx1730CommonReadRegistersAddr<Vx1730ReadRegisters::RocFirmwareRevision>           : std::integral_constant<ushort, 0x8124> {};
template<> struct Vx1730CommonReadRegistersAddr<Vx1730ReadRegisters::EventStored>                   : std::integral_constant<ushort, 0x812C> {};
template<> struct Vx1730CommonReadRegistersAddr<Vx1730ReadRegisters::SetMonitorDac>                 : std::integral_constant<ushort, 0x8138> {};
template<> struct Vx1730CommonReadRegistersAddr<Vx1730ReadRegisters::BoardInfo>                     : std::integral_constant<ushort, 0x8140> {};
template<> struct Vx1730CommonReadRegistersAddr<Vx1730ReadRegisters::MonitorDacMode>                : std::integral_constant<ushort, 0x8144> {};
template<> struct Vx1730CommonReadRegistersAddr<Vx1730ReadRegisters::EventSize>                     : std::integral_constant<ushort, 0x814C> {};
template<> struct Vx1730CommonReadRegistersAddr<Vx1730ReadRegisters::FanSpeedCtrl>                  : std::integral_constant<ushort, 0x8168> {};
template<> struct Vx1730CommonReadRegistersAddr<Vx1730ReadRegisters::MemoryBufferAlmostFullLevel>   : std::integral_constant<ushort, 0x816C> {};
template<> struct Vx1730CommonReadRegistersAddr<Vx1730ReadRegisters::RunStartStopDelay>             : std::integral_constant<ushort, 0x8170> {};
template<> struct Vx1730CommonReadRegistersAddr<Vx1730ReadRegisters::BoardFailureStatus>            : std::integral_constant<ushort, 0x8178> {};
template<> struct Vx1730CommonReadRegistersAddr<Vx1730ReadRegisters::FrontPanelLvdsIoNewFeatures>   : std::integral_constant<ushort, 0x81A0> {};
template<> struct Vx1730CommonReadRegistersAddr<Vx1730ReadRegisters::ReadoutCtrl>                   : std::integral_constant<ushort, 0xEF00> {};
template<> struct Vx1730CommonReadRegistersAddr<Vx1730ReadRegisters::ReadoutStatus>                 : std::integral_constant<ushort, 0xEF04> {};
template<> struct Vx1730CommonReadRegistersAddr<Vx1730ReadRegisters::BoardId>                       : std::integral_constant<ushort, 0xEF08> {};
template<> struct Vx1730CommonReadRegistersAddr<Vx1730ReadRegisters::McstBaseAddrAndCtrl>           : std::integral_constant<ushort, 0xEF0C> {};
template<> struct Vx1730CommonReadRegistersAddr<Vx1730ReadRegisters::RelocationAddress>             : std::integral_constant<ushort, 0xEF10> {};
template<> struct Vx1730CommonReadRegistersAddr<Vx1730ReadRegisters::InterruptStatusId>             : std::integral_constant<ushort, 0xEF14> {};
template<> struct Vx1730CommonReadRegistersAddr<Vx1730ReadRegisters::InterruptEventNumber>          : std::integral_constant<ushort, 0xEF18> {};
template<> struct Vx1730CommonReadRegistersAddr<Vx1730ReadRegisters::MaxEventsPerBlt>               : std::integral_constant<ushort, 0xEF1C> {};
template<> struct Vx1730CommonReadRegistersAddr<Vx1730ReadRegisters::Scratch>                       : std::integral_constant<ushort, 0xEF20> {};


template<Vx1730ReadRegisters> struct Vx1730IndivReadRegistersAddr;
//the offsets are always 0x0100
template<Vx1730ReadRegisters> struct Vx1730IndivReadRegistersOffs { static constexpr ushort value = 0x0100;};
//now define addresses
template<> struct Vx1730IndivReadRegistersAddr<Vx1730ReadRegisters::Dummy32>                        : std::integral_constant<ushort, 0x1024> {};
template<> struct Vx1730IndivReadRegistersAddr<Vx1730ReadRegisters::ChannelDynamicRange>            : std::integral_constant<ushort, 0x1028> {};
template<> struct Vx1730IndivReadRegistersAddr<Vx1730ReadRegisters::ChannelPulseWidth>              : std::integral_constant<ushort, 0x1070> {};
template<> struct Vx1730IndivReadRegistersAddr<Vx1730ReadRegisters::ChannelTriggerThreshold>        : std::integral_constant<ushort, 0x1080> {};
template<> struct Vx1730IndivReadRegistersAddr<Vx1730ReadRegisters::ChannelStatus>                  : std::integral_constant<ushort, 0x1088> {};
template<> struct Vx1730IndivReadRegistersAddr<Vx1730ReadRegisters::AmcFirmwareRevision>            : std::integral_constant<ushort, 0x108C> {};
template<> struct Vx1730IndivReadRegistersAddr<Vx1730ReadRegisters::ChannelDcOffset>                : std::integral_constant<ushort, 0x1098> {};
template<> struct Vx1730IndivReadRegistersAddr<Vx1730ReadRegisters::ChannelAdcTemp>                 : std::integral_constant<ushort, 0x10A8> {};


enum class Vx1730WriteRegisters : unsigned short
{ Dummy32, ChannelDynamicRange, ChannelPulseWidth, ChannelTriggerThreshold,
  CoupleSelfTriggerLogic, ChannelDcOffset, BoardConfig, BufferOrg, CustomSize,
  ChannelAdcCalibration, AcqControl, SoftwareTrigger, GlobalTriggerMask,
  FrontPanelTrgOutEnableMask, PostTrigger, LvdsIoData, FrontPanelIoCtrl,
  ChannelEnableMask, SetMonitorDac, SoftwareClockSync, MonitorDacMode,
  FanSpeedCtrl, MemoryBufferAlmostFullLevel, RunStartStopDelay,
  FrontPanelLvdsIoNewFeatures, ChannelShutdown, ReadoutCtrl, BoardId,
  McstBaseAddrAndCtrl, RelocationAddress, InterruptStatusId,
  InterruptEventNumber, MaxEventsPerBlt, Scratch, SoftwareReset,
  SoftwareClear, ConfigurationReset };

template<Vx1730WriteRegisters> struct Vx1730CommonWriteRegistersAddr;
template<> struct Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::BoardConfig>                 : std::integral_constant<ushort, 0x8000> {};
template<> struct Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::BufferOrg>                   : std::integral_constant<ushort, 0x800C> {};
template<> struct Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::CustomSize>                  : std::integral_constant<ushort, 0x8020> {};
template<> struct Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::ChannelAdcCalibration>       : std::integral_constant<ushort, 0x809C> {};
template<> struct Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::AcqControl>                  : std::integral_constant<ushort, 0x8100> {};
template<> struct Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::SoftwareTrigger>             : std::integral_constant<ushort, 0x8108> {};
template<> struct Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::GlobalTriggerMask>           : std::integral_constant<ushort, 0x810C> {};
template<> struct Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::FrontPanelTrgOutEnableMask>  : std::integral_constant<ushort, 0x8110> {};
template<> struct Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::PostTrigger>                 : std::integral_constant<ushort, 0x8114> {};
template<> struct Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::LvdsIoData>                  : std::integral_constant<ushort, 0x8118> {};
template<> struct Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::FrontPanelIoCtrl>            : std::integral_constant<ushort, 0x811C> {};
template<> struct Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::ChannelEnableMask>           : std::integral_constant<ushort, 0x8120> {};
template<> struct Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::SetMonitorDac>               : std::integral_constant<ushort, 0x8138> {};
template<> struct Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::SoftwareClockSync>           : std::integral_constant<ushort, 0x813C> {};
template<> struct Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::MonitorDacMode>              : std::integral_constant<ushort, 0x8144> {};
template<> struct Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::FanSpeedCtrl>                : std::integral_constant<ushort, 0x8168> {};
template<> struct Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::MemoryBufferAlmostFullLevel> : std::integral_constant<ushort, 0x816C> {};
template<> struct Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::RunStartStopDelay>           : std::integral_constant<ushort, 0x8170> {};
template<> struct Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::FrontPanelLvdsIoNewFeatures> : std::integral_constant<ushort, 0x81A0> {};
template<> struct Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::ChannelShutdown>             : std::integral_constant<ushort, 0x81C0> {};
template<> struct Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::ReadoutCtrl>                 : std::integral_constant<ushort, 0xEF00> {};
template<> struct Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::BoardId>                     : std::integral_constant<ushort, 0xEF08> {};
template<> struct Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::McstBaseAddrAndCtrl>         : std::integral_constant<ushort, 0xEF0C> {};
template<> struct Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::RelocationAddress>           : std::integral_constant<ushort, 0xEF10> {};
template<> struct Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::InterruptStatusId>           : std::integral_constant<ushort, 0xEF14> {};
template<> struct Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::InterruptEventNumber>        : std::integral_constant<ushort, 0xEF18> {};
template<> struct Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::MaxEventsPerBlt>             : std::integral_constant<ushort, 0xEF1C> {};
template<> struct Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::Scratch>                     : std::integral_constant<ushort, 0xEF20> {};
template<> struct Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::SoftwareReset>               : std::integral_constant<ushort, 0xEF24> {};
template<> struct Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::SoftwareClear>               : std::integral_constant<ushort, 0xEF28> {};
template<> struct Vx1730CommonWriteRegistersAddr<Vx1730WriteRegisters::ConfigurationReset>          : std::integral_constant<ushort, 0xEF34> {};


template<Vx1730WriteRegisters> struct Vx1730IndivWriteRegistersAddr;
//the offsets are always 0x0100
template<Vx1730WriteRegisters> struct Vx1730IndivWriteRegistersOffs { static constexpr ushort value = 0x0100;};
//now define addresses
template<> struct Vx1730IndivWriteRegistersAddr<Vx1730WriteRegisters::Dummy32>                      : std::integral_constant<ushort, 0x1024> {};
template<> struct Vx1730IndivWriteRegistersAddr<Vx1730WriteRegisters::ChannelDynamicRange>          : std::integral_constant<ushort, 0x1028> {};
template<> struct Vx1730IndivWriteRegistersAddr<Vx1730WriteRegisters::ChannelPulseWidth>            : std::integral_constant<ushort, 0x1070> {};
template<> struct Vx1730IndivWriteRegistersAddr<Vx1730WriteRegisters::ChannelTriggerThreshold>      : std::integral_constant<ushort, 0x1080> {};
template<> struct Vx1730IndivWriteRegistersAddr<Vx1730WriteRegisters::ChannelDcOffset>              : std::integral_constant<ushort, 0x1098> {};


template<Vx1730WriteRegisters> struct Vx1730IbcastWriteRegistersAddr;
template<> struct Vx1730IbcastWriteRegistersAddr<Vx1730WriteRegisters::Dummy32>                      : std::integral_constant<ushort, 0x8024> {};
template<> struct Vx1730IbcastWriteRegistersAddr<Vx1730WriteRegisters::ChannelDynamicRange>          : std::integral_constant<ushort, 0x8028> {};
template<> struct Vx1730IbcastWriteRegistersAddr<Vx1730WriteRegisters::ChannelPulseWidth>            : std::integral_constant<ushort, 0x8070> {};
template<> struct Vx1730IbcastWriteRegistersAddr<Vx1730WriteRegisters::ChannelTriggerThreshold>      : std::integral_constant<ushort, 0x8080> {};
template<> struct Vx1730IbcastWriteRegistersAddr<Vx1730WriteRegisters::ChannelDcOffset>              : std::integral_constant<ushort, 0x8098> {};


template<Vx1730WriteRegisters> struct Vx1730GroupWriteRegistersAddr;
//the offsets are almost always 0x0200 for the group write registers
template<Vx1730WriteRegisters> struct Vx1730GroupWriteRegistersOffs { static constexpr ushort value = 0x0200;};
//now define addresses
template<> struct Vx1730GroupWriteRegistersAddr<Vx1730WriteRegisters::CoupleSelfTriggerLogic>       : std::integral_constant<ushort, 0x1084> {};


template<Vx1730WriteRegisters> struct Vx1730GbcastWriteRegistersAddr;
template<> struct Vx1730GbcastWriteRegistersAddr<Vx1730WriteRegisters::CoupleSelfTriggerLogic>       : std::integral_constant<ushort, 0x8084> {};

}

}

#endif //ORCHID_SRC_HARDWARE_DIGITIZER_DIGITIZERREGISTERS_H
