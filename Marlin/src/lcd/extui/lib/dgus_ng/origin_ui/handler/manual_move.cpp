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

#include "manual_move.h"

#if ENABLED(DGUS_ORIGIN_MANUAL_MOVE)

#include "../../../../../../gcode/queue.h"
#include "../../../../../../core/types.h"

#include "../../DGUSDisplay.h"

using namespace dgus;

namespace dgus_origin {
namespace manual_move {

extern CachedState cached_state;

namespace {

/**
 * If a move action is cached, identify the corresponding axis and move:
 *   1. switch to relative move mode
 *   2. move with overriden feed rate
 *   3. restore relative/absolute mode
 *   4. restore feed rate
 */
void handle_move_distance() {
  constexpr feedRate_t max_feed_rates[] = DEFAULT_MAX_FEEDRATE;
  constexpr feedRate_t feed_rate_factor = 0.5;
  const auto backup_feed_rate{static_cast<feedRate_t>(roundf(MMS_TO_MMM(feedrate_mm_s)))};
  const bool backup_is_relative_mode{relative_mode};
  uint32_t feed_rate;
  char axiscode;

  if (cached_state.move_command.direction == CachedState::MoveCommand::NO_ACTION)
    return;

  char move_sign{(cached_state.move_command.direction == CachedState::MoveCommand::MOVE_POSITIVE) ? '+' : '-'};
  cached_state.move_command.direction = CachedState::MoveCommand::NO_ACTION;

  if (cached_state.move_command.axis_id == static_cast<uint8_t>(AxisEnum::X_AXIS)) {
    if (!ExtUI::canMove(ExtUI::axis_t::X))
      return;
    constexpr uint32_t constexpr_feed_rate =
        static_cast<uint32_t>(roundf(MMS_TO_MMM(max_feed_rates[AxisEnum::X_AXIS] * feed_rate_factor)));
    feed_rate = constexpr_feed_rate;
    axiscode = 'X';
  } else if (cached_state.move_command.axis_id == static_cast<uint8_t>(AxisEnum::Y_AXIS)) {
    if (!ExtUI::canMove(ExtUI::axis_t::Y))
      return;
    constexpr uint32_t constexpr_feed_rate =
        static_cast<uint32_t>(roundf(MMS_TO_MMM(max_feed_rates[AxisEnum::Y_AXIS] * feed_rate_factor)));
    feed_rate = constexpr_feed_rate;
    axiscode = 'Y';
  } else if (cached_state.move_command.axis_id == static_cast<uint8_t>(AxisEnum::Z_AXIS)) {
    if (!ExtUI::canMove(ExtUI::axis_t::Z))
      return;
    constexpr uint32_t constexpr_feed_rate =
        static_cast<uint32_t>(roundf(MMS_TO_MMM(max_feed_rates[AxisEnum::Z_AXIS] * feed_rate_factor)));
    feed_rate = constexpr_feed_rate;
    axiscode = 'Z';
  } else
    return;

  // switch to relative move mode
  if (!backup_is_relative_mode)
    GCodeQueue::enqueue_now_P(PSTR("G91"));

  // move
  char buf[32]; // "G0 X-99999.99 F12345"
  uint16_t move_value = cached_state.distance_10_um / 100;
  uint16_t move_fraction = cached_state.distance_10_um % 100;

  snprintf_P(buf, sizeof(buf), PSTR("G0 %c%c%d.%02d F%lu"), axiscode, move_sign, move_value, move_fraction, feed_rate);
  GCodeQueue::enqueue_one_now(buf);

  // restore feed rate
  const uint16_t feed_rate_value = static_cast<uint16_t>(backup_feed_rate);
  const uint16_t feed_rate_fraction = (backup_feed_rate - feed_rate_value) * 100;
  snprintf_P(buf, sizeof(buf), PSTR("G0 F%d.%d"), feed_rate_value, feed_rate_fraction);
  GCodeQueue::enqueue_one_now(buf);

  // restore relative mode
  if (!backup_is_relative_mode)
    GCodeQueue::enqueue_now_P(PSTR("G90"));

  // TODO rubienr - necessary?
  DGUSScreenVariableHandler::ForceCompleteUpdate();
}

} // namespace

void handle_move_argument_update(DGUS_VP_Variable &var, void *val_ptr) {
#if ENABLED(DEBUG_DGUSLCD)
  DEBUG_ECHOLNPGM("handle_move_argument_update");
#endif
  *static_cast<uint16_t *>(var.memadr) = dgus::swap16(*static_cast<uint16_t *>(val_ptr));
  handle_move_distance();
}

} // namespace manual_move
} // namespace dgus_origin

#endif
