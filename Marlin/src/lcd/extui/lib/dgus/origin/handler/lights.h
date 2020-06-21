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
    static constexpr uint8_t UNSET = 0;
    static constexpr uint8_t OFF = 1;
    static constexpr uint8_t ON = 2;
    struct {
      // falgs set by display
      uint8_t disable : 1;
      uint8_t enable : 1;
      // falgs set internally
      uint8_t disabled : 1;
      uint8_t enabled : 1;
      uint8_t _unused : 4;
      uint8_t intensity;
    } __attribute__((packed));
    uint16_t data;
  } case_light_control{.data = 0};
#endif

#if ENABLED(HAS_COLOR_LEDS)

  union ColorLedControl0 {
    uint16_t data;
    struct {
      // falgs set by display
      uint8_t disable : 1;
      uint8_t enable : 1;
      // falgs set internally
      uint8_t disabled : 1;
      uint8_t enabled : 1;
      uint8_t _unused : 4;
      uint8_t intensity;
    } __attribute__((packed));
  } color_led_control_0{.data = 0};

  union ColorLedControl1 {
    uint16_t data;
    struct {
      uint8_t red;
      uint8_t green;

    } __attribute__((packed));
  } color_led_control_1{.data = 0};

  union ColorLedControl2 {
    uint16_t data;
    struct {
      uint8_t blue;
      uint8_t white;
    } __attribute__((packed));
  } color_led_control_2{.data = 0};
#endif
};

#if ENABLED(HAS_COLOR_LEDS)

/**
 * Buffers the given LED argument and updates the LED color.
 *
 * @param var DGUS variable of one of CachedState::ColorLedControlN; memaddr mustn't be nullptr
 * @param val_ptr one of VP_CASE_COLOR_LED_CONTROL*
 */
void handle_color_led_update(DGUS_VP_Variable &var, void *val_ptr);

#endif // HAS_COLOR_LEDS

#if ENABLED(CASE_LIGHT_ENABLE)

/**
 * Enable, disable or change case light intensity.
 * - clears on/off request flags
 * - updates the on/off state flags for back propagation to display
 * @param var memaddr mustn't be nullptr
 * @param val_ptr low byte on/off, high byte brightness
 */
void handle_case_light(DGUS_VP_Variable &var, void *val_ptr);

#endif // CASE_LIGHT_ENABLE

} // namespace lights
} // namespace dgus_origin

#endif