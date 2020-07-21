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

#include "eeprom.h"

#if ENABLED(DGUS_ORIGIN_FILAMENT_EEPROM)

#include "../../../../../../gcode/queue.h"
#include "../../DGUSDisplay.h"

#if ENABLED(PRINTCOUNTER)
#include "../../module/printcounter.h"
#endif

namespace dgus_origin {
namespace eeprom {

void handle_eeprom_store_restore(DGUS_VP_Variable &var, void *val_ptr) {
#if ENABLED(DEBUG_DGUSLCD)
  DEBUG_ECHOLNPGM("handle_eeprom_store_restore");
#endif
  auto *cached{static_cast<CachedState *>(var.memadr)};
  auto *request{static_cast<CachedState *>(val_ptr)};
  request->data = dgus::swap16(request->data);

  if (request->restore_to_factory_settings) {
#if ENABLED(PRINTCOUNTER)
    print_job_timer.initStats();
#endif
    GCodeQueue::enqueue_now_P(PSTR("M502\nM500"));
    request->loaded_saved_restored_unknown = CachedState::RESTORED;
    request->restore_to_factory_settings = 0;
  }
  if (request->load_from_eeprom) {
    GCodeQueue::enqueue_now_P(PSTR("M501"));
    request->loaded_saved_restored_unknown = CachedState::LOADED;
    request->load_from_eeprom = 0;
  }
  if (request->save_to_eeprom) {
    GCodeQueue::enqueue_now_P(PSTR("M500"));
    request->loaded_saved_restored_unknown = CachedState::SAVED;
    request->save_to_eeprom = 0;
  }

  cached->data = request->data;
}

} // namespace eeprom
} // namespace dgus_origin

#endif
