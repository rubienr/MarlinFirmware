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

namespace dgus_origin {
namespace lights {

extern CachedState cached_state;

#if ENABLED(HAS_COLOR_LEDS)

namespace {

/**
 * Update LED color according to the bufferd arguments:
 * #CachedState::ColorLedControlN
 * - clears on/off request flags
 * - updates the on/off state flags for back propagation to display

 */
void update_color_leds() {
  // disable
  if (cached_state.color_led_control_0.disable) {
    queue.enqueue_now_P(PSTR("M150 P0 R0 U0 B0 W0"));

    cached_state.color_led_control_0.disable = 0;
    cached_state.color_led_control_0.enabled = 0;
    cached_state.color_led_control_0.disabled = 1;
  }

  if (
      // on enable
      cached_state.color_led_control_0.enable ||
      // on color changed
      cached_state.color_led_control_0.enabled) {
    char buf[32]{0};
    sprintf(buf,
            "M150 P%d R%d U%d B%d W%d",
            cached_state.color_led_control_0.intensity,
            cached_state.color_led_control_1.red,
            cached_state.color_led_control_1.green,
            cached_state.color_led_control_2.blue,
            cached_state.color_led_control_2.white);
    queue.enqueue_one_now(buf);

    cached_state.color_led_control_0.enable = 0;
    cached_state.color_led_control_0.enabled = 1;
    cached_state.color_led_control_0.disabled = 0;
  }
}

} // namespace

void handle_color_led_update(DGUS_VP_Variable &var, void *val_ptr) {
#if ENABLED(DEBUG_DGUSLCD)
  DEBUG_ECHOLNPGM("handle_color_led_update");
#endif
  *static_cast<uint16_t *>(var.memadr) = Dgus::swap16(*static_cast<uint16_t *>(val_ptr));
  update_color_leds();
}

#endif // HAS_COLOR_LEDS

#if ENABLED(CASE_LIGHT_ENABLE)

void handle_case_light(DGUS_VP_Variable &var, void *val_ptr) {
  DEBUG_ECHOLNPGM("handle_case_light");

  CachedState::CaseLightControl *case_light_request = static_cast<CachedState::CaseLightControl *>(val_ptr);

  const bool do_enable_case_light{!case_light_request->enabled && case_light_request->enable};

  if (do_enable_case_light) {
    // ExtUI::setCaseLightState(false);
    case_light_request->enable = 0;
  }

  if (case_light_request->disable) {
    // ExtUI::setCaseLightState(true);
    // TODO rubienr: turn colour leds almost off, not full otherwise case light is also off
    queue.enqueue_now_P(PSTR("M150 P1 R0 U0 B1 W0"));
    case_light_request->disable = 0;
  }

#if DISABLED(CASE_LIGHT_NO_BRIGHTNESS)
  const float float_brightness = map(case_light_request->intensity, 0, 255, 0, 100);
  void setCaseLightBrightness_percent(float_brightness);
#else
  char buf[16] = {0};
  sprintf(buf, "M355 P%d S%d", case_light_request->intensity, ((do_enable_case_light) ? 1 : 0));
  queue.enqueue_one_now(buf);
#endif

  case_light_request->enabled = ExtUI::getCaseLightState();
  case_light_request->disabled = !case_light_request->enabled;

  *static_cast<uint16_t *>(var.memadr) = case_light_request->data;
}

#endif // CASE_LIGHT_ENABLE

} // namespace lights
} // namespace dgus_origin

#endif
