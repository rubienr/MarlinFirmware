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

#include "../../../../../../inc/MarlinConfig.h"

#if ENABLED(EEPROM_SETTINGS)
#define DGUS_ORIGIN_FILAMENT_EEPROM
#endif

#if ENABLED(DGUS_ORIGIN_FILAMENT_EEPROM)

#include "../../../../ui_api.h"

struct DGUS_VP_Variable;

namespace dgus_origin {
namespace eeprom {

union CachedState {
  uint16_t data;
  constexpr static const uint8_t UNKNOWN{0};
  constexpr static const uint8_t SAVED{1};
  constexpr static const uint8_t LOADED{2};
  constexpr static const uint8_t RESTORED{4};

  struct {
    uint8_t restore_to_factory_settings : 1;
    uint8_t load_from_eeprom : 1;
    uint8_t save_to_eeprom : 1;
    uint8_t _unused : 5;
    uint8_t loaded_saved_restored_unknown : 3;
    uint8_t _unused2 : 5;
  } __attribute__((packed));
};

// --- handler ---

/**
 * Handles eeprom store/restore/factory-reset requests.
 * - clears request flags for back propagation to display
 *
 * @param var
 * @param val_ptr
 */
void handle_eeprom_store_restore(DGUS_VP_Variable &var, void *val_ptr);

} // namespace eeprom
} // namespace dgus_origin

#endif