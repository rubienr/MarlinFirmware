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
#pragma once

#include "../../../../../../inc/MarlinConfig.h"

#if ENABLED(HAS_BED_PROBE)
#define DGUS_ORIGIN_NOZZLE_OFFSET
#endif

#if ENABLED(DGUS_ORIGIN_NOZZLE_OFFSET)

#include "../../../../ui_api.h"
#include "../../DGUSDisplay.h"

struct DGUS_VP_Variable;

namespace dgus_origin {
namespace nozzle_offset {

union CachedState {
  struct {
    uint8_t reset_x_flag : 1;
    uint8_t reset_y_flag : 1;
    uint8_t reset_z_flag : 1;
    uint8_t reset_flag : 1;
    uint8_t _unused : 4;
  } __attribute__((packed));
  uint16_t data;
};

/**
 * Handle and clear probe offset request flags.
 * @param var buffered flags that are back propagated to display; memptr must not be nullptr
 * @param val_ptr display request flags; must not be nullptr
 */
void handle_probe_offset(DGUS_VP_Variable &var, void *val_ptr);
/**
 * Take probe offset and write to axis withtout boundary check.
 * @tparam axis axis idendifier
 * @param var local axis offset value; probe.offset.x or .y, \sa
 * #HandleProbeOffsetZAxis(DGUS_VP_Variable &, void *)
 * @param val_ptr display axis offset request
 */
template <ExtUI::axis_t axis> void handle_probe_offset_axis(DGUS_VP_Variable &var, void *val_ptr);
/**
 * Take probe offset and write to Z-axis with boundary check.
 * @param var local axis offset value; probe.offset.z
 * @param val_ptr display axis offset request
 */
void handle_probe_offset_z_axis(DGUS_VP_Variable &var, void *val_ptr);

} // namespace nozzle_offset
} // namespace dgus_origin

namespace dgus_origin {
namespace nozzle_offset {

template <ExtUI::axis_t axis> void handle_probe_offset_axis(DGUS_VP_Variable &var, void *val_ptr) {
  DEBUG_ECHOLNPGM("HandleProbeOffsetAxis");
  UNUSED(var);
  if (val_ptr == nullptr)
    return;
  union {
    float as_float;
    uint16_t as_uint;
    int16_t as_int;
  } offset_mm{.as_uint = Dgus::swap16(*static_cast<uint16_t *>(val_ptr))};
  offset_mm.as_float = static_cast<float>(offset_mm.as_int) / 100U;
  ExtUI::setProbeOffset_mm(offset_mm.as_float, axis);
}

} // namespace nozzle_offset
} // namespace dgus_origin
#endif