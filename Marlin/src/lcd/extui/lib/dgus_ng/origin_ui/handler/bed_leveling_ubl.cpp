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

#include "bed_leveling_ubl.h"

#if ENABLED(DGUS_ORIGIN_AUTO_BED_LEVELING_UBL)

#include "../../../../../../gcode/queue.h"
#include "../../DGUSDisplay.h"
#include "../../../../ui_api.h"

using namespace dgus;

namespace dgus_origin {
namespace bed_leveling_ubl {

extern CachedState cached_state;

namespace {

/**
 * Handle UBL reqeusts according to the bufferd parameters.
 */
void handle_request_flags() {

  // --- do UBL probing ---

  if (cached_state.request_flags.start_ubl) {
#if ENABLED(DEBUG_DGUSLCD)
    DEBUG_ECHOLNPGM("handle_request_flags: probe mesh");
#endif
    GCodeQueue::enqueue_now_P(PSTR("G28 R X Y")); // home
    GCodeQueue::enqueue_now_P(PSTR("G28 Z"));     // home
    GCodeQueue::enqueue_now_P(PSTR("G29 P1"));    // probe mesh
    GCodeQueue::enqueue_now_P(PSTR("G29 P3"));    // interpolate missing points
    cached_state.request_flags.start_ubl = 0;
  }

  // --- enable/disable UBL ---

  if (cached_state.request_flags.enable_ubl) {
#if ENABLED(DEBUG_DGUSLCD)
    DEBUG_ECHOLNPGM("handle_request_flags: enable UBL");
#endif
    GCodeQueue::enqueue_now_P(PSTR("M420 S1"));

    cached_state.on_off_state.on_off_unknown = CachedState::OnOff::ON;
    cached_state.request_flags.enable_ubl = 0;
  }

  if (cached_state.request_flags.disable_ubl) {
#if ENABLED(DEBUG_DGUSLCD)
    DEBUG_ECHOLNPGM("handle_request_flags: disable UBL");
#endif
    GCodeQueue::enqueue_now_P(PSTR("M420 S0"));
    cached_state.on_off_state.on_off_unknown = CachedState::OnOff::OFF;
    cached_state.request_flags.disable_ubl = 0;
  }

  // --- load mesh from slot in EEPROM ---

  if (cached_state.request_flags.load_mesh) {
#if ENABLED(DEBUG_DGUSLCD)
    DEBUG_ECHOLNPGM("handle_request_flags: load mesh");
#endif
    // --- load mesh ---
    char buf[12];
    sprintf_P(buf, PSTR("M420 L%d"), cached_state.fade_height_slot_number.slot_number);
    GCodeQueue::enqueue_one_now(buf);

// --- populate details to display ---
#if ENABLED(HAS_LEVELING)
#if ENABLED(HAS_MESH)
    const uint16_t z_fade_height_percent_uint16_t =
        constrain(static_cast<uint16_t>(roundf(ExtUI::getMeshFadeHeight() * 10)), 0, 255);
    const uint8_t z_fade_height_mm_percent = static_cast<uint8_t>(z_fade_height_percent_uint16_t);

    cached_state.fade_height_slot_number.fade_height = z_fade_height_mm_percent;
#endif // HAS_MESH
    cached_state.on_off_state.on_off_unknown =
        (ExtUI::getLevelingActive()) ? CachedState::OnOff::ON : CachedState::OnOff::OFF;

#endif // HAS_LEVELING
    cached_state.request_flags.load_mesh = 0;

    // --- save mesh to slot in EEPROM ---
  }
  if (cached_state.request_flags.save_mesh) {
#if ENABLED(DEBUG_DGUSLCD)
    DEBUG_ECHOLNPGM("handle_request_flags: save mesh");
#endif

    const uint8_t fade_height_mm_tenth = static_cast<uint8_t>(cached_state.fade_height_slot_number.fade_height);
    const uint8_t fade_height_mm = static_cast<uint8_t>(fade_height_mm_tenth / 10U);
    const uint8_t fade_height_mm_fraction = static_cast<uint8_t>(fade_height_mm_tenth % 10U);
    char buf[20];

    sprintf_P(buf, PSTR("M420 Z%d.%d"), fade_height_mm, fade_height_mm_fraction);
    GCodeQueue::enqueue_one_now(buf);

    sprintf_P(buf, PSTR("G29 S%d"), cached_state.fade_height_slot_number.slot_number);
    GCodeQueue::enqueue_one_now(buf);

    cached_state.request_flags.save_mesh = 0;
  }

  // --- z fade-height changed ---

  if (cached_state.request_flags.fade_height_changed) {
#if ENABLED(DEBUG_DGUSLCD)
    DEBUG_ECHOLNPGM("handle_request_flags: update Z fade-heidht");
#endif
    const uint8_t fade_height_mm_tenth = static_cast<uint8_t>(cached_state.fade_height_slot_number.fade_height);
    const uint8_t fade_height_mm = static_cast<uint8_t>(fade_height_mm_tenth / 10U);
    const uint8_t fade_height_mm_fraction = static_cast<uint8_t>(fade_height_mm_tenth % 10U);
    char buf[16];

    sprintf_P(buf, PSTR("M420 Z%d.%d"), fade_height_mm, fade_height_mm_fraction);
    GCodeQueue::enqueue_one_now(buf);
    cached_state.request_flags.fade_height_changed = 0;
  }

  // --- slot selection changed ---

  if (cached_state.request_flags.slot_changed) {
#if ENABLED(DEBUG_DGUSLCD)
    DEBUG_ECHOLNPGM("handle_request_flags: slot selection changed");
#endif
    cached_state.request_flags.slot_changed = 0;
    //cached_state.request_flags.load_mesh = 1;
    handle_request_flags();
  }
}

} // namespace

void handle_parameter_load_save_probe(DGUS_VP_Variable &var, void *val_ptr) {
#if ENABLED(DEBUG_DGUSLCD)
  DEBUG_ECHOLNPGM("handle_parameter_load_save_probe");
#endif
  *static_cast<uint16_t *>(var.memadr) = dgus::swap16(*static_cast<uint16_t *>(val_ptr));
  handle_request_flags();
}

void handle_parameter_fade_slot(DGUS_VP_Variable &var, void *val_ptr) {
#if ENABLED(DEBUG_DGUSLCD)
  DEBUG_ECHOLNPGM("handle_parameter_fade_slot");
#endif
  auto *cached{static_cast<CachedState::FadeHeightSlotNumber *>(var.memadr)};
  auto *request{static_cast<CachedState::FadeHeightSlotNumber *>(val_ptr)};
  request->data = dgus::swap16(request->data);

  if (cached->slot_number != request->slot_number)
    cached_state.request_flags.slot_changed = 1;

  if (cached->fade_height != request->fade_height)
    cached_state.request_flags.fade_height_changed = 1;

  cached->data = request->data;

  handle_request_flags();
}

} // namespace bed_leveling_ubl
} // namespace dgus_origin

#endif
