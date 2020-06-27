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
#include "../../../../../../module/motion.h"

namespace dgus_origin {
namespace filament {

extern CachedState cached_state;

namespace {

void handle_filament_load_unload() {
  cached_state.load_unload.extruder_id = constrain(cached_state.load_unload.extruder_id, 0, EXTRUDERS - 1);

  cached_state.load_unload.ok_error_clear = CachedState::LoadUnload::CLEAR;

  if (cached_state.load_unload.command != CachedState::LoadUnload::LOAD &&
      cached_state.load_unload.command != CachedState::LoadUnload::UNLOAD) {
    return;
  }

  if (Temperature::targetTooColdToExtrude(cached_state.load_unload.extruder_id)) {
    cached_state.load_unload.ok_error_clear = CachedState::LoadUnload::ERROR;
    cached_state.load_unload.command = CachedState::LoadUnload::NO_ACTION;
    return;
  }

  char load_unload{' '};
  char command{' '};

  if (cached_state.load_unload.command == CachedState::LoadUnload::UNLOAD) {
    command = '2';
    load_unload = 'U';
  }

  if (cached_state.load_unload.command == CachedState::LoadUnload::LOAD) {
    command = '1';
    load_unload = 'L';
  }

  if (command != ' ' && load_unload != ' ') {
    char buf[24];
    sprintf_P(buf,
              PSTR("M70%c T%d %c%d"),
              command,
              cached_state.load_unload.extruder_id,
              load_unload,
              FILAMENT_CHANGE_UNLOAD_LENGTH);
    GCodeQueue::enqueue_one_now(buf);
    cached_state.load_unload.ok_error_clear = CachedState::LoadUnload::OK;
  }

  cached_state.load_unload.command = CachedState::LoadUnload::NO_ACTION;
}

void handle_filament_extrude_retract() {
  cached_state.load_unload.ok_error_clear = CachedState::LoadUnload::CLEAR;

  if (cached_state.load_unload.command != CachedState::LoadUnload::EXTRUDE &&
      cached_state.load_unload.command != CachedState::LoadUnload::RETRACT) {
    return;
  }

  if (Temperature::targetTooColdToExtrude(cached_state.load_unload.extruder_id)) {
    cached_state.load_unload.ok_error_clear = CachedState::LoadUnload::ERROR;
    cached_state.load_unload.command = CachedState::LoadUnload::NO_ACTION;
    return;
  }

  const bool backup_is_relative_mode{relative_mode};
  if (!backup_is_relative_mode)
    GCodeQueue::enqueue_one_P(PSTR("G91"));

  char sign{(cached_state.load_unload.command == CachedState::LoadUnload::EXTRUDE) ? '+' : '-'};
  uint16_t int_position_e = cached_state.extrude_retract.distance / 100;
  uint16_t int_position_e_fraction = cached_state.extrude_retract.distance % 100;

  char buf[24];
  sprintf_P(buf, PSTR("G1 E%c%d.%02d"), sign, int_position_e, int_position_e_fraction);
  GCodeQueue::enqueue_one_now(buf);

  if (!backup_is_relative_mode)
    GCodeQueue::enqueue_one_P(PSTR("G90"));

  cached_state.load_unload.command = CachedState::LoadUnload::NO_ACTION;
}

} // namespace

void handle_filament_move(DGUS_VP_Variable &var, void *val_ptr) {
#if ENABLED(DEBUG_DGUSLCD)
  DEBUG_ECHOLNPGM("handle_filament_move");
#endif
  static_assert(EXTRUDERS > 0, "Minimal one extruder neded.");
  *static_cast<uint16_t *>(var.memadr) = dgus::swap16(*static_cast<uint16_t *>(val_ptr));

  handle_filament_load_unload();
  handle_filament_extrude_retract();
}

} // namespace filament
} // namespace dgus_origin

#endif
