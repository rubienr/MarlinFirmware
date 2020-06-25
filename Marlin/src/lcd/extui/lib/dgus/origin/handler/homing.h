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

#define DGUS_ORIGIN_HOMING

#if ENABLED(DGUS_ORIGIN_HOMING)

#include "../../../../ui_api.h"
#include "../../DGUSDisplay.h"

struct DGUS_VP_Variable;

namespace dgus_origin {
namespace homing {

union CachedState {
  uint16_t data;
  struct {
    uint8_t x : 1;
    uint8_t y : 1;
    uint8_t z : 1;
    uint8_t a : 1;
    uint8_t b : 1;
    uint8_t c : 1;
    uint8_t raise_before_home : 1;
    uint8_t _unused : 1;
    uint8_t cannot_move : 1;
    uint8_t _unused2 : 7;
  } __attribute__((packed));
};

/**
 * Interprets the command flags and clears each handled one for back propagation to display.
 * @param var var.memaddr must not be nullptr
 * @param val_ptr
 */
void handle_homing_command(DGUS_VP_Variable &var, void *val_ptr);

} // namespace homing
} // namespace dgus_origin

#endif