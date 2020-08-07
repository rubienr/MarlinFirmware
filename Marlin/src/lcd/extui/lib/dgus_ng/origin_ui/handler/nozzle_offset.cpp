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

using namespace dgus;

namespace dgus_origin {
namespace nozzle_offset {

void handle_probe_offset_request(DGUS_VP_Variable &var, void *val_ptr) {
#if ENABLED(DEBUG_DGUSLCD)
  DEBUG_ECHOLNPGM("handle_probe_offset_request");
#endif
  auto *request{static_cast<CachedState *>(var.memadr)};
  request->data = dgus::swap16(*static_cast<uint16_t *>(val_ptr));
  constexpr float default_offset[] = NOZZLE_TO_PROBE_OFFSET;

  if (request->reset_x || request->reset_all) {
    ExtUI::setProbeOffset_mm(default_offset[X_AXIS], ExtUI::X);
    request->reset_x = 0;
  }

  if (request->reset_y || request->reset_all) {
    ExtUI::setProbeOffset_mm(default_offset[Y_AXIS], ExtUI::Y);
    request->reset_y = 0;
  }

  if (request->reset_z || request->reset_all) {
    ExtUI::setZOffset_mm(default_offset[Z_AXIS]);
    request->reset_z = 0;
  }
}

void handle_set_probe_offset_z_axis(DGUS_VP_Variable &var, void *val_ptr) {
#if ENABLED(DEBUG_DGUSLCD)
  DEBUG_ECHOLNPGM("handle_set_probe_offset_z_axis");
#endif
  OffsetDistance offset_mm{.as_uint = dgus::swap16(*static_cast<uint16_t *>(val_ptr))};
  offset_mm.as_float = static_cast<float>(offset_mm.as_int) / 100U;

  ExtUI::setZOffset_mm(offset_mm.as_float);
}

} // namespace nozzle_offset
} // namespace dgus_origin

#endif
