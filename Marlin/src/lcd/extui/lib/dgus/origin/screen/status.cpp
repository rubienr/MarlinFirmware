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

const uint16_t screen_variables_1[] PROGMEM {
  to_address(dgus::memory_layout::TemperatureControl::Control),
#if HOTENDS > 0
      to_address(dgus::memory_layout::Temperature::ExtruderNIs),
      to_address(dgus::memory_layout::Temperature::ExtruderNSet),
#endif
#if ENABLED(HAS_HEATED_BED)
      to_address(dgus::memory_layout::Temperature::BedIs), to_address(dgus::memory_layout::Temperature::BedSet),
#endif
#if ENABLED(HAS_HEATED_CHAMBER)
      to_address(dgus::memory_layout::Temperature::ChamberIs), to_address(dgus::memory_layout::Temperature::ChamberSet),
#endif

      to_address(dgus::memory_layout::PositionAxis::X), to_address(dgus::memory_layout::PositionAxis::Y),
      to_address(dgus::memory_layout::PositionAxis::Z), to_address(dgus::memory_layout::PositionAxis::E),
      to_address(dgus::memory_layout::FeedRate::Percentage), to_address(dgus::memory_layout::FlowRate::ExtruderN),
      to_address(dgus::memory_layout::FlowRate::Control),
#if FAN_COUNT > 0
      to_address(dgus::memory_layout::FanSpeed::FanN), to_address(dgus::memory_layout::FanSpeed::Control),
#endif
      to_address(dgus::memory_layout::PrintStats::PrintProgressPercentage), 0x0000
};

const uint16_t screen_variables_2[] PROGMEM{to_address(dgus::memory_layout::PrintStats::PrintTime),
                                            to_address(dgus::memory_layout::PrintStats::PrintProgressPercentage),
                                            0x0000};

} // namespace status
} // namespace dgus_origin

#endif
