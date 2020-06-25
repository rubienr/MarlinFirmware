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
#pragma once

#include "../../../../../inc/MarlinConfig.h"

#define DGUS_ORIGIN_VERSIONS

#if ENABLED(DGUS_ORIGIN_VERSIONS)

#include "memory_layout.h"

struct DGUS_VP_Variable;

namespace dgus_origin {
namespace versions {

// ... UI Version always on 0x1000...0x1002 so that the firmware can check this and bail out.
// TODO rubienr:
//  find out what the intention is here. who decides the in-/compatibility:
//    * marlin sends version to display: DGUS decides but that thing is very limited
//    * display sends version to marlin: marlin decides and tries to send the information (M117) about that incident

// cached state
namespace marlin_version {
constexpr char MarlinVersion[16] PROGMEM{SHORT_BUILD_VERSION};
constexpr char MarlinDetailedVersion[32] PROGMEM{DETAILED_BUILD_VERSION};
constexpr char MarlinDistributionDate[16] PROGMEM{STRING_DISTRIBUTION_DATE};
constexpr char MarlinConfigurationAuthor[38] PROGMEM{STRING_CONFIG_H_AUTHOR};
constexpr char MarlinCompileDate[16] PROGMEM{__DATE__};
}; // namespace marlin_version

union UiVersion {
  struct {
    uint8_t major; // major: incremented when incompatible
    uint8_t minor; // minor: incremented on new features, but compatible
    uint8_t patch; // patch: fixed which do not change functionality
    uint8_t _unused;
  } __attribute__((packed)) version;
  struct {
    uint16_t low_word;
    uint16_t high_word;
  } __attribute__((packed));
} __attribute__((packed));

using UiFlavor = uint8_t[to_uint8_t(dgus::memory_layout::UiVersion::FlavourBytes)];

extern const UiVersion ui_version;
extern const UiFlavor ui_flavor;

} // namespace versions
} // namespace dgus_origin

#endif