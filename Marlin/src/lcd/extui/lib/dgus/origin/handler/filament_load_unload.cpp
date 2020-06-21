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
#include "../../DGUSDisplay.h"

namespace dgus_origin {
namespace filament {

void handle_filament_load_unload(DGUS_VP_Variable &var, void *val_ptr) {
  DEBUG_ECHOLNPGM("handle_filament_load_unload");

  static_assert(EXTRUDERS > 0, "Minimal one extruder neded.");
  if (val_ptr == nullptr)
    return;
  if (var.memadr == nullptr)
    return;

  // TODO raoul
  union Command {
    uint16_t data;
    struct {
      uint8_t unload_flag : 1;
      uint8_t load_flag : 1;
      uint8_t _pad : 6;
      uint8_t extruder_id;
    } __attribute__((packed));
  };

  Command display_request = {.data = Dgus::swap16(*static_cast<uint16_t *>(val_ptr))};
  Command *filament_command = static_cast<Command *>(var.memadr);
  *filament_command = display_request;
  filament_command->extruder_id = static_cast<uint8_t>(constrain(filament_command->extruder_id, 0, EXTRUDERS - 1));

  char buf[16] = {0};
  if (filament_command->unload_flag) {
    filament_command->unload_flag = 0;
    sprintf(buf, "M702 T%d U%d", filament_command->extruder_id, FILAMENT_CHANGE_UNLOAD_LENGTH);
  } else if (filament_command->load_flag) {
    filament_command->load_flag = 0;
    sprintf(buf, "M701 T%d L%d", filament_command->extruder_id, FILAMENT_CHANGE_FAST_LOAD_LENGTH);
  } else
    return;

  queue.enqueue_one_now(buf);
}

} // namespace filament
} // namespace dgus_origin

#endif
