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

#include "lights.h"

#if ENABLED(DGUS_ORIGIN_LIGHTS)

#include "../../../../../../gcode/queue.h"
#include "../../DGUSDisplay.h"
#if ENABLED(CASE_LIGHT_ENABLE)
#include "../../feature/caselight.h"
#endif

using namespace dgus;

namespace dgus_origin {
namespace lights {

extern CachedState cached_state;

#if ENABLED(HAS_COLOR_LEDS)

namespace {

/**
 * Update LED color according to the bufferd arguments.
 * - clears on/off request flags
 * - updates the on/off state flags for back propagation to display
 */
void update_color_leds() {
  // disable
  if (cached_state.color_led_control_0.disable) {
    GCodeQueue::enqueue_now_P(PSTR("M150 P0 R0 U0 B0 W0"));
    cached_state.color_led_control_0.on_off_unknown = CachedState::ColorLedControl0::OFF;
    cached_state.color_led_control_0.disable = 0;
    ;
  }
  // enable
  if (cached_state.color_led_control_0.enable ||                                              // on enable, or
      cached_state.color_led_control_0.on_off_unknown == CachedState::ColorLedControl0::ON) { // on color changed
    char buf[32];
    sprintf_P(buf,
              PSTR("M150 P%d R%d U%d B%d W%d"),
              cached_state.color_led_control_0.intensity,
              cached_state.color_led_control_1.red,
              cached_state.color_led_control_1.green,
              cached_state.color_led_control_2.blue,
              cached_state.color_led_control_2.white);
    GCodeQueue::enqueue_one_now(buf);

    cached_state.color_led_control_0.on_off_unknown = CachedState::ColorLedControl0::ON;
    cached_state.case_light_control.on_off_unknown = CachedState::CaseLightControl::OFF;
    cached_state.color_led_control_0.enable = 0;
  }
}

} // namespace

void handle_color_led(DGUS_VP_Variable &var, void *val_ptr) {
#if ENABLED(DEBUG_DGUSLCD)
  DEBUG_ECHOLNPGM("handle_color_led_update");
#endif
  *static_cast<uint16_t *>(var.memadr) = dgus::swap16(*static_cast<uint16_t *>(val_ptr));
  update_color_leds();
}

#endif // HAS_COLOR_LEDS

#if ENABLED(CASE_LIGHT_ENABLE)

void handle_case_light(DGUS_VP_Variable &var, void *val_ptr) {
#if ENABLED(DEBUG_DGUSLCD)
  DEBUG_ECHOLN("handle_case_light");
#endif
  *static_cast<uint16_t *>(var.memadr) = dgus::swap16(*static_cast<uint16_t *>(val_ptr));
  auto *case_light_request{static_cast<CachedState::CaseLightControl *>(var.memadr)};

  // disable
  if (case_light_request->disable) {
    // TODO rubienr: turn colour leds almost off, not fully otherwise case light is also off
    queue.enqueue_now_P(PSTR("M150 P1 R0 U0 B1 W0"));
    case_light_request->on_off_unknown = CachedState::CaseLightControl::OFF;
    case_light_request->disable = 0;
  }
  // enable
  const bool do_enable_case_light{case_light_request->enable || // on enabled, or
                                  case_light_request->on_off_unknown ==
                                      CachedState::CaseLightControl::ON}; // on color changed
  if (do_enable_case_light) {
    case_light_request->enable = 0;
    case_light_request->on_off_unknown = CachedState::CaseLightControl::ON;
    cached_state.color_led_control_0.on_off_unknown = CachedState::ColorLedControl0::OFF;
  }

#if DISABLED(CASE_LIGHT_NO_BRIGHTNESS)
  // TODO rubienr - set brightness via ExtUI
  //const float float_brightness = map(case_light_request->intensity, 0, 255, 0, 100);
  //ExtUI::setCaseLightBrightness_percent(float_brightness);
  case_light_brightness = case_light_request->intensity;
  update_case_light();
#else
  char buf[16];
  sprintf_P(buf, PSTR("M355 P%d S%d"), case_light_request->intensity, ((do_enable_case_light) ? 1 : 0));
  queue.enqueue_one_now(buf);
#endif
}

#endif // CASE_LIGHT_ENABLE

} // namespace lights
} // namespace dgus_origin

#endif
