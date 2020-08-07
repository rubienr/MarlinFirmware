/**
 * Marlin 3D Printer Firmware
 * Copyright (c) 2020 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (c) 2011 Camiel Gubbels / Erik van der Zalm
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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/* DGUS implementation written by coldtobi in 2019 for Marlin */

#include "../../../../../inc/MarlinConfig.h"

#if ENABLED(HAS_DGUS_LCD)

uint8_t constexpr constexpr_strlen_uint8(const char *str) { return (*str) ? (1 + constexpr_strlen_uint8(str + 1)) : 0; }

namespace vp_macros {

constexpr uint8_t sizeof_vp_adress_variable(nullptr_t) { return 0; }
template <typename t> constexpr uint8_t sizeof_vp_adress_variable(const t *) { return sizeof(t); }

constexpr uint16_t to_address(nullptr_t) { return 0; }
template <typename t> constexpr uint16_t to_address(const t a) { return static_cast<uint16_t>(a);}

} // namespace vp_macros

// Helper to define a DGUS_VP_Variable for common use cases.
#define VPHELPER(VPADR, VPADRVAR, RXFPTR, TXFPTR)                                            \
  {                                                                                          \
    .VP = vp_macros::to_address(VPADR), .memadr = VPADRVAR, .size = vp_macros::sizeof_vp_adress_variable(VPADRVAR), \
    .set_by_display_handler = RXFPTR, .send_to_display_handler = TXFPTR                      \
  }

// Helper to define a DGUS_VP_Variable when the sizeo of the var cannot be determined automatically.
#define VPHELPER_STR(VPADR, VPADRVAR, STRLEN, RXFPTR, TXFPTR)                                     \
  {                                                                                               \
    .VP = vp_macros::to_address(VPADR), .memadr = VPADRVAR, .size = static_cast<uint8_t>(STRLEN), .set_by_display_handler = RXFPTR, \
    .send_to_display_handler = TXFPTR                                                             \
  }

#endif // HAS_DGUS_LCD
