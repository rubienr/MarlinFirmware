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

#include "../handler/nozzle_offset.h"

namespace dgus_origin {
namespace nozzle_offset {

// screen variables
extern const uint16_t screen_variables[];
// cached state
extern CachedState cached_state;

} // namespace nozzle_offset
} // namespace dgus_origin
#endif