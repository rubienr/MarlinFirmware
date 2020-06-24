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

#define DGUS_ORIGIN_MOVE

#if ENABLED(DGUS_ORIGIN_MOVE)

#include "../../../../ui_api.h"
#include "../../DGUSDisplay.h"

struct DGUS_VP_Variable;

namespace dgus_origin {
namespace move {

union Data {
  uint16_t data;
  struct {
    uint8_t low_byte;
    uint8_t high_byte;
  } __attribute__((packed));
  uint16_t as_uint_16;
  int16_t as_int_16;
  float as_float;
};

/**
 * Moves axis by the given distance.
 * @param var var.memaddr shall be nullptr; value is unused
 * @param val_ptr the signed distance in 1/10th mm to move
 */
void handle_move_relatvie_x(DGUS_VP_Variable &var, void *val_ptr);
void handle_move_relatvie_y(DGUS_VP_Variable &var, void *val_ptr);
void handle_move_relatvie_z(DGUS_VP_Variable &var, void *val_ptr);

/**
 * Moves axis to the given position.
 * @param var var.memaddr shall be nullptr; value is unused
 * @param val_ptr the signed position in 1/10th mm to move to
 */
void handle_move_absolute_x(DGUS_VP_Variable &var, void *val_ptr);
void handle_move_absolute_y(DGUS_VP_Variable &var, void *val_ptr);
void handle_move_absolute_z(DGUS_VP_Variable &var, void *val_ptr);

} // namespace move
} // namespace dgus_origin

#endif