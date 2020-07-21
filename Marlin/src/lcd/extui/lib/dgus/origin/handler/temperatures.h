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

#define DGUS_ORIGIN_TEMPERATURES

#if ENABLED(DGUS_ORIGIN_TEMPERATURES)

#include "../../../../ui_api.h"
#include "../../DGUSDisplay.h"

struct DGUS_VP_Variable;

namespace dgus_origin {
namespace temperatures {

struct CachedState {
  union Control {
    uint16_t data;
    struct {
      uint8_t disable_hotend : 1;
      uint8_t disable_bed : 1;
      uint8_t disable_chamber : 1;
      uint8_t disable_all : 1;
      uint8_t do_preset_preheat_1 : 1;
      uint8_t do_preset_preheat_2 : 1;
      uint8_t _unused : 2;
      uint8_t hotend_id : 8;
    } __attribute__((packed));
  } control;

  union Status {
    uint16_t data;
    struct {
      uint8_t is_hotend_heating : 1;
      uint8_t is_hotend_cooling : 1;
      uint8_t is_bed_heating : 1;
      uint8_t is_bed_cooling : 1;
      uint8_t is_chamber_heating : 1;
      uint8_t is_chamber_cooling : 1;
      uint8_t _unused : 2;
      uint8_t _unused2 : 8;
    } __attribute__((packed));
  } status;

  struct Temperatures {
    union HotendNTarget {
      uint16_t data;
      int16_t celsius;
    } hotend_target_temperature;
  } temperatures;
};

/**
 * Updates the is_heating|cooling flags of selected hotend, bed, and chamber.
 * @param var var.memadr must not be nullptr
 * @param val_ptr must not be nullptr
 */
void handle_heating_status(DGUS_VP_Variable &var, void *val_ptr);

/**
 * Handles temperature changes request.
 * - Sets the target temperature of the selected hotend.
 * - Updates the current temperature of the selected hotend.
 * @param var var.memadr must not be nullptr
 * @param val_ptr must not be nullptr
 */
void handle_set_hotend_temperature(DGUS_VP_Variable &var, void *val_ptr);

/**
 * Handles temperature changes request.
 * @param var unused
 * @param val_ptr must not be nullptr
 */
void handle_set_bed_temperature(DGUS_VP_Variable &var, void *val_ptr);

#if ENABLED(HAS_HEATED_CHAMBER)
/**
 * Handles temperature changes request.
 * @param var unused
 * @param val_ptr must not be nullptr
 */
void handle_set_chamber_temperature(DGUS_VP_Variable &var, void *val_ptr);
#endif

/**
 * Handles flags defined in CachedState::Control.
 * @param var var.memadr must not be nullptr
 * @param val_ptr must not be nullptr
 */
void handle_temperature_control_command(DGUS_VP_Variable &var, void *val_ptr);

/**
 * Sends the selected hotend's temperature to display.
 * @param var unused
 */
void handle_send_hotend_temperature_to_display(DGUS_VP_Variable &var);

} // namespace temperatures
} // namespace dgus_origin
#endif