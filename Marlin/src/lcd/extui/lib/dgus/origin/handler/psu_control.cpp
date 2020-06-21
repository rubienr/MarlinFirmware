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

#include "psu_control.h"

#if ENABLED(DGUS_ORIGIN_PSU_CONTROL)

#include "../../../../../../gcode/queue.h"
#include "../../DGUSDisplay.h"

namespace dgus_origin {
namespace psu_control {

void handle_psu_on_off(DGUS_VP_Variable &var, void *val_ptr) {
  DEBUG_ECHOLNPGM("handle_psu_on_off");

  CachedState *state = static_cast<CachedState *>(var.memadr);
  state->data = dgus::swap16(*static_cast<uint16_t *>(val_ptr));

  switch (state->enable_disable) {
    case CachedState::UNSET:
    default:
      return;
    case CachedState::DISABLE:
      queue.enqueue_now_P(PSTR("M81"));
      state->enabled_disabled = CachedState::DISABLE;

      break;
    case CachedState::ENABLE:
      queue.enqueue_now_P(PSTR("M80"));
      state->enabled_disabled = CachedState::ENABLE;
      break;
  }

  state->enable_disable = CachedState::UNSET;
}

} // namespace psu_control
} // namespace dgus_origin

#endif
