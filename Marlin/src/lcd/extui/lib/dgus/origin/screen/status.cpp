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

#include "status.h"

#if ENABLED(DGUS_ORIGIN_STATUS)

#include "../memory_layout.h"

namespace dgus_origin {
namespace status {

const uint16_t VPScreenList1[] PROGMEM = {
#if HOTENDS >= 1
    to_address(dgus::memory_layout::Temperatures::E0Is),
    to_address(dgus::memory_layout::Temperatures::E0Set),
#endif
#if HOTENDS >= 2
    to_address(dgus::memory_layout::Temperatures::E1Is),
    to_address(dgus::memory_layout::Temperatures::E1Set),
#endif
#if HOTENDS >= 3
    to_address(dgus::memory_layout::Temperatures::E2Is),
    to_address(dgus::memory_layout::Temperatures::E2Set),
#endif
#if HOTENDS >= 4
    to_address(dgus::memory_layout::Temperatures::E3Is),
    to_address(dgus::memory_layout::Temperatures::E3Set),
#endif
#if HOTENDS >= 5
    to_address(dgus::memory_layout::Temperatures::E4Is),
    to_address(dgus::memory_layout::Temperatures::E4Set),
#endif
#if HOTENDS >= 6
    to_address(dgus::memory_layout::Temperatures::E5Is),
    to_address(dgus::memory_layout::Temperatures::E5Set),
#endif
#if HAS_HEATED_BED
    to_address(dgus::memory_layout::Temperatures::BedIs),
    to_address(dgus::memory_layout::Temperatures::BedSet),
#endif
#if HAS_FAN0
    to_address(dgus::memory_layout::FanSpeed::Fan0Percentage),

#endif
#if FAN_COUNT >= 2
    to_address(dgus::memory_layout::FanSpeed::Fan1Percentage),
#endif
#if FAN_COUNT >= 3
    to_address(dgus::memory_layout::FanSpeed::Fan2Percentage),
#endif
#if FAN_COUNT >= 4
    to_address(dgus::memory_layout::FanSpeed::Fan3Percentage),
#endif
#if FAN_COUNT >= 5
    to_address(dgus::memory_layout::FanSpeed::Fan4Percentage),
#endif
#if FAN_COUNT >= 6
    to_address(dgus::memory_layout::FanSpeed::Fan5Percentage),
#endif
    to_address(dgus::memory_layout::Position::X),
    to_address(dgus::memory_layout::Position::Y),
    to_address(dgus::memory_layout::Position::Z),
    to_address(dgus::memory_layout::FeedRate::Percentage),
    to_address(dgus::memory_layout::PrintStats::PrintProgressPercentage),
    0x0000}; // namespace fowrates

const uint16_t VPScreenList2[] PROGMEM = {
#if EXTRUDERS >= 1
    to_address(dgus::memory_layout::Flowrates::E0),
#endif
#if EXTRUDERS >= 2
    to_address(dgus::memory_layout::Flowrates::E1),
#endif
#if EXTRUDERS >= 3
    to_address(dgus::memory_layout::Flowrates::E2),
#endif
#if EXTRUDERS >= 4
    to_address(dgus::memory_layout::Flowrates::E3),
#endif
#if EXTRUDERS >= 5
    to_address(dgus::memory_layout::Flowrates::E4),
#endif
#if EXTRUDERS >= 6
    to_address(dgus::memory_layout::Flowrates::E5),
#endif
    to_address(dgus::memory_layout::PrintStats::PrintProgressPercentage),
    to_address(dgus::memory_layout::PrintStats::PrintTime),
    0x0000};

} // namespace status
} // namespace dgus_origin

#endif
