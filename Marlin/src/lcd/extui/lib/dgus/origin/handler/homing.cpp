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

#include "../../../../../../module/motion.h"
#include "../../../../../../gcode/queue.h"
#include "../../../../../../core/types.h"

#include "../../DGUSDisplay.h"

namespace dgus_origin {
namespace homing {

extern CachedState cached_state;

namespace {

void update_homing_status_flags() {
  cached_state.is_homed_x = (axes_need_homing(static_cast<uint8_t>(_BV(AxisEnum::X_AXIS))) != 0) ? 0 : 1;
  cached_state.is_homed_y = (axes_need_homing(static_cast<uint8_t>(_BV(AxisEnum::Y_AXIS))) != 0) ? 0 : 1;
  cached_state.is_homed_z = (axes_need_homing(static_cast<uint8_t>(_BV(AxisEnum::Z_AXIS))) != 0) ? 0 : 1;
  cached_state.is_homed_all = (axes_need_homing() != 0) ? 0 : 1;
}

} // namespace

void handle_homing_command(DGUS_VP_Variable &var, void *val_ptr) {
#if ENABLED(DEBUG_DGUSLCD)
  DEBUG_ECHOLNPGM("handle_homing_command");
#endif
  cached_state.data = dgus::swap16(*static_cast<uint16_t *>(val_ptr));
  char r{' '}, x{' '}, y{' '}, z{' '};

  if (cached_state.do_raise_before_home) {
    r = 'R';
    cached_state.do_raise_before_home = 0;
  }
  if (cached_state.do_home_x) {
    x = 'X';
    cached_state.is_homed_x = 0;
    cached_state.is_homed_all = 0;
    cached_state.do_home_x = 0;
  }
  if (cached_state.do_home_y) {
    y = 'Y';
    cached_state.is_homed_y = 0;
    cached_state.is_homed_all = 0;
    cached_state.do_home_y = 0;
  }
  if (cached_state.do_home_z) {
    z = 'Z';
    cached_state.is_homed_z = 0;
    cached_state.is_homed_all = 0;
    cached_state.do_home_z = 0;
  }
  handle_send_homing_status(var);

  char buf[16]; // "G28 R X Y Z"
  snprintf_P(buf, sizeof(buf), PSTR("G28 %c %c %c %c"), r, x, y, z);
  GCodeQueue::enqueue_one_now(buf);

  update_homing_status_flags();
}

void handle_send_homing_status(DGUS_VP_Variable &var) {
#if ENABLED(DEBUG_DGUSLCD)
  DEBUG_ECHOLNPGM("handle_send_homing_status");
#endif
  update_homing_status_flags();

  dgus::DgusWord_t status{.data = cached_state.data};
  status.data = dgus::swap16(status.data);
  DGUSDisplay::WriteVariable(var.VP, status.data);
}

} // namespace homing
} // namespace dgus_origin

#endif
