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

#if ENABLED(PSU_CONTROL)
#define DGUS_ORIGIN_PSU_CONTROL
#endif

#if ENABLED(DGUS_ORIGIN_PSU_CONTROL)

#include "../handler/psu_control.h"

namespace dgus_origin {
namespace psu_control {

// screen variables
extern const uint16_t VPScreenList[] PROGMEM;
// cached state
extern CachedState cached_state;

} // namespace psu_control
} // namespace dgus_origin

#endif