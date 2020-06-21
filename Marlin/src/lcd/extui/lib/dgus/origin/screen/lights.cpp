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

#include "lights.h"

#if ENABLED(DGUS_ORIGIN_LIGHTS)

namespace dgus_origin {
namespace lights {

#include "../memory_layout.h"

CachedState cached_state;

const uint16_t VPScreenList[] PROGMEM = {
#if ENABLED(CASE_LIGHT_ENABLE)
    to_address(dgus::memory_layout::CaseLight::Control),
#endif
#if ENABLED(HAS_COLOR_LEDS)
    to_address(dgus::memory_layout::ColorLeds::Control0),
    to_address(dgus::memory_layout::ColorLeds::Control1),
    to_address(dgus::memory_layout::ColorLeds::Control2),
#endif
    0x0000};

} // namespace lights
} // namespace dgus_origin

#endif
