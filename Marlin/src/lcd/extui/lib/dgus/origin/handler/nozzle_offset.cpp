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

#include "nozzle_offset.h"

#if ENABLED(DGUS_ORIGIN_NOZZLE_OFFSET)

namespace dgus_origin {
namespace nozzle_offset {

void handle_probe_offset(DGUS_VP_Variable &var, void *val_ptr) {
  DEBUG_ECHOLNPGM("handle_probe_offset");

  CachedState *cached = static_cast<CachedState *>(var.memadr);
  CachedState *request = static_cast<CachedState *>(val_ptr);
  cached->data = request->data;

  // todo rubienr - how to get values from MarlinSettings
  constexpr float default_offset[] = NOZZLE_TO_PROBE_OFFSET;

  if (cached->reset_x_flag) {
    ExtUI::setProbeOffset_mm(default_offset[X_AXIS], ExtUI::X);
    cached->reset_x_flag = 0;
  }

  if (cached->reset_y_flag) {
    ExtUI::setProbeOffset_mm(default_offset[Y_AXIS], ExtUI::Y);
    cached->reset_y_flag = 0;
  }

  if (cached->reset_z_flag) {
    ExtUI::setZOffset_mm(default_offset[Z_AXIS]);
    cached->reset_z_flag = 0;
  }
}

void handle_probe_offset_z_axis(DGUS_VP_Variable &var, void *val_ptr) {
  DEBUG_ECHOLNPGM("handle_probe_offset_z_axis");
  UNUSED(var);
  union {
    float as_float;
    uint16_t as_uint;
    int16_t as_int;
  } z_offset_mm{.as_uint = Dgus::swap16(*static_cast<uint16_t *>(val_ptr))};

  z_offset_mm.as_float = static_cast<float>(z_offset_mm.as_int) / 100U;
  ExtUI::setZOffset_mm(z_offset_mm.as_float);
}

} // namespace nozzle_offset
} // namespace dgus_origin

#endif
