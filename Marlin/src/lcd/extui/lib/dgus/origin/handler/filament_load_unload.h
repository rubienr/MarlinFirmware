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

#if ENABLED(FILAMENT_LOAD_UNLOAD_GCODES)
#define DGUS_ORIGIN_FILAMENT_LOAD_UNLOAD
#endif

#if ENABLED(DGUS_ORIGIN_FILAMENT_LOAD_UNLOAD)

#include "../../../../ui_api.h"
#include "../../DGUSDisplay.h"

struct DGUS_VP_Variable;

namespace dgus_origin {
namespace filament {

/**
 * flags for display requests; cleared internally
 */
union CachedState {
  constexpr static const uint8_t CLEAR{0};
  constexpr static const uint8_t ERROR{1};
  constexpr static const uint8_t OK{2};
  uint16_t data;
  struct {
    uint8_t unload : 1;
    uint8_t load : 1;
    uint8_t ok_error_clear : 2;
    uint8_t _unused : 4;
    uint8_t extruder_id;
  } __attribute__((packed));
};

/**
 * Caches display request/arguments and performs filament load unload requests.
 *
 * Uses M70x gcodes and ADVANCED_PAUSE_FEATURE and FILAMENT_LOAD_UNLOAD_GCODES.
 * - clears flags once request is handled for back propagation to display
 *
 * @param var
 * @param val_ptr
 */
void handle_filament_load_unload(DGUS_VP_Variable &var, void *val_ptr);

} // namespace filament
} // namespace dgus_origin
#endif