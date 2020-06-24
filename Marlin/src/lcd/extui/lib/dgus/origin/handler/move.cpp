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

#include "move.h"

#if ENABLED(DGUS_ORIGIN_MOVE)

#include "../../../../../../gcode/queue.h"
#include "../../../../../../core/types.h"

#include "../../DGUSDisplay.h"

namespace dgus_origin {
namespace move {

namespace {
void moveAxis(bool relative, AxisEnum axis, int16_t &distance) {
  SERIAL_ECHOLNPAIR("move axis", axis);
  SERIAL_ECHOLNPAIR("move dist ", distance);
  SERIAL_ECHOLNPAIR("move rel ", relative);
}
} // namespace

void handle_move_relatvie_x(DGUS_VP_Variable &var, void *val_ptr) {
#if ENABLED(DEBUG_DGUSLCD)
  DEBUG_ECHOLNPGM("handle_move_relatvie_x");
#endif
  Data distance{.as_uint_16 = dgus::swap16(*static_cast<uint16_t *>(val_ptr))};
  moveAxis(true, AxisEnum::X_AXIS, distance.as_int_16);
}

void handle_move_relatvie_y(DGUS_VP_Variable &var, void *val_ptr) {
#if ENABLED(DEBUG_DGUSLCD)
  DEBUG_ECHOLNPGM("handle_move_relatvie_y");
#endif
  Data distance{.as_uint_16 = dgus::swap16(*static_cast<uint16_t *>(val_ptr))};
  moveAxis(true, AxisEnum::Y_AXIS, distance.as_int_16);
}

void handle_move_relatvie_z(DGUS_VP_Variable &var, void *val_ptr) {
#if ENABLED(DEBUG_DGUSLCD)
  DEBUG_ECHOLNPGM("handle_move_relatvie_z");
#endif
  Data distance{.as_uint_16 = dgus::swap16(*static_cast<uint16_t *>(val_ptr))};
  moveAxis(true, AxisEnum::Z_AXIS, distance.as_int_16);
}

void handle_move_absolute_x(DGUS_VP_Variable &var, void *val_ptr) {
#if ENABLED(DEBUG_DGUSLCD)
  DEBUG_ECHOLNPGM("handle_move_absolute_x");
#endif
  Data distance{.as_uint_16 = dgus::swap16(*static_cast<uint16_t *>(val_ptr))};
  moveAxis(false, AxisEnum::X_AXIS, distance.as_int_16);
}

void handle_move_absolute_y(DGUS_VP_Variable &var, void *val_ptr) {
#if ENABLED(DEBUG_DGUSLCD)
  DEBUG_ECHOLNPGM("handle_move_absolute_y");
#endif
  Data distance{.as_uint_16 = dgus::swap16(*static_cast<uint16_t *>(val_ptr))};
  moveAxis(false, AxisEnum::Y_AXIS, distance.as_int_16);
}

void handle_move_absolute_z(DGUS_VP_Variable &var, void *val_ptr) {
#if ENABLED(DEBUG_DGUSLCD)
  DEBUG_ECHOLNPGM("handle_move_absolute_z");
#endif
  Data distance{.as_uint_16 = dgus::swap16(*static_cast<uint16_t *>(val_ptr))};
  moveAxis(false, AxisEnum::Z_AXIS, distance.as_int_16);
}

} // namespace move
} // namespace dgus_origin

#endif
