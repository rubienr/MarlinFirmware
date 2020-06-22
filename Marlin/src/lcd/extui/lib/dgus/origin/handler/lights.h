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

#if ANY(CASE_LIGHT_ENABLE, HAS_COLOR_LEDS)
#define DGUS_ORIGIN_LIGHTS
#else
#error DGUS_ORIGIN_LIGHTS requires at least one or all of: CASE_LIGHT_ENABLE, HAS_COLOR_LEDS.
#endif

#if ENABLED(DGUS_ORIGIN_LIGHTS)

#include "../../../../ui_api.h"
#include "../../DGUSDisplay.h"

struct DGUS_VP_Variable;

namespace dgus_origin {
namespace lights {

/**
 *  flags for display requests
 */
struct CachedState {

#if ENABLED(CASE_LIGHT_ENABLE)
  union CaseLightControl {
    constexpr static const uint8_t UNKNOWN{0};
    constexpr static const uint8_t OFF{1};
    constexpr static const uint8_t ON{2};
    uint16_t data;
    struct {
      uint8_t on_off_unknown : 2;
      uint8_t disable : 1;
      uint8_t enable : 1;
      uint8_t _unused : 4;
      uint8_t intensity;
    } __attribute__((packed));
  } case_light_control;
#endif

#if ENABLED(HAS_COLOR_LEDS)
  union ColorLedControl0 {
    constexpr static const uint8_t UNKNOWN{0};
    constexpr static const uint8_t OFF{1};
    constexpr static const uint8_t ON{2};
    uint16_t data;
    struct {
      uint8_t on_off_unknown : 2;
      uint8_t disable : 1;
      uint8_t enable : 1;
      uint8_t _unused : 4;
      uint8_t intensity;
    } __attribute__((packed));
  } color_led_control_0;

  union ColorLedControl1 {
    uint16_t data;
    struct {
      uint8_t red;
      uint8_t green;
    } __attribute__((packed));
  } color_led_control_1;

  union ColorLedControl2 {
    uint16_t data;
    struct {
      uint8_t blue;
      uint8_t white;
    } __attribute__((packed));
  } color_led_control_2;
#endif
};

#if ENABLED(HAS_COLOR_LEDS)

/**
 * Update LED color according to the bufferd arguments.
 * - clears on/off request flags
 * - updates the on/off state flags for back propagation to display
 * @param var
 * @param val_ptr
 */
void handle_color_led(DGUS_VP_Variable &var, void *val_ptr);

#endif // HAS_COLOR_LEDS

#if ENABLED(CASE_LIGHT_ENABLE)

/**
 *
 * Update case light according to the bufferd arguments.
 * - clears on/off request flags
 * - updates the on/off state flags for back propagation to display
 * @param var
 * @param val_ptr
 */
void handle_case_light(DGUS_VP_Variable &var, void *val_ptr);

#endif // CASE_LIGHT_ENABLE

} // namespace lights
} // namespace dgus_origin

#endif