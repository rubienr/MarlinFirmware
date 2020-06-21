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

#include "manual_extrude.h"

#if ENABLED(DGUS_ORIGIN_MANUAL_EXTRUDE)

namespace dgus_origin {
namespace manual_extrude {

const uint16_t VPScreenList[] PROGMEM = {
    /*
    #if EXTRUDERS >= 1
        to_address(dgus::memory_layout::PoseitionE::E0),
    #endif
    #if EXTRUDERS >= 2
        to_address(dgus::memory_layout::PoseitionE::E1),
    #endif
    #if EXTRUDERS >= 3
        to_address(dgus::memory_layout::PoseitionE::E2),
    #endif
    #if EXTRUDERS >= 4
        to_address(dgus::memory_layout::PoseitionE::E3),
    #endif
    #if EXTRUDERS >= 5
        to_address(dgus::memory_layout::PoseitionE::E4),
    #endif
    #if EXTRUDERS >= 6
        to_address(dgus::memory_layout::PoseitionE::E5),
    #endif
    */
    0x0000};

} // namespace manual_extrude
} // namespace dgus_origin

#endif
