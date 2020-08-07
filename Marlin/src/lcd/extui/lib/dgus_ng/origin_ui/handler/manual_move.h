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

#define DGUS_ORIGIN_MANUAL_MOVE

#if ENABLED(DGUS_ORIGIN_MANUAL_MOVE)

#include "../../../../ui_api.h"
#include "../../DGUSDisplay.h"

namespace dgus {
struct DGUS_VP_Variable;
}

namespace dgus_origin {
namespace manual_move {

struct CachedState {
  uint16_t distance_10_um;

  union MoveCommand {
    constexpr static const uint8_t NO_ACTION{0};
    constexpr static const uint8_t MOVE_NEGATIVE{1};
    constexpr static const uint8_t MOVE_POSITIVE{2};
    uint16_t data;
    struct {
      uint8_t axis_id : 8;
      uint8_t direction : 2;
      uint8_t _unused : 6;
    } __attribute__((packed));
  } move_command;
};

/**
 * Caches and moves the respecitve axis if move action is specified.
 * The cached action is cleared and propagated to display.
 *
 * @param var var.memadr must not be nullptr but pointer to one of #CachedState.distance, #CachedState.axis
 * #CachedState.move_command
 * @param val_ptr must not be nullptr
 */
void handle_move_argument_update(dgus::DGUS_VP_Variable &var, void *val_ptr);

} // namespace manual_move
} // namespace dgus_origin

#endif