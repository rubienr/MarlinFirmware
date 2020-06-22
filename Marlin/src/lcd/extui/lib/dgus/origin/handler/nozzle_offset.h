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
  uint16_t data;
  struct {
    // flags set by dispaly
    uint8_t reset_x : 1;
    uint8_t reset_y : 1;
    uint8_t reset_z : 1;
    uint8_t reset_all : 1;
    uint8_t _unused : 4;
    // flags set internally
    uint8_t _unused2;
  } __attribute__((packed));
};

union OffsetDistance {
  float as_float;
  uint16_t as_uint;
  int16_t as_int;
};

/**
 * Handle and clear probe offset request flags.
 *
 * @param var
 * @param val_ptr
 */
void handle_probe_offset_request(DGUS_VP_Variable &var, void *val_ptr);
/**
 * Take probe offset and write to axis without boundary check.
 *
 * @tparam axis
 * @param var
 * @param val_ptr
 */
template <ExtUI::axis_t axis> void handle_set_probe_offset_axis(DGUS_VP_Variable &var, void *val_ptr);
/**
 * Take new probe offset and write to Z-axis with boundary check.
 *
 * @param var
 * @param val_ptr
 */
void handle_set_probe_offset_z_axis(DGUS_VP_Variable &var, void *val_ptr);

} // namespace nozzle_offset
} // namespace dgus_origin

namespace dgus_origin {
namespace nozzle_offset {

template <ExtUI::axis_t axis> void handle_set_probe_offset_axis(DGUS_VP_Variable &var, void *val_ptr) {
#if ENABLED(DEBUG_DGUSLCD)
  DEBUG_ECHOLNPGM("handle_set_probe_offset_axis");
#endif
  UNUSED(var);
  OffsetDistance offset_mm{.as_uint = dgus::swap16(*static_cast<uint16_t *>(val_ptr))};
  offset_mm.as_float = static_cast<float>(offset_mm.as_int) / 100U;

  ExtUI::setProbeOffset_mm(offset_mm.as_float, axis);
}

} // namespace nozzle_offset
} // namespace dgus_origin
#endif