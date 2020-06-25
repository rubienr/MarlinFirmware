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

#include "flowrates.h"

#if ENABLED(DGUS_ORIGIN_FLOWRATES)

#include "../memory_layout.h"

namespace dgus_origin {
namespace flowrates {

const uint16_t screen_variables_1[] PROGMEM {
#if EXTRUDERS >= 1
  to_address(dgus::memory_layout::Flowrates::E0),
#endif
#if EXTRUDERS >= 2
      to_address(dgus::memory_layout::Flowrates::E1),
#endif
#if EXTRUDERS >= 3
      to_address(dgus::memory_layout::Flowrates::E2),
#endif
      0x0000
};

const uint16_t screen_variables_2[] PROGMEM {
#if EXTRUDERS >= 4
  to_address(dgus::memory_layout::Flowrates::E3),
#endif
#if EXTRUDERS >= 5
      to_address(dgus::memory_layout::Flowrates::E4),
#endif
#if EXTRUDERS >= 6
      to_address(dgus::memory_layout::Flowrates::E5),
#endif
      0x0000
};

} // namespace flowrates
} // namespace dgus_origin

#endif
