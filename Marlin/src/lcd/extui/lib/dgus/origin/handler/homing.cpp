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

#include "homing.h"

#if ENABLED(DGUS_ORIGIN_HOMING)

#include "../../../../../../gcode/queue.h"
#include "../../../../../../core/types.h"

#include "../../DGUSDisplay.h"

namespace dgus_origin {
namespace homing {

extern CachedState cached_state;

void handle_homing_command(DGUS_VP_Variable &var, void *val_ptr) {
#if ENABLED(DEBUG_DGUSLCD)
  DEBUG_ECHOLNPGM("handle_homing_command");
#endif
  cached_state.data = dgus::swap16(*static_cast<uint16_t *>(val_ptr));
  char r{' '}, x{' '}, y{' '}, z{' '};

  if (cached_state.raise_before_home) {
    r = 'R';
    cached_state.raise_before_home = 0;
  }
  if (cached_state.x) {
    x = 'X';
    cached_state.x = 0;
  }
  if (cached_state.y) {
    y = 'Y';
    cached_state.y = 0;
  }
  if (cached_state.z) {
    z = 'Z';
    cached_state.z = 0;
  }

  char buf[16]; // "G28 R X Y Z"
  snprintf_P(buf, sizeof(buf), PSTR("G28 %c %c %c %c"), r, x, y, z);
  GCodeQueue::enqueue_one_now(buf);
}

} // namespace homing
} // namespace dgus_origin

#endif
