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

#include "feedrates.h"

#if ENABLED(DGUS_ORIGIN_FEEDRATES)

#include "../memory_layout.h"

namespace dgus_origin {
namespace feedrates {

const uint16_t VPScreenList[] PROGMEM = {VP_Feedrate_Percentage,
#if HAS_FAN0
                                         to_address(dgus::memory_layout::FanSpeed::Fan0Percentage),
#endif
#if HAS_FAN1
                                         to_address(dgus::memory_layout::FanSpeed::Fan1Percentage),
#endif
#if HAS_FAN2
                                         to_address(dgus::memory_layout::FanSpeed::Fan2Percentage),
#endif
#if HAS_FAN3
                                         to_address(dgus::memory_layout::FanSpeed::Fan3Percentage),
#endif
#if HAS_FAN4
                                         to_address(dgus::memory_layout::FanSpeed::Fan4Percentage),
#endif
#if HAS_FAN5
                                         to_address(dgus::memory_layout::FanSpeed::Fan5Percentage),
#endif
                                         0x0000};
} // namespace feedrates
} // namespace dgus_origin

#endif
