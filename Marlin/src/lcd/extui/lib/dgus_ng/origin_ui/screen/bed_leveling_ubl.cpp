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

#include "bed_leveling_ubl.h"

#if ENABLED(DGUS_ORIGIN_AUTO_BED_LEVELING_UBL)
#include "../memory_layout.h"

namespace dgus_origin {
namespace bed_leveling_ubl {

CachedState cached_state{0, 0, 0};

const uint16_t screen_variables[] PROGMEM{to_address(dgus::memory_layout::Ubl::RequestFlags),
                                          to_address(dgus::memory_layout::Ubl::FadeHeightSlotNnumber),
                                          to_address(dgus::memory_layout::Ubl::OnOoffUnused),
                                          0x0000};

} // namespace bed_leveling_ubl
} // namespace dgus_origin

#endif
