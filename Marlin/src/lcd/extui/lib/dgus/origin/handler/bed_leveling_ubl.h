/**
 * Marlin 3D Printer Firmware
 * Copyright (c) 2020 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
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
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 */
#pragma once

#include "../../../../../../inc/MarlinConfig.h"

#if ENABLED(AUTO_BED_LEVELING_UBL)
#define DGUS_ORIGIN_AUTO_BED_LEVELING_UBL
#endif

#if ENABLED(DGUS_ORIGIN_AUTO_BED_LEVELING_UBL)

#include "../../../../ui_api.h"
#include "../../DGUSDisplay.h"

struct DGUS_VP_Variable;

namespace dgus_origin {
namespace bed_leveling_ubl {

struct CachedState {
  /// flags defining UBL operations to process
  union RequestFlags {
    struct {
      // falgs set by display
      uint8_t start_ubl : 1;
      uint8_t enable_ubl : 1;
      uint8_t disable_ubl : 1;
      uint8_t load_mesh : 1;
      uint8_t save_mesh : 1;
      // flags set internally
      // TODO rubienr - allign unsed
      uint8_t _unused : 1;
      uint8_t _unused2 : 1;
      uint8_t slot_changed : 1;
      uint8_t fade_height_changed : 1;
    } __attribute__((packed));
    uint16_t data;

  } request_flags{.data = 0};

  /// fade (low byte) and slot number (hight byte) arguments set by display or internally
  union FadeHeightSlotNumber {
    struct {
      // falgs set by display
      uint8_t fade_height;
      uint8_t slot_number;
    } __attribute__((packed));
    uint16_t data;
  } fade_height_slot_number{.data = 0};

  union OnOff {
    static constexpr uint8_t UNSET = 0;
    static constexpr uint8_t OFF = 1;
    static constexpr uint8_t ON = 2;
    struct {
      // falgs set by display
      uint8_t on_off : 2;
      uint8_t _unused : 6;
      uint8_t _unused2;
    } __attribute__((packed));
    uint16_t data;
  } on_off{0}; /// enable/disable (low byte) state set internally
};

/**
 * Buffers the given UBL argument and handles possible UBL requests.
 * \sa #HandleBedLevelingUblParameter_Fade_Slot(DGUS_VP_Variable &, void *)
 * @param var DGUS variable of one of OriginVariables.bed_leveling*
 * OriginVariables.except of bed_leveling__fade_height__slot_number
 * @param val_ptr one of VP_BED_LEVELING_PARAMETER*
 */
void handle_parameter_save(DGUS_VP_Variable &var, void *val_ptr);
/**
 * Buffers the given UBL z height-fade and slot number and triggers "on varible
 * changed" handling.
 * @param var DGUS variable of
 * OriginVariables.bed_leveling__fade_height__slot_number
 * @param val_ptr VP_BED_LEVELING_PARAMETER__ON_OFF__UNUSED
 */
void handle_parameter_fade_slot(DGUS_VP_Variable &var, void *val_ptr);

} // namespace bed_leveling_ubl
} // namespace dgus_origin

#endif