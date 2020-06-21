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
 * 0x3000 .. 0x4FFF - Marlin data to be displayed
 * 0x5000 ..        - stack pointers to modify display elements, e.g. change color or
 *                    other attributes
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
  Message1Bytes = 0x20,
  Message2Bytes = 0x20,
  Message3Bytes = 0x20,
  Message4Bytes = 0x20,
};

} // namespace memory_layout
} // namespace dgus

//=============================================================================
// 0x2000 - 0x2FFF: variable pointer that will trigger Marlin actions =========
//=============================================================================

// Screenchange request for screens that only make sense when printer is idle.
// e.g movement is only allowed if printer is not printing.
// Marlin must confirm by setting the screen manually.
constexpr uint16_t VP_SCREENCHANGE_ASK = 0x2000;
constexpr uint16_t VP_SCREENCHANGE =
    0x2001; // Key-Return button to new menu pressed. Data contains target screen in low byte and info in high byte.
constexpr uint16_t VP_TEMP_ALL_OFF = 0x2002;        // Turn all heaters off. Value arbitrary ;)=
constexpr uint16_t VP_SCREENCHANGE_WHENSD = 0x2003; // "Print" Button touched -- go only there if there is an SD Card.

constexpr uint16_t VP_CONFIRMED = 0x2010; // OK on confirm screen.

// Buttons on the SD-Card File listing.
#if ENABLED(SDSUPPORT)
constexpr uint16_t VP_SD_ScrollEvent =
    0x2020; // Data: 0 for "up a directory", numbers are the amount to scroll, e.g -1 one up, 1 one down
constexpr uint16_t VP_SD_FileSelected = 0x2022; // Number of file field selected.
constexpr uint16_t VP_SD_FileSelectConfirm =
    0x2024; // (This is a virtual VP and emulated by the Confirm Screen when a file has been confirmed)

constexpr uint16_t VP_SD_ResumePauseAbort = 0x2026; // Resume(Data=0), Pause(Data=1), Abort(Data=2) SD Card prints
constexpr uint16_t VP_SD_AbortPrintConfirmed =
    0x2028; // Abort print confirmation (virtual, will be injected by the confirm dialog)
constexpr uint16_t VP_SD_Print_Setting = 0x2040;
constexpr uint16_t VP_SD_Print_LiveAdjustZ = 0x2050; // Data: 0 down, 1 up
#endif

namespace dgus {
namespace memory_layout {

// display memory addresses
enum class PowerSupplyUnit : uint16_t {
  Control = 0x2004,
};

enum class CaseLight : uint16_t {
  // Case light:
  // High byte represents the intensity
  // Low byte is the display request: 0x0 value unset, 0x1 light off, 0x2 light on
  Control = 0x2005,
};

enum class ColorLeds : uint16_t {
  // Colour LEDs:
  // High byte represents the intensity
  // Low byte is the display request: 0x0 value unset, 0x1 light off, 0x2 light on
  Control0 = 0x2006,
  // High byte red component, low byte green component
  Control1 = 0x2007,
  // High byte blue component, low byte white component
  Control2 = 0x2008,
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
};

// display memory addresses
enum class Addresses : uint16_t {
#if ENABLED(CASE_LIGHT_ENABLE)
  // Case light:
  // High byte represents the intensity
  // Low byte is the display request: 0x0 value unset, 0x1 light off, 0x2 light on
  CaseLightControl = 0x2005,
#endif
#if ENABLED(DGUS_ORIGIN_LIGHTS)
  // Colour LEDs:
  // High byte represents the intensity
  // Low byte is the display request: 0x0 value unset, 0x1 light off, 0x2 light on
  ColorControl0 = 0x2006,
  // High byte red component, low byte green component
  ColorControl1 = 0x2007,
  // High byte blue component, low byte white component
  ColorControl2 = 0x2008,
#endif
};
} // namespace memory_layout
} // namespace dgus

// Controls for movement (we can't use the incremental / decremental feature of the display at this feature works only
// with 16 bit values (which would limit us to 655.35mm, which is likely not a problem for common setups, but i don't
// want to rule out hangprinters support) A word about the coding: The VP will be per axis and the return code will be
// an signed 16 bit value in 0.01 mm resolution, telling us the relative travel amount t he user wants to do. So eg. if
// the display sends us VP=2100 with value 100, the user wants us to move X by +1 mm.
constexpr uint16_t VP_MOVE_X = 0x2100;
constexpr uint16_t VP_MOVE_Y = 0x2102;
constexpr uint16_t VP_MOVE_Z = 0x2104;

namespace dgus {
namespace memory_layout {

enum class MoveE : uint16_t {
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

} // namespace memory_layout
} // namespace dgus

// Power loss recovery
constexpr uint16_t VP_POWER_LOSS_RECOVERY = 0x2180;

// Fan Control Buttons , switch between "off" and "on"
constexpr uint16_t VP_FAN0_CONTROL = 0x2200;
constexpr uint16_t VP_FAN1_CONTROL = 0x2202;
// constexpr uint16_t VP_FAN2_CONTROL = 0x2204;
// constexpr uint16_t VP_FAN3_CONTROL = 0x2206;

// Heater Control Buttons, triggered between "cool down" and "heat PLA" state
#if EXTRUDERS >= 1
constexpr uint16_t VP_E0_CONTROL = 0x2210;
#endif
#if EXTRUDERS >= 2
constexpr uint16_t VP_E1_CONTROL = 0x2212;
#endif
#if EXTRUDERS >= 3
constexpr uint16_t VP_E2_CONTROL = 0x2214;
#endif
#if EXTRUDERS >= 4
constexpr uint16_t VP_E3_CONTROL = 0x2216;
#endif
#if EXTRUDERS >= 5
constexpr uint16_t VP_E4_CONTROL = 0x2218;
#endif
#if EXTRUDERS >= 6
constexpr uint16_t VP_E5_CONTROL = 0x221A;
#endif

constexpr uint16_t VP_BED_CONTROL = 0x221C;

// Preheat
// TODO rubienr: "preheat" E0 vs. "control" E1 - seems to be mixed up somehow (compare with hiprecyDGUSdisplayDef.h).
// TODO rubienr: decorate commented code with define-guards to be fully dependent on Configuration*.h
constexpr uint16_t VP_E0_BED_PREHEAT = 0x2220;
constexpr uint16_t VP_E1_BED_CONTROL = 0x2222;
// constexpr uint16_t VP_E2_BED_CONTROL = 0x2224;
// constexpr uint16_t VP_E3_BED_CONTROL = 0x2226;
// constexpr uint16_t VP_E4_BED_CONTROL = 0x2228;
// constexpr uint16_t VP_E5_BED_CONTROL = 0x222A;

namespace dgus {
namespace memory_layout {

// EEPROM store, reset
enum class Eeprom : uint16_t {
  Control = 0x2400,
};

} // namespace memory_layout
} // namespace dgus

// PID auto tune
#if HAS_HEATER_0
constexpr uint16_t VP_PID_AUTOTUNE_E0 = 0x2410;
#endif
#if HAS_HEATER_1
constexpr uint16_t VP_PID_AUTOTUNE_E1 = 0x2412;
#endif
#if HAS_HEATER_2
constexpr uint16_t VP_PID_AUTOTUNE_E2 = 0x2414;
#endif
#if HAS_HEATER_3
constexpr uint16_t VP_PID_AUTOTUNE_E3 = 0x2416;
#endif
#if HAS_HEATER_4
constexpr uint16_t VP_PID_AUTOTUNE_E4 = 0x2418;
#endif
#if HAS_HEATER_5
constexpr uint16_t VP_PID_AUTOTUNE_E5 = 0x241A;
#endif
#if HAS_HEATER_BED
constexpr uint16_t VP_PID_AUTOTUNE_BED = 0x2420;
#endif

//=============================================================================
// 0x3000 - 0x4FFF: Marlin data to be displayed  ==============================
//=============================================================================

// TODO rubienr - reorder version details addresses
namespace dgus {
namespace memory_layout {
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
}
} // namespace dgus

// Place for status messages.
constexpr uint16_t VP_M117 = 0x3020;
constexpr uint8_t VP_M117_LEN = 0x20;

// Temperatures.
#if HAS_HEATER_0
constexpr uint16_t VP_T_E0_Is = 0x3060;  // 4 Byte Integer
constexpr uint16_t VP_T_E0_Set = 0x3062; // 2 Byte Integer
#endif
#if HAS_HEATER_1
constexpr uint16_t VP_T_E1_Is = 0x3064;  // 4 Byte Integer
constexpr uint16_t VP_T_E1_Set = 0x3066; // 2 Byte Integer
#endif
#if HAS_HEATER_2
constexpr uint16_t VP_T_E2_Is = 0x3068;  // 4 Byte Integer
constexpr uint16_t VP_T_E2_Set = 0x306A; // 2 Byte Integer
#endif
#if HAS_HEATER_3
constexpr uint16_t VP_T_E3_Is = 0x306C;  // 4 Byte Integer
constexpr uint16_t VP_T_E3_Set = 0x306E; // 2 Byte Integer
#endif
#if HAS_HEATER_4
constexpr uint16_t VP_T_E4_Is = 0x3070;  // 4 Byte Integer
constexpr uint16_t VP_T_E4_Set = 0x3072; // 2 Byte Integer
#endif
#if HAS_HEATER_5
constexpr uint16_t VP_T_E5_Is = 0x3074;  // 4 Byte Integer
constexpr uint16_t VP_T_E5_Set = 0x3076; // 2 Byte Integer
#endif

#if HAS_HEATER_BED
constexpr uint16_t VP_T_Bed_Is = 0x3080;  // 4 Byte Integer
constexpr uint16_t VP_T_Bed_Set = 0x3082; // 2 Byte Integer
#endif

#if EXTRUDERS >= 1
constexpr uint16_t VP_Flowrate_E0 = 0x3090; // 2 Byte Integer
#endif
#if EXTRUDERS >= 2
constexpr uint16_t VP_Flowrate_E1 = 0x3092; // 2 Byte Integer
#endif
#if EXTRUDERS >= 3
constexpr uint16_t VP_Flowrate_E2 = 0x3094; // 2 Byte Integer
#endif
#if EXTRUDERS >= 4
constexpr uint16_t VP_Flowrate_E3 = 0x3096; // 2 Byte Integer
#endif
#if EXTRUDERS >= 5
constexpr uint16_t VP_Flowrate_E4 = 0x3098; // 2 Byte Integer
#endif
#if EXTRUDERS >= 6
constexpr uint16_t VP_Flowrate_E5 = 0x309A; // 2 Byte Integer
#endif

#if HAS_FAN0
constexpr uint16_t VP_Fan0_Percentage = 0x3100; // 2 Byte Integer (0..100)
#endif
#if HAS_FAN1
constexpr uint16_t VP_Fan1_Percentage = 0x33A2; // 2 Byte Integer (0..100)
#endif
#if HAS_FAN2
constexpr uint16_t VP_Fan2_Percentage = 0x33A4; // 2 Byte Integer (0..100)
#endif
#if HAS_FAN3
constexpr uint16_t VP_Fan3_Percentage = 0x33A6; // 2 Byte Integer (0..100)
#endif
#if HAS_FAN4
constexpr uint16_t VP_Fan4_Percentage = 0x33A8; // 2 Byte Integer (0..100)
#endif
#if HAS_FAN5
constexpr uint16_t VP_Fan5_Percentage = 0x33AA; // 2 Byte Integer (0..100)
#endif

constexpr uint16_t VP_Feedrate_Percentage = 0x3102;      // 2 Byte Integer (0..100)
constexpr uint16_t VP_PrintProgress_Percentage = 0x3104; // 2 Byte Integer (0..100)

constexpr uint16_t VP_PrintTime = 0x3106;
constexpr uint16_t VP_PrintTime_LEN = 10;

constexpr uint16_t VP_PrintAccTime = 0x3160;
constexpr uint16_t VP_PrintAccTime_LEN = 32;

constexpr uint16_t VP_PrintsTotal = 0x3180;
constexpr uint16_t VP_PrintsTotal_LEN = 16;

namespace dgus {
namespace memory_layout {
// 4 Byte Fixed point number; format xxx.yy
enum class Position : uint16_t {
  X = 0x3110,
  Y = 0x3112,
  Z = 0x3114,
  Bytes = 4,
};

// 4 Byte Fixed point number; format xxx.yy
enum class PoseitionE : uint16_t {
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

} // namespace memory_layout
} // namespace dgus

// SDCard File Listing

#if ENABLED(SDSUPPORT)
constexpr uint16_t VP_SD_FileName_LEN = 32;    // LEN is shared for all entries.
constexpr uint16_t DGUS_SD_FILESPERSCREEN = 5; // FIXME move that info to the display and read it from there.
constexpr uint16_t VP_SD_FileName0 = 0x3200;
constexpr uint16_t VP_SD_FileName1 = 0x3220;
constexpr uint16_t VP_SD_FileName2 = 0x3240;
constexpr uint16_t VP_SD_FileName3 = 0x3260;
constexpr uint16_t VP_SD_FileName4 = 0x3280;

// TODO rubienr: cleanup
constexpr uint16_t VP_SD_Print_Filename = 0x32C0; //
#endif

// Fan status
#if HAS_FAN0
constexpr uint16_t VP_FAN0_STATUS = 0x3300;
#endif
#if HAS_FAN1
constexpr uint16_t VP_FAN1_STATUS = 0x3302;
#endif
#if HAS_FAN2
constexpr uint16_t VP_FAN2_STATUS = 0x3304;
#endif
#if HAS_FAN3
constexpr uint16_t VP_FAN3_STATUS = 0x3306;
#endif
#if HAS_FAN4
constexpr uint16_t VP_FAN3_STATUS = 0x3308;
#endif
#if HAS_FAN5
constexpr uint16_t VP_FAN3_STATUS = 0x330A;
#endif

// Heater status
#if EXTRUDERS >= 1
constexpr uint16_t VP_E0_STATUS = 0x3310;
#endif
#if EXTRUDERS >= 2
constexpr uint16_t VP_E1_STATUS = 0x3312;
#endif
#if EXTRUDERS >= 3
constexpr uint16_t VP_E2_STATUS = 0x3314;
#endif
#if EXTRUDERS >= 4
constexpr uint16_t VP_E3_STATUS = 0x3316;
#endif
#if EXTRUDERS >= 5
constexpr uint16_t VP_E4_STATUS = 0x3318;
#endif
#if EXTRUDERS >= 6
constexpr uint16_t VP_E5_STATUS = 0x331A;
#endif

constexpr uint16_t VP_BED_STATUS = 0x331C;

constexpr uint16_t VP_MOVE_OPTION = 0x3400;

// Steps per mm
constexpr uint16_t VP_X_STEP_PER_MM = 0x3600; // at the moment , 2 byte unsigned int , 0~1638.4
#if ENABLED(X_DUAL_STEPPER_DRIVERS)
constexpr uint16_t VP_X2_STEP_PER_MM = 0x3602;
#endif
constexpr uint16_t VP_Y_STEP_PER_MM = 0x3604;
#if ENABLED(Y_DUAL_STEPPER_DRIVERS)
constexpr uint16_t VP_Y2_STEP_PER_MM = 0x3606;
#endif
constexpr uint16_t VP_Z_STEP_PER_MM = 0x3608;
#if NUM_Z_STEPPER_DRIVERS >= 2
constexpr uint16_t VP_Z2_STEP_PER_MM = 0x360A;
#endif

#if EXTRUDERS >= 1
constexpr uint16_t VP_E0_STEP_PER_MM = 0x3610;
#endif
#if EXTRUDERS >= 2
constexpr uint16_t VP_E1_STEP_PER_MM = 0x3612;
#endif
#if EXTRUDERS >= 3
constexpr uint16_t VP_E2_STEP_PER_MM = 0x3614;
#endif
#if EXTRUDERS >= 4
constexpr uint16_t VP_E3_STEP_PER_MM = 0x3616;
#endif
#if EXTRUDERS >= 5
constexpr uint16_t VP_E4_STEP_PER_MM = 0x3618;
#endif
#if HAS_EXTRUDERS >= 6
constexpr uint16_t VP_E5_STEP_PER_MM = 0x361A;
#endif

// PIDs
#if HAS_HEATER_0
constexpr uint16_t VP_E0_PID_P = 0x3700; // at the moment , 2 byte unsigned int , 0~1638.4
constexpr uint16_t VP_E0_PID_I = 0x3702;
constexpr uint16_t VP_E0_PID_D = 0x3704;
#endif

#if HAS_HEATER_BED
constexpr uint16_t VP_BED_PID_P = 0x3710;
constexpr uint16_t VP_BED_PID_I = 0x3712;
constexpr uint16_t VP_BED_PID_D = 0x3714;
#endif

// Wating screen status
constexpr uint16_t VP_WAITING_STATUS = 0x3800;

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
template <typename enum_t> constexpr uint8_t to_uint8_t(const enum_t e) { return static_cast<uint8_t>(e); }