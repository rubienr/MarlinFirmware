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

#include "versions.h"

#if ENABLED(DGUS_ORIGIN_VERSIONS)

#include "macros.h"

namespace dgus_origin {
namespace versions {

const UiVersion ui_version{.version = {.major = 1, .minor = 0, .patch = 1, ._unused = 0}};

#ifndef DGUS_FLAVOUR
#define DGUS_FLAVOUR "DGUS"
#endif

const UiFlavor ui_flavor PROGMEM{DGUS_FLAVOUR};
static_assert(sizeof(DGUS_FLAVOUR) <= sizeof(UiFlavor), "Failed to declare UI flavour.");

} // namespace versions
} // namespace dgus_origin

#endif
