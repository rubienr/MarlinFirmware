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

#include "temperatures.h"

#if ENABLED(DGUS_ORIGIN_TEMPERATURES)

#include "../../../../../../gcode/queue.h"
#include "../../../../../../module/temperature.h"
#include "../../DGUSDisplay.h"
#include "../../../../../../module/motion.h"

using namespace dgus;

namespace dgus_origin {
namespace temperatures {

extern CachedState cached_state;

namespace {

void update_selected_hotend_temperature() {
  cached_state.temperatures.hotend_target_temperature.celsius =
      Temperature::degTargetHotend(cached_state.control.hotend_id);
}

void update_heating_status() {
  cached_state.status.is_hotend_heating = Temperature::isHeatingHotend(cached_state.control.hotend_id);
  cached_state.status.is_hotend_cooling = Temperature::isCoolingHotend(cached_state.control.hotend_id);

#if ENABLED(HAS_HEATED_BED)
  cached_state.status.is_bed_heating = Temperature::isHeatingBed();
  cached_state.status.is_bed_cooling = Temperature::isCoolingBed();
#endif

#if ENABLED(HAS_HEATED_CHAMBER)
  cached_state.status.is_chamber_heating = Temperature::isHeatingChamber();
  cached_state.status.is_chamber_cooling = Temperature::isCoolingChamber();
#endif
}

void handle_preheat() {
  uint16_t hotend_temp;
#if ENABLED(HAS_HEATED_BED)
  uint16_t bed_temp;
#endif
  uint16_t fan_speed;

  if (cached_state.control.do_preset_preheat_1) {
    hotend_temp = PREHEAT_1_TEMP_HOTEND;
#if ENABLED(HAS_HEATED_BED)
    bed_temp = PREHEAT_1_TEMP_BED;
#endif
    fan_speed = PREHEAT_1_FAN_SPEED;
    cached_state.control.do_preset_preheat_1 = 0;
    cached_state.control.do_preset_preheat_2 = 0;
  } else if (cached_state.control.do_preset_preheat_2) {
    hotend_temp = PREHEAT_2_TEMP_HOTEND;
#if ENABLED(HAS_HEATED_BED)
    bed_temp = PREHEAT_2_TEMP_BED;
#endif
    fan_speed = PREHEAT_2_FAN_SPEED;
    cached_state.control.do_preset_preheat_2 = 0;
  } else
    return;

  Temperature::setTargetHotend(hotend_temp, cached_state.control.hotend_id);

#if ENABLED(HAS_HEATED_BED)
  Temperature::setTargetBed(bed_temp);
#endif
  Temperature::set_fan_speed(constrain(cached_state.control.hotend_id, 0, FAN_COUNT - 1), fan_speed);
}

void handle_disable_heating() {
  if (cached_state.control.disable_hotend) {
    Temperature::setTargetHotend(0, cached_state.control.hotend_id);
    cached_state.control.disable_hotend = 0;
  }

#if ENABLED(HAS_HEATED_BED)
  if (cached_state.control.disable_bed) {
    Temperature::setTargetBed(0);
    cached_state.control.disable_bed = 0;
  }
#endif

#if ENABLED(HAS_HEATED_CHAMBER)
  if (cached_state.control.disable_chamber) {
    Temperature::setTargetChamber(0);
    cached_state.control.disable_chamber = 0;
  }
#endif
  if (cached_state.control.disable_all) {
    Temperature::disable_all_heaters();
    cached_state.control.disable_all = 0;
  }
}

} // namespace

void handle_heating_status(DGUS_VP_Variable &var, void *val_ptr) {
#if ENABLED(DEBUG_DGUSLCD)
  DEBUG_ECHOLNPGM("handle_heating_status");
#endif
  *static_cast<uint16_t *>(var.memadr) = dgus::swap16(*static_cast<uint16_t *>(val_ptr));

  update_heating_status();
}

void handle_set_hotend_temperature(DGUS_VP_Variable &var, void *val_ptr) {
#if ENABLED(DEBUG_DGUSLCD)
  DEBUG_ECHOLNPGM("handle_set_hotend_temperature");
#endif
  *static_cast<uint16_t *>(var.memadr) = dgus::swap16(*static_cast<uint16_t *>(val_ptr));

  Temperature::setTargetHotend(cached_state.temperatures.hotend_target_temperature.celsius,
                               cached_state.control.hotend_id);
}

void handle_set_bed_temperature(DGUS_VP_Variable &var, void *val_ptr) {
#if ENABLED(DEBUG_DGUSLCD)
  DEBUG_ECHOLNPGM("handle_set_bed_temperature");
#endif
  dgus::DgusWord_t temperature{.data = *static_cast<uint16_t *>(val_ptr)};
  temperature.data = dgus::swap16(temperature.data);
  Temperature::setTargetBed(temperature.as_int);
}

#if ENABLED(HAS_HEATED_CHAMBER)
void handle_set_chamber_temperature(DGUS_VP_Variable &var, void *val_ptr) {
#if ENABLED(DEBUG_DGUSLCD)
  DEBUG_ECHOLNPGM("handle_set_chamber_temperature");
#endif
  dgus::DgusWord_t temperature{.data = *static_cast<uint16_t *>(val_ptr)};
  temperature.data = dgus::swap16(temperature.data);
  Temperature::setTargetChamber(temperature.as_int);
}
#endif

void handle_temperature_control_command(DGUS_VP_Variable &var, void *val_ptr) {
#if ENABLED(DEBUG_DGUSLCD)
  DEBUG_ECHOLNPGM("handle_temperature_control_command");
#endif
  *static_cast<uint16_t *>(var.memadr) = dgus::swap16(*static_cast<uint16_t *>(val_ptr));

  cached_state.control.hotend_id = constrain(cached_state.control.hotend_id, 0, EXTRUDERS - 1);

  handle_preheat();
  handle_disable_heating();
}

void handle_send_hotend_temperature_to_display(DGUS_VP_Variable &var) {
#if ENABLED(DEBUG_DGUSLCD)
  DEBUG_ECHOLNPGM("handle_send_hotend_temperature_to_display");
#endif
  float t{Temperature::degHotend(cached_state.control.hotend_id)};
  t = roundf(t);
  dgus::DgusFloat_t data{.as_float = t};
  data.as_int_16 = static_cast<int16_t>(data.as_float);
  data.data_16 = dgus::swap16(data.data_16);

  update_selected_hotend_temperature();


  DGUSDisplay::write_variable(var.VP, data.data_16);
}

} // namespace temperatures
} // namespace dgus_origin

#endif
