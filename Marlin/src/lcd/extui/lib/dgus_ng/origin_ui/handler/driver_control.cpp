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

#include "driver_control.h"

#if ENABLED(DGUS_ORIGIN_DRIVER_CONTROL)

#include "../../../../../../gcode/queue.h"
#include "../../DGUSDisplay.h"

using namespace dgus;

namespace dgus_origin {
namespace driver_control {

void handle_motor_lock_unlock(DGUS_VP_Variable &var, void *val_ptr) {
#if ENABLED(DEBUG_DGUSLCD)
  DEBUG_ECHOLNPGM("handle_motor_lock_unlock");
#endif
  auto *cached{static_cast<CachedState *>(var.memadr)};
  auto *request{static_cast<CachedState *>(val_ptr)};
  request->data = dgus::swap16(request->data);

  if (request->enable) {
    GCodeQueue::enqueue_now_P(PSTR("M17"));
    cached->on_off_unknown = CachedState::ON;
  }

  if (request->disable) {
    GCodeQueue::enqueue_now_P(PSTR("M18"));
    cached->on_off_unknown = CachedState::OFF;
  }
}

} // namespace driver_control
} // namespace dgus_origin

#endif
