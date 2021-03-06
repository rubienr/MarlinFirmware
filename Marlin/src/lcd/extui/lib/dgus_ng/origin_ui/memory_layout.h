/**
 * Marlin 3D Printer Firmware
 * Copyright (c) 2020 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (c) 2011 Camiel Gubbels / Erik van der Zalm
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#pragma once

#include "../../../../../inc/MarlinConfig.h"

/**
 * Display memory layout used for T5UID. Except of the system variables the layout is arbitrary.
 *
 * DGUD displays exchage data by address. Addresses are distinguised in between variable pointer (VP) and stack pointer
 * (SP). One address points to a word which is two bytes:
 * - low address byte: most significant byte
 * - hight address byte: least significant byte
 *
 * Variable pointer (VP)
 * Variable pointer are defined in the DGUS project and must match the defninition in this file. Data is exchanged once
 * the display device detects variable changes (for example the screen was touched) and sends it via serial connection.
 * When Marlin sends data via serial connection the device updates by the memory and renders the display
 * instantaneoulsy.
 *
 * Stack pointer (SP)
 * SP may optionally be assigned to VP in the DGUS project. SPs can be utilized to access and modify variable's
 * attributes like color, size, placement and much more depending on the variable type: Icon, text, slider, page sliding
 * etc. Stack pointers have no address restriction, however 0x5000 and above is used in this layout.
 *
 * Memory Layout
 * 0x0000 .. 0x0FFF - System variables: Read, write and R/W variabled such as
 *                    HW version, music player setting, sytem restart, format flash etc.
 * 0x1000 .. 0x1FFF - variables to never change location, regardless of UI version
 * 0x2000 .. 0x2FFF - controls: variable pointer that will trigger Marlin actions
 * 0x3000 .. 0x4FFF - Read Marlin data to be displayed but not changed by the display (read only)
 *           0x4210   Reserved for icons' default value (0).
 * 0x5000 ..        - stack pointers to modify display elements, e.g. change color or
 *                    other attributes
 *
 * Note: Whenever an address denotes more than one word, add a size hint beginning with same name and
 * trailing "Bytes" appended.
 */

//=============================================================================
// 0x0000 - 0x0FFF: system variables  =========================================
//=============================================================================

//=============================================================================
// 0x1000 - 0x1FFF: variables to never change location ========================
//=============================================================================

namespace dgus {
namespace memory_layout {

enum class UiVersion : uint16_t {
  // UI Version always on 0x1000 ... 0x1002 so that the firmware can check this and bail out.
  MajorMinor = 0x1000,
  Patch = 0x1001,
  // lets reserve 16 bytes here to determine if UI is suitable for this Marlin. tbd.
  Flavour = 0x1010,
  FlavourBytes = 16,

};

// Storage space for the Killscreen messages. 0x1100 - 0x1200 . Reused for the popup.
enum class UiMessages : uint16_t {
  Message1 = 0x1100,
  Message2 = 0x1140,
  Message3 = 0x1180,
  Message4 = 0x11C0,
  Message1Bytes = 32,
  Message2Bytes = 32,
  Message3Bytes = 32,
  Message4Bytes = 32,
};

//=============================================================================
// 0x2000 - 0x2FFF: variable pointer that will trigger Marlin actions =========
//=============================================================================

enum class ScreenAction : uint16_t {
  // Screenchange request for screens that only make sense when printer is idle.
  // e.g movement is only allowed if printer is not printing.
  ScreenChangeAsk = 0x2000, // Marlin must confirm by setting the screen manually.
  // Key-Return button to new menu pressed. Data contains target screen in low byte and info in high byte.
  ScreenChange = 0x2001,
  AllHeaderOff = 0x2002,     // turn all heaters off
  ScreenchangeIfSd = 0x2003, // "Print" Button touched - go there only if there is an SD Card.
  UserConfirmed = 0x2010,    // OK on confirm screen.
};

#if ENABLED(SDSUPPORT)
// Buttons on the SD-Card File listing.
enum class SdAction : uint16_t {
  // Data: 0 for "up a directory", numbers are the amount to scroll, e.g -1 one up, 1 one down
  ScrollEvent = 0x2020,
  FileSelected = 0x2022, // Number of file field selected.
  // This is a virtual VP and emulated by the Confirm Screen when a file has been confirmed.
  FileSelectConfirm = 0x2024,
  ResumePauseAbort = 0x2026,    // Resume(Data=0), Pause(Data=1), Abort(Data=2) SD Card prints
  AbortPrintConfirmed = 0x2028, // Abort print confirmation (virtual, will be injected by the confirm dialog)
  Print_Setting = 0x2040,
  Print_LiveAdjustZ = 0x2050, // Data: 0 down, 1 up
};
#endif

// display memory addresses
enum class PowerSupplyUnit : uint16_t {
  Control = 0x2004,
};

#if ENABLED(CASE_LIGHT_ENABLE)
enum class CaseLight : uint16_t {
  Control = 0x2005,
};
#endif

#if HAS_COLOR_LEDS
enum class ColorLeds : uint16_t {
  Control0 = 0x2006,
  Control1 = 0x2007,
  Control2 = 0x2008,
};
#endif

// TODO rubienr - replace enum Move by this:
// 4 Byte Fixed point number; format xxx.yy

/*enum class Incremental : uint16_t {
  X = 0x2100,
  Y = 0x2102,
  Z = 0x2104,
};*/

enum class MoveAxis : uint16_t {
  Distance = 0x2100, // 0x3400,
  // DistanceBytes = 4,
  // Axis = 0x2102,
  Command = 0x2101,
};

enum class Ubl : uint16_t {
  RequestFlags = 0x2121,
  FadeHeightSlotNnumber = 0x2122,
  OnOoffUnused = 0x2123,
};

enum class DriverControl : uint16_t {
  LockUnlockControl = 0x2130,
};

enum class OffsetNozzleToProbe : uint16_t {
  Nozzle0Control = 0x2124,
  Nozzle0X = 0x2125,
  Nozzle0Y = 0x2126,
  Nozzle0Z = 0x2127,
};

// display memory addresses
enum class Filament : uint16_t {
  LoadUnloadControl = 0x2300,
  ExtrudeRetractControl = 0x2301,
};

enum class MoveE : uint16_t {
// Distance = 0x2104,
// DistanceBytes = 4,
// Index = 0x2106,
// Command = 0x2107,
#if EXTRUDERS >= 1
  E0 = 0x2110,
#endif
#if EXTRUDERS >= 2
  E1 = 0x2112,
#endif
#if EXTRUDERS >= 3
  E2 = 0x2114,
#endif
#if EXTRUDERS >= 4
  E3 = 0x2116,
#endif
#if EXTRUDERS >= 5
  E4 = 0x2118,
#endif
#if EXTRUDERS >= 6
  E5 = 0x211A,
#endif
};

enum class Homing : uint16_t {
  Control = 0x2120,
};

// Power loss recovery
enum class Recovery : uint16_t {
  PowerLoss = 0x2180,
};

// Fan Control Buttons , switch between "off" and "on"
enum class FanControl : uint16_t {
#if HAS_FAN0
  Fan0 = 0x2200,
#endif
#if HAS_FAN1
  Fan1 = 0x2201,
#endif
#if HAS_FAN2
  Fan2 = 0x2202,
#endif
#if HAS_FAN3
  Fan3 = 0x2203,
#endif
#if HAS_FAN4
  Fan4 = 0x2204,
#endif
#if HAS_FAN5
  Fan5 = 0x2205,
#endif
};

enum class TemperatureControl : uint16_t {
  Control = 0x2210,
};

/*
// Heater Control Buttons, triggered between "cool down" and "heat PLA" state
enum class HeaterControl : uint16_t {
#if EXTRUDERS >= 1
  E0 = 0x2210,
#endif
#if EXTRUDERS >= 2
  E1 = 0x2212,
#endif
#if EXTRUDERS >= 3
  E2 = 0x2214,
#endif
#if EXTRUDERS >= 4
  E3 = 0x2216,
#endif
#if EXTRUDERS >= 5
  E4 = 0x2218,
#endif
#if EXTRUDERS >= 6
  E5 = 0x221A,
#endif
  Bed = 0x221C,
};*/

// Preheat
enum class Preheat : uint16_t {
#if HAS_HEATER_BED
  Bed = 0x2220,
#endif
#if EXTRUDERS >= 1
  E0 = 0x2221,
// constexpr uint16_t VP_E0_BED_PREHEAT = 0x2220; constexpr uint16_t VP_E1_BED_CONTROL = 0x2222;
#endif
#if EXTRUDERS >= 2
  E1 = 0x2222,
#endif
#if EXTRUDERS >= 3
  E2 = 0x2223,
#endif
#if EXTRUDERS >= 4
  E3 = 0x2224,
#endif
#if EXTRUDERS >= 5
  E4 = 0x2225,
#endif
#if EXTRUDERS >= 6
  E5 = 0x2226,
#endif
};

// EEPROM store, reset
enum class Eeprom : uint16_t {
  Control = 0x2400,
};

enum class PidAutotune : uint16_t {
// PID auto tune
#if HAS_HEATER_0
  E0 = 0x2410,
#endif
#if HAS_HEATER_1
  E1 = 0x2412,
#endif
#if HAS_HEATER_2
  E2 = 0x2414,
#endif
#if HAS_HEATER_3
  E3 = 0x2416,
#endif
#if HAS_HEATER_4
  E4 = 0x2418,
#endif
#if HAS_HEATER_5
  E5 = 0x241A,
#endif
#if HAS_HEATER_BED
  Bed = 0x2420,
#endif
};

//=============================================================================
// 0x3000 - 0x4FFF: Marlin data to be displayed  ==============================
//=============================================================================

// TODO rubienr - reorder version details addresses
enum class MarlinVersion : uint16_t {
  Version = 0x3000,
  VersionBytes = 16,
  DetailedVersion = 0x3510,
  DetailedVersionBytes = 32,
  CompileDate = 0x3520,
  CompileDateBYtes = 16,
  DistributionDate = 0x3530,
  DistributionDateBytes = 16,
  ConfigAuthor = 0x3540,
  ConfigAuthorBytes = 38,
};

enum class Temperature : uint16_t {
  Status = 0x3060,

  ExtruderNIs = 0x3061,
  ExtruderNSet = 0x3062,

#if ENABLED(HAS_HEATED_BED)
  BedIs = 0x3063,
  BedSet = 0x3064,
#endif

#if ENABLED(HAS_HEATED_CHAMBER)
  ChamberIs = 0x3065,
  ChamberSet = 0x3066,
#endif

  /*
  #if HAS_HEATER_1
    E1Is = 0x3064,
    E1Set = 0x3066,
  #endif
  #if HAS_HEATER_2
    E2Is = 0x3068,
    E2Set = 0x306A,
  #endif
  #if HAS_HEATER_3
    E3Is = 0x306C,
    E3Set = 0x306E,
  #endif
  #if HAS_HEATER_4
    E4Is = 0x3070,
    E4Set = 0x3072,
  #endif
  #if HAS_HEATER_5
    E5Is = 0x3074,
    E5Set = 0x3076,
  #endif*/

};

enum class LcdMessage : uint16_t {
  M117 = 0x3020,
  M117Bytes = 0x20,
};

enum class FlowRate : uint16_t {
  Control = 0x3090,
  Status = 0x3091,
  ExtruderN = 0x3092, // shared among all N extruders
  /*#if EXTRUDERS >= 1
    E0 = 0x3090,
  #endif
  #if EXTRUDERS >= 2
    E1 = 0x3092,
  #endif
  #if EXTRUDERS >= 3
    E2 = 0x3094,
  #endif
  #if EXTRUDERS >= 4
    E3 = 0x3096,
  #endif
  #if EXTRUDERS >= 5
    E4 = 0x3098,
  #endif
  #if EXTRUDERS >= 6
    E5 = 0x309A,
  #endif
    EBytes = 2, // 2 byte integer
    */
};

enum class FanSpeed : uint16_t {
  //  FanPercentageBytes = 2,
  Control = 0x3100,
  Status = 0x3101,
  FanN = 0x3102, // shared among all N fans

  /*#if HAS_FAN0
    Fan0Percentage = 0x3100, // 2 Byte Integer (0..100)
  #endif
  #if HAS_FAN1
    Fan1Percentage = 0x33A2,
  #endif
  #if HAS_FAN2
    Fan2Percentage = 0x33A4,
  #endif
  #if HAS_FAN3
    Fan3Percentage = 0x33A6,
  #endif
  #if HAS_FAN4
    Fan4Percentage = 0x33A8,
  #endif
  #if HAS_FAN5
    Fan5Percentage = 0x33AA,
  #endif*/
};

enum class FeedRate : uint16_t {
  Percentage = 0x3105, // 2 Byte Integer (0..100)
};

enum class PrintStats : uint16_t {
  PrintProgressPercentage = 0x3104, // 2 Byte Integer (0..100)
  PrintTime = 0x3106,
  PrintTimeBytes = 16,
  PrintAccTime = 0x3160,
  PrintAccTimeBytes = 32,
  PrintsTotal = 0x3180,
  PrintsTotalBytes = 16,
};

// 4 Byte Fixed point number; format xxx.yy
enum class PositionAxis : uint16_t {
  X = 0x3110,
  Y = 0x3112,
  Z = 0x3114,
  E = 0x3116,
  Bytes = 4, // size shared among all axis
};

// 4 Byte Fixed point number; format xxx.yy
enum class PositionE : uint16_t {
#if EXTRUDERS >= 1
  E0 = 0x3120,
#endif
#if EXTRUDERS >= 2
  E1 = 0x3124,
#endif
#if EXTRUDERS >= 3
  E2 = 0x3126,
#endif
#if EXTRUDERS >= 4
  E3 = 0x3128,
#endif
#if EXTRUDERS >= 5
  E4 = 0x312A,
#endif
#if EXTRUDERS >= 6
  E5 = 0x312C,
#endif
  Bytes = 4,
};

#if ENABLED(SDSUPPORT)
// SDCard File Listing
enum class SdFileListing : uint16_t {
  // TODO rubienr: implementation assumes correct alignment of FileNameN in
  //  DGUSScreenVariableHandler::DGUSLCD_SD_SendFilename
  FileName0 = 0x3200,
  FileName1 = 0x3220,
  FileName2 = 0x3240,
  FileName3 = 0x3260,
  FileName4 = 0x3280,
  FileNameBytes = 32, // Length is shared among all entries.
  FilesPerScreen = 5, // FIXME move that info to the display and read it from there.
  Print_Filename = 0x32C,
};
#endif

enum class FanStatus : uint16_t {
// Fan status
#if HAS_FAN0
  Fan0 = 0x3300,
#endif
#if HAS_FAN1
  Fan1 = 0x3302,
#endif
#if HAS_FAN2
  Fan2 = 0x3304,
#endif
#if HAS_FAN3
  Fan3 = 0x3306,
#endif
#if HAS_FAN4
  Fan4 = 0x3308,
#endif
#if HAS_FAN5
  Fan5 = 0x330A,
#endif
};

// Heater status
enum class HeaterStatus : uint16_t {
#if EXTRUDERS >= 1
  E0 = 0x3310,
#endif
#if EXTRUDERS >= 2
  E1 = 0x3312,
#endif
#if EXTRUDERS >= 3
  E2 = 0x3314,
#endif
#if EXTRUDERS >= 4
  E3 = 0x3316,
#endif
#if EXTRUDERS >= 5
  E4 = 0x3318,
#endif
#if EXTRUDERS >= 6
  E5 = 0x331A,
#endif
  Bed = 0x331C,
};

// Steps per mm
enum class StepsPerMm : uint16_t {
  X = 0x3600, // at the moment , 2 byte unsigned int , 0~1638.4
#if ENABLED(X_DUAL_STEPPER_DRIVERS)
  X2 = 0x3602,
#endif
  Y = 0x3604,
#if ENABLED(Y_DUAL_STEPPER_DRIVERS)
  Y2 = 0x3606,
#endif
  Z = 0x3608,
#if NUM_Z_STEPPER_DRIVERS >= 2
  Z2 = 0x360A,
#endif
#if EXTRUDERS >= 1
  E0 = 0x3610,
#endif
#if EXTRUDERS >= 2
  E1 = 0x3612,
#endif
#if EXTRUDERS >= 3
  E2 = 0x3614,
#endif
#if EXTRUDERS >= 4
  E3 = 0x3616,
#endif
#if EXTRUDERS >= 5
  E4 = 0x3618,
#endif
#if HAS_EXTRUDERS >= 6
  E5 = 0x361A,
#endif
};

// PIDs
enum class Pid : uint16_t {
// 2 byte unsigned int , 0~1638.4 for heater/bed
#if HAS_HEATER_0
  E0P = 0x3700,
  E0I = 0x3702,
  E0D = 0x3704,
#endif
#if HAS_HEATER_1
  E1P = 0x3706,
  E1I = 0x3708,
  E1D = 0x370A,
#endif
#if HAS_HEATER_2
  E2P = 0x370C,
  E2I = 0x370E,
  E2D = 0x3710,
#endif
#if HAS_HEATER_3
  E3P = 0x3712,
  E3I = 0x3714,
  E3D = 0x3716,
#endif
#if HAS_HEATER_4
  E4P = 0x3718,
  E4I = 0x371A,
  E4D = 0x371C,
#endif
#if HAS_HEATER_5
  E5P = 0x371E,
  E5I = 0x3720,
  E5D = 0x3722,
#endif

#if HAS_HEATER_BED
  BedP = 0x3710,
  BedI = 0x3712,
  BedD = 0x3714,
#endif
};

// Wating screen status
enum class ScreenStatus : uint16_t {
  Waiting = 0x3800,
};

//=============================================================================
// 0x5000 - 0x....: stack pointers  ===========================================
//=============================================================================

/*
#if HAS_HEATER_0
constexpr uint16_t SP_T_E0_Is = 0x5000;
constexpr uint16_t SP_T_E0_Set = 0x5010;
#endif
#if HAS_HEATER_1
constexpr uint16_t SP_T_E1_Is = 0x5020;
constexpr uint16_t SP_T_E1_Set = 0x5050;
#endif
#if HAS_HEATER_BED
constexpr uint16_t SP_T_Bed_Is = 0x5030;
constexpr uint16_t SP_T_Bed_Set = 0x5040;
#endif
#if HAS_HEATER_2
constexpr uint16_t SP_T_E2_Is = 0x5060;
constexpr uint16_t SP_T_E2_Set = 0x5070;
#endif
#if HAS_HEATER_3
constexpr uint16_t SP_T_E3_Is = 0x5080;
constexpr uint16_t SP_T_E3_Set = 0x5090;
#endif
#if HAS_HEATER_4
constexpr uint16_t SP_T_E4_Is = 0x50A0;
constexpr uint16_t SP_T_E4_Set = 0x50C0;
#endif
#if HAS_HEATER_5
constexpr uint16_t SP_T_E5_Is = 0x50E0;
constexpr uint16_t SP_T_E5_Set = 0x5100;
#endif
*/

template <typename enum_t> uint16_t constexpr to_address(const enum_t e) { return static_cast<uint16_t>(e); }

namespace {
  template <typename from_enum_t, typename to_cast_type_t> constexpr uint8_t to_XintNN_t(const from_enum_t e) {
    return static_cast<to_cast_type_t>(e);
  }
}

template <typename enum_t> constexpr uint8_t to_uint8_t(const enum_t e) { return to_XintNN_t<enum_t, uint8_t>(e); }
template <typename enum_t> constexpr uint16_t to_uint16_t(const enum_t e) { return to_XintNN_t<enum_t, uint16_t>(e); }

} // namespace memory_layout
} // namespace dgus

using dgus::memory_layout::to_address;
using dgus::memory_layout::to_uint8_t;
using dgus::memory_layout::to_uint16_t;
