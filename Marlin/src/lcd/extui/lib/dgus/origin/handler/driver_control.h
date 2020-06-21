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

#define DGUS_ORIGIN_DRIVER_CONTROL

#include "../../../../../../core/macros.h"

#if ENABLED(DGUS_ORIGIN_DRIVER_CONTROL)

#include "../../../../ui_api.h"
#include "../../DGUSDisplay.h"

struct DGUS_VP_Variable;

namespace dgus_origin {
namespace driver_control {

union CachedState {

  static constexpr uint8_t UNSET = 0;
  static constexpr uint8_t ENABLE = 1;
  static constexpr uint8_t DISABLE = 2;
  struct {
    // flags set by display
    uint8_t enable_disable : 2;
    uint8_t _unused : 6;
    // flags set internally
    uint8_t enabled_disabled : 2;
    uint8_t _unused2 : 6;
  } __attribute__((packed));
  uint16_t data;
};

/**
 * Enable or disable motors' driver.
 * @param var
 * @param val_ptr low byte: 0 - undefined, 1 - disable motors, 2 - enable motors; high byte: unused
 */
void handle_motor_lock_unlock(DGUS_VP_Variable &var, void *val_ptr);

} // namespace driver_control
} // namespace dgus_origin

#endif