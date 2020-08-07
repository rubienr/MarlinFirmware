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

#define DGUS_ORIGIN_SPEED_RATES

#if ENABLED(DGUS_ORIGIN_SPEED_RATES)

#include "../../../../ui_api.h"
#include "../../DGUSDisplay.h"

namespace dgus {
struct DGUS_VP_Variable;
}

namespace dgus_origin {
namespace speedrates {

struct CachedStateFlowRate {
  union Control {
    uint16_t data;
    struct {
      uint8_t _unused : 8;
      uint8_t extruder_id : 8;
    } __attribute__((packed));
  } control;

  union Status {
    uint16_t data;
    struct {
      uint8_t _unused : 8;
      uint8_t _unused2 : 8;
    } __attribute__((packed));
  } status;

  union Speed {
    uint16_t data;
    int16_t percent;
  } rate;
};

struct CachedStateFanRate {
  union Control {
    uint16_t data;
    struct {
      uint8_t _unused : 8;
      uint8_t fan_id : 8;
    } __attribute__((packed));
  } control;

  union Status {
    uint16_t data;
    struct {
      uint8_t _unused : 8;
      uint8_t _unused2 : 8;
    } __attribute__((packed));
  } status;

  union Speed {
    uint16_t data;
    struct {
      uint8_t percent;
      uint8_t pwm;
    } __attribute__((packed));
  } rate;
};

void handle_fan_control_command(dgus::DGUS_VP_Variable &var, void *val_ptr);

void handle_set_fan_rate(dgus::DGUS_VP_Variable &var, void *val_ptr);

void handle_send_fan_rate(dgus::DGUS_VP_Variable &var);

void handle_flow_control_command(dgus::DGUS_VP_Variable &var, void *val_ptr);

void handle_set_flow_rate(dgus::DGUS_VP_Variable &var, void *val_ptr);

void handle_send_flow_rate(dgus::DGUS_VP_Variable &var);

} // namespace speedrates
} // namespace dgus_origin
#endif