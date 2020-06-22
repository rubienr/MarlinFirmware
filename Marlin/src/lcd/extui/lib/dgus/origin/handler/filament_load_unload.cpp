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

#include "filament_load_unload.h"

#if ENABLED(DGUS_ORIGIN_FILAMENT_LOAD_UNLOAD)

#include "../../../../../../gcode/queue.h"
#include "../../../../../../module/temperature.h"
#include "../../DGUSDisplay.h"

namespace dgus_origin {
namespace filament {

void handle_filament_load_unload(DGUS_VP_Variable &var, void *val_ptr) {
#if ENABLED(DEBUG_DGUSLCD)
  DEBUG_ECHOLNPGM("handle_filament_load_unload");
#endif
  static_assert(EXTRUDERS > 0, "Minimal one extruder neded.");

  auto *cached{static_cast<CachedState *>(var.memadr)};
  auto *request{static_cast<CachedState *>(val_ptr)};
  request->data = dgus::swap16(request->data);
  char buf[16];

  request->extruder_id = constrain(request->extruder_id, 0, EXTRUDERS - 1);

  if ((request->unload || request->load) && Temperature::targetTooColdToExtrude(request->extruder_id)) {
    request->ok_error_clear = CachedState::ERROR;
    request->unload = 0;
    request->load = 0;
  } else {
    request->ok_error_clear = CachedState::CLEAR;
  }

  if (request->unload) {
    sprintf(buf, "M702 T%d U%d", request->extruder_id, FILAMENT_CHANGE_UNLOAD_LENGTH);
    GCodeQueue::enqueue_one_now(buf);
    request->ok_error_clear = CachedState::OK;
    request->unload = 0;
  }

  if (request->load) {
    sprintf(buf, "M701 T%d L%d", request->extruder_id, FILAMENT_CHANGE_FAST_LOAD_LENGTH);
    GCodeQueue::enqueue_one_now(buf);
    request->ok_error_clear = CachedState::OK;
    request->load = 0;
  }

  cached->data = request->data;
}

} // namespace filament
} // namespace dgus_origin

#endif
