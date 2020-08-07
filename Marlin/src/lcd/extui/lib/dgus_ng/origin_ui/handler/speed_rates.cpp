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

#include "speed_rates.h"

#if ENABLED(DGUS_ORIGIN_SPEED_RATES)

#include "../../../../../../gcode/queue.h"
#include "../../../../../../module/temperature.h"
#include "../../DGUSDisplay.h"
#include "../../../../../../module/planner.h"

using namespace dgus;

namespace dgus_origin {
namespace speedrates {

extern CachedStateFanRate cached_state_fan;
extern CachedStateFlowRate cached_state_flow;

namespace {

void update_selected_fan_rate() {
  cached_state_fan.rate.percent = Temperature::fanPercent(Temperature::fan_speed[cached_state_fan.control.fan_id]);
  cached_state_fan.rate.pwm = Temperature::fan_speed[cached_state_fan.control.fan_id];
}

void set_selected_fan_rate() {
  Temperature::set_fan_speed(cached_state_fan.control.fan_id, map(cached_state_fan.rate.percent, 0, 100, 0, 255));
  update_selected_fan_rate();
}

void update_selected_extruder_flow_rate() {
  cached_state_flow.rate.percent = Planner::flow_percentage[cached_state_flow.control.extruder_id];
}

void set_selected_extruder_flow_rate() {
  Planner::flow_percentage[cached_state_flow.control.extruder_id] = cached_state_flow.rate.percent;
  Planner::refresh_e_factor(cached_state_flow.control.extruder_id);
  update_selected_extruder_flow_rate();
}

} // namespace

void handle_fan_control_command(DGUS_VP_Variable &var, void *val_ptr) {
#if ENABLED(DEBUG_DGUSLCD)
  DEBUG_ECHOLNPGM("handle_fan_control_command");
#endif
  *static_cast<uint16_t *>(var.memadr) = dgus::swap16(*static_cast<uint16_t *>(val_ptr));

  cached_state_fan.control.fan_id = constrain(cached_state_fan.control.fan_id, 0, FAN_COUNT - 1);
  update_selected_fan_rate();
}

void handle_set_fan_rate(DGUS_VP_Variable &var, void *val_ptr) {
#if ENABLED(DEBUG_DGUSLCD)
  DEBUG_ECHOLNPGM("handle_set_fan_rate");
#endif
  *static_cast<uint16_t *>(var.memadr) = dgus::swap16(*static_cast<uint16_t *>(val_ptr));
  set_selected_fan_rate();
}

void handle_send_fan_rate(DGUS_VP_Variable &var) {
#if ENABLED(DEBUG_DGUSLCD)
  DEBUG_ECHOLNPGM("handle_send_fan_rate");
#endif
  update_selected_fan_rate();

  dgus::DgusWord_t status{.data = cached_state_fan.rate.data};
  status.data = dgus::swap16(status.data);
  DGUSDisplay::write_variable(var.VP, status.data);
}

void handle_flow_control_command(DGUS_VP_Variable &var, void *val_ptr) {
#if ENABLED(DEBUG_DGUSLCD)
  DEBUG_ECHOLNPGM("handle_flow_control_command");
#endif
  *static_cast<uint16_t *>(var.memadr) = dgus::swap16(*static_cast<uint16_t *>(val_ptr));

  cached_state_flow.control.extruder_id = constrain(cached_state_flow.control.extruder_id, 0, EXTRUDERS - 1);
  update_selected_extruder_flow_rate();
}

void handle_set_flow_rate(DGUS_VP_Variable &var, void *val_ptr) {
#if ENABLED(DEBUG_DGUSLCD)
  DEBUG_ECHOLNPGM("handle_set_flow_rate");
#endif
  *static_cast<uint16_t *>(var.memadr) = dgus::swap16(*static_cast<uint16_t *>(val_ptr));
  set_selected_extruder_flow_rate();
}

void handle_send_flow_rate(DGUS_VP_Variable &var) {
#if ENABLED(DEBUG_DGUSLCD)
  DEBUG_ECHOLNPGM("handle_send_flow_rate");
#endif
  update_selected_extruder_flow_rate();

  dgus::DgusWord_t status{.data = cached_state_flow.rate.data};
  status.data = dgus::swap16(status.data);
  DGUSDisplay::write_variable(var.VP, status.data);
}

} // namespace speedrates
} // namespace dgus_origin

#endif
