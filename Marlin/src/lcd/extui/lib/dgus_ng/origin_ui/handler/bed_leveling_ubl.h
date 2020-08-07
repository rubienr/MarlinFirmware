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

namespace dgus {
struct DGUS_VP_Variable;
}

namespace dgus_origin {
namespace bed_leveling_ubl {

struct CachedState {
  union RequestFlags {
    uint16_t data;
    struct {
      // falgs set by display
      uint8_t start_ubl : 1;
      uint8_t enable_ubl : 1;
      uint8_t disable_ubl : 1;
      uint8_t load_mesh : 1;
      uint8_t save_mesh : 1;
      uint8_t _unused : 3;
      // flags set internally
      uint8_t slot_changed : 1;
      uint8_t fade_height_changed : 1;
      uint8_t _unused2 : 6;
    } __attribute__((packed));
  } request_flags;

  union FadeHeightSlotNumber {
    uint16_t data;
    struct {
      // falgs set by display
      uint8_t fade_height;
      uint8_t slot_number;
    } __attribute__((packed));
  } fade_height_slot_number;

  union OnOff {
    static constexpr uint8_t UKNOWN = 0;
    static constexpr uint8_t OFF = 1;
    static constexpr uint8_t ON = 2;
    uint16_t data;
    struct {
      // falgs set by display
      uint8_t on_off_unknown : 2;
      uint8_t _unused : 6;
      uint8_t _unused2;
    } __attribute__((packed));
  } on_off_state;
};

/**
 * Caches the given UBL argument and  triggers "on varible changed" handling.
 *
 * @param var
 * @param val_ptr
 */
void handle_parameter_load_save_probe(dgus::DGUS_VP_Variable &var, void *val_ptr);
/**
 * Caches the given UBL z height-fade and slot number and triggers "on varible changed" handling.
 *
 * @param var
 * @param val_ptr
 */
void handle_parameter_fade_slot(dgus::DGUS_VP_Variable &var, void *val_ptr);

} // namespace bed_leveling_ubl
} // namespace dgus_origin

#endif