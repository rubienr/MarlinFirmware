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

#include "screen/eeprom.h"
#include "screen/filament_load_unload.h"
#include "screen/nozzle_offset.h"
#include "screen/bed_leveling_ubl.h"
#include "screen/lights.h"
#include "screen/psu_control.h"
#include "screen/driver_control.h"
#include "screen/info.h"
#include "screen/boot.h"
#include "screen/tools.h"
#include "screen/temperatures.h"
#include "screen/flowrates.h"
#include "screen/feedrates.h"
#include "screen/sd_files.h"
#include "screen/sd_print_manipulation.h"
#include "screen/status.h"
#include "screen/manual_move.h"
#include "screen/manual_extrude.h"
#include "screen/main.h"
#include "memory_layout.h"

enum DGUSLCD_Screens : uint8_t {
  // 000-009 reserved for the default font
  // 010-012 unused
  // 013     reserved for 13TouchFile.bin
  // 014     reserved for 14ShowFile.bin
  // 015     button click audio effect
  // 016-021 unused
  // 022     reserved for 22_Config.bin
  DGUSLCD_SCREEN_BOOT = 24,
  DGUSLCD_SCREEN_MAIN = 26,
#if ENABLED(DGUS_ORIGIN_TEMPERATURES)
  DGUSLCD_SCREEN_TEMPERATURE = 28,
#endif
#if ENABLED(DGUS_ORIGIN_STATUS)
  DGUSLCD_SCREEN_STATUS = 30,
  DGUSLCD_SCREEN_STATUS2 = 32,
#endif
#if ENABLED(DGUS_ORIGIN_MANUAL_MOVE)
  DGUSLCD_SCREEN_MANUALMOVE = 40,
#endif
#if ENABLED(DGUS_ORIGIN_MANUAL_EXTRUDE)
  DGUSLCD_SCREEN_MANUALEXTRUDE = 42,
#endif
#if ENABLED(DGUS_ORIGIN_FEEDRATES)
  DGUSLCD_SCREEN_FANANDFEEDRATE = 44,
#endif
#if ENABLED(DGUS_ORIGIN_FLOWRATES)
  DGUSLCD_SCREEN_FLOWRATES_1 = 46,
  DGUSLCD_SCREEN_FLOWRATES_2 = 48,
#endif
#if ENABLED(DGUS_ORIGIN_SDFILES)
  DGUSLCD_SCREEN_SDFILELIST = 50,
#endif
#if ENABLED(DGUS_ORIGIN_SDPRINT_MANIPULATION)
  DGUSLCD_SCREEN_SDPRINTMANIPULATION = 52,
#endif
#if ENABLED(DGUS_ORIGIN_INFO)
  DGUSLCD_SCREEN_INFO = 54, ///< info screen shows Versions of CPU, UI, Marlin, Marlin configuration, etc...
#endif
#if ENABLED(DGUS_ORIGIN_TOOLS)
  DGUSLCD_SCREEN_TOOLS = 60, ///< screen for showing tools sub menu entries
#endif
#if ENABLED(DGUS_ORIGIN_AUTO_BED_LEVELING_UBL)
  DGUSLCD_SCREEN_BED_LEVELING = 62, ///< screen for showing/manipulating bed leveling
#endif
#if ENABLED(DGUS_ORIGIN_PSU_CONTROL)
  DGUSLCD_SCREEN_PSU = 64, ///< screen for showing/manipulating power supply status
#endif
#if ENABLED(DGUS_ORIGIN_DRIVER_CONTROL)
  DGUSLCD_SCREEN_MOTORS = 66, ///< screen for showing/manipulating motors status
#endif
#if ENABLED(DGUS_ORIGIN_NOZZLE_OFFSET)
  DGUSLCD_SCREEN_PROBE_OFFSET = 68, ///< screen for showing/manipulating nozzle to probe offset
#endif
#if ENABLED(DGUS_ORIGIN_LIGHTS)
  DGUSLCD_SCREEN_LIGHTS = 70, ///< screen for showing/manipulating case and RGB light
#endif
#if ENABLED(EEPROM_SETTINGS)
  DGUSLCD_SCREEN_EEPROM = 72, ///< screen for manipulating EEPROM: load, save, restore to factory
#endif
#if ENABLED(DGUS_ORIGIN_FILAMENT_LOAD_UNLOAD)
  DGUSLCD_SCREEN_FILAMENT_LOAD_UNLOAD = 74,
#endif
  // 090-105 reserved for .ico icon sets
  DGUSLCD_SCREEN_POWER_LOSS = 106,
  DGUSLCD_SCREEN_UTILITY = 110,
  DGUSLCD_SCREEN_PREHEAT = 120,
  DGUSLCD_SCREEN_FILAMENT_HEATING = 146,
  DGUSLCD_SCREEN_FILAMENT_LOADING = 148,
  DGUSLCD_SCREEN_FILAMENT_UNLOADING = 158,
  DGUSLCD_SCREEN_SDPRINTTUNE = 170,
  DGUSLCD_SCREEN_CONFIRM = 240,
  DGUSLCD_SCREEN_KILL = 250, ///< Kill Screen. Must always be 250 (to be able to display "Error wrong LCD Version")
  DGUSLCD_SCREEN_WAITING = 251,
  /// special target, popup screen will also return this code to say "return to previous screen"
  DGUSLCD_SCREEN_POPUP = 252,
  DGUSLDC_SCREEN_UNUSED = 255
};
