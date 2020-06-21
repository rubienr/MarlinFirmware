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

namespace dgus_origin {
namespace eeprom {

void handle_settings(DGUS_VP_Variable &var, void *val_ptr) {
  DEBUG_ECHOLNPGM("handle_settings");
  const uint16_t value = dgus::swap16(*(uint16_t *)val_ptr);

  // TODO raoul
  constexpr uint16_t restore_to_factory_settings_flag = 0x1;
  constexpr uint16_t load_from_eeprom_flag = 0x2;
  constexpr uint16_t save_to_eeprom_flag = 0x4;

  switch (value) {
    default:
      break;
    case restore_to_factory_settings_flag:
#if ENABLED(PRINTCOUNTER)
      print_job_timer.initStats();
#endif
      queue.enqueue_now_P(PSTR("M502\nM500"));
      break;
    case load_from_eeprom_flag:
      queue.enqueue_now_P(PSTR("M501"));
      break;
    case save_to_eeprom_flag:
      queue.enqueue_now_P(PSTR("M500"));
      break;
  }
}

} // namespace eeprom
} // namespace dgus_origin

#endif
