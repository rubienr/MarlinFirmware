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
  struct {
    uint8_t unload_flag : 1;
    uint8_t load_flag : 1;
    uint8_t _pad : 6;
    uint8_t extruder_id;
  } __attribute__((packed));
  uint16_t data;
};

/**
 * Loads or unloads filament using M70x gcodes. Requires ADVANCED_PAUSE_FEATURE
 * and FILAMENT_LOAD_UNLOAD_GCODES. Takes display request (flags) and arguments,
 * stores them locally, clears the request (flags) once request is executed and
 * sends the updated arguments back to display.
 * @param var locally cached arguments
 * @param val_ptr display request arguments
 */
void handle_filament_load_unload(DGUS_VP_Variable &var, void *val_ptr);

} // namespace filament
} // namespace dgus_origin
#endif