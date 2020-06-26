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

#include "filament_load_unload.h"

#if ENABLED(DGUS_ORIGIN_FILAMENT_LOAD_UNLOAD)

#include "../memory_layout.h"
namespace dgus_origin {
namespace filament {

CachedState cached_state{{0},{100}};
const uint16_t screen_variables[] PROGMEM{to_address(dgus::memory_layout::Filament::LoadUnloadControl),
                                          to_address(dgus::memory_layout::Filament::ExtrudeRetractControl),
                                          0x0000};

} // namespace filament
} // namespace dgus_origin

#endif
