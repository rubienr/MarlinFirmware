/**
 * Marlin 3D Printer Firmware
 * Copyright (c) 2020 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (c) 2011 Camiel Gubbels / Erik van der Zalm
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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/* DGUS implementation written by coldtobi in 2019 for Marlin */

#include "../../../../inc/MarlinConfig.h"

#if HAS_DGUS_LCD

#if HOTENDS > 2
#warning "More than 2 hotends not implemented on the Display UI design."
#endif

#include "../../ui_api.h"

#include "../../../../module/temperature.h"
#include "../../../../module/motion.h"
#include "../../../../gcode/queue.h"
#include "../../../../module/planner.h"
#if ENABLED(SDSUPPORT)
#include "../../../../sd/cardreader.h"
#endif
#include "../../../../libs/duration_t.h"
#include "../../../../module/printcounter.h"
#if ENABLED(POWER_LOSS_RECOVERY)
#include "../../../../feature/powerloss.h"
#endif

#include "DGUSDisplay.h"
//#include "DGUSVPVariable.h"
#include "DGUSDisplayDef.h"

namespace dgus {

// Preamble... 2 Bytes, usually 0x5A 0xA5, but configurable
constexpr uint8_t DGUS_HEADER1 = 0x5A;
constexpr uint8_t DGUS_HEADER2 = 0xA5;

constexpr uint8_t DGUS_CMD_WRITEVAR = 0x82;
constexpr uint8_t DGUS_CMD_READVAR = 0x83;

//#if ENABLED(DEBUG_DGUSLCD)
//  bool dguslcd_local_debug; // = false;
//#endif

#if ENABLED(DGUS_FILAMENT_LOADUNLOAD)
typedef struct {
  ExtUI::extruder_t extruder; // which extruder to operate
  uint8_t action;             // load or unload
  bool heated;                // heating done ?
  float purge_length;         // the length to extrude before unload, prevent filament jam
} filament_data_t;
static filament_data_t filament_data;
#endif

uint16_t DGUSScreenVariableHandler::ConfirmVP;

#if ENABLED(SDSUPPORT)
int16_t DGUSScreenVariableHandler::top_file = 0;
int16_t DGUSScreenVariableHandler::file_to_print = 0;
static ExtUI::FileList filelist;
#endif

void (*DGUSScreenVariableHandler::confirm_action_cb)() = nullptr;

// DGUSScreenVariableHandler ScreenHandler;

DGUSLCD_Screens DGUSScreenVariableHandler::current_screen;
DGUSLCD_Screens DGUSScreenVariableHandler::past_screens[NUM_PAST_SCREENS];
uint8_t DGUSScreenVariableHandler::update_ptr;
uint16_t DGUSScreenVariableHandler::skipVP;
bool DGUSScreenVariableHandler::ScreenComplete;

// DGUSDisplay dgusdisplay;

rx_datagram_state_t DGUSDisplay::rx_datagram_state = DGUS_IDLE;
uint8_t DGUSDisplay::rx_datagram_len = 0;
bool DGUSDisplay::Initialized = false;
bool DGUSDisplay::no_reentrance = false;

#define dgusserial DGUS_SERIAL

uint16_t swap16(const uint16_t value) {
  // clang-format off
  return
    ((value & 0x00ffU) << 8U)|
    ((value & 0xff00U) >> 8U);
  // clang-format on
}

uint32_t swap32(const uint32_t value) {
  // clang-format off
  return
    ((value & 0xff000000) >> 24U) |
    ((value & 0x00ff0000) >>  8U) |
    ((value & 0x0000ff00) <<  8U) |
    ((value & 0x000000ff) << 24U) ;
  // clang-format on
}

namespace handler {

void assign_float_from_int_word(DGUS_VP_Variable &var, void *val_ptr) {
  DgusFloat_t *int_data{static_cast<DgusFloat_t *>(val_ptr)};
  DgusFloat_t *memadr{static_cast<DgusFloat_t *>(var.memadr)};
  memadr->data_16 = dgus::swap16(int_data->data_16);
  memadr->as_float = memadr->as_int_16;
}

void assign_float_from_uint_word(DGUS_VP_Variable &var, void *val_ptr) {
  DgusFloat_t *int_data{static_cast<DgusFloat_t *>(val_ptr)};
  DgusFloat_t *memadr{static_cast<DgusFloat_t *>(var.memadr)};
  memadr->data_16 = dgus::swap16(int_data->data_16);
  memadr->as_float = memadr->as_uint_16;
}

void assign_int_from_int_word(DGUS_VP_Variable &var, void *val_ptr) {
  DgusWord_t *int_data{static_cast<DgusWord_t *>(val_ptr)};
  DgusWord_t *memadr{static_cast<DgusWord_t *>(var.memadr)};
  memadr->data = dgus::swap16(int_data->data);
  memadr->as_int = static_cast<int16_t>(memadr->as_int);
}

void assign_int_from_uint_word(DGUS_VP_Variable &var, void *val_ptr) {
  DgusWord_t *int_data{static_cast<DgusWord_t *>(val_ptr)};
  DgusWord_t *memadr{static_cast<DgusWord_t *>(var.memadr)};
  memadr->data = dgus::swap16(int_data->data);
  memadr->as_int = static_cast<uint16_t>(memadr->as_uint);
}

void assign_uint_from_int_word(DGUS_VP_Variable &var, void *val_ptr) {
  DgusWord_t *int_data{static_cast<DgusWord_t *>(val_ptr)};
  DgusWord_t *memadr{static_cast<DgusWord_t *>(var.memadr)};
  memadr->data = dgus::swap16(int_data->data);
  memadr->as_uint = static_cast<uint16_t>(memadr->as_int);
}

void assign_uint_from_uint_word(DGUS_VP_Variable &var, void *val_ptr) {
  DgusWord_t *int_data{static_cast<DgusWord_t *>(val_ptr)};
  DgusWord_t *memadr{static_cast<DgusWord_t *>(var.memadr)};
  memadr->data = dgus::swap16(int_data->data);
  memadr->as_uint = memadr->as_uint;
}

void send_word(DGUS_VP_Variable &var) {
  DgusWord_t value{.data = *static_cast<uint16_t *>(var.memadr)};

  value.data = swap16(value.data);
  DGUSDisplay::WriteVariable(var.VP, value.data);
}

void send_word_from_int(DGUS_VP_Variable &var) {
  DgusWord_t value{.as_int = *static_cast<int16_t *>(var.memadr)};

  value.data = swap16(value.data);
  DGUSDisplay::WriteVariable(var.VP, value.data);
}

void send_word_from_uint(DGUS_VP_Variable &var) {
  DgusWord_t value{.as_uint = *static_cast<uint16_t *>(var.memadr)};

  value.data = swap16(value.data);
  DGUSDisplay::WriteVariable(var.VP, value.data);
}

void send_float_from_int(DGUS_VP_Variable &var) {
  DgusFloat_t value{.as_int_16 = *static_cast<int16_t *>(var.memadr)};
  value.as_float = value.as_int_16;

  value.data_32 = swap32(value.data_32);
  DGUSDisplay::WriteVariable(var.VP, &value.data_32, static_cast<uint8_t>(sizeof(float)));
}

void send_float_from_uint(DGUS_VP_Variable &var) {
  DgusFloat_t value{.as_uint_16 = *static_cast<uint16_t *>(var.memadr)};
  value.as_float = value.as_uint_16;

  value.data_32 = swap32(value.data_32);
  DGUSDisplay::WriteVariable(var.VP, &value.data_32, static_cast<uint8_t>(sizeof(float)));
}

void send_float_from_float(DGUS_VP_Variable &var) {
  DgusFloat_t value{.as_float = *static_cast<float *>(var.memadr)};

  value.data_32 = swap32(value.data_32);
  DGUSDisplay::WriteVariable(var.VP, &value.data_32, static_cast<uint8_t>(sizeof(float)));
}

void send_string(DGUS_VP_Variable &var) {
  char *tmp = (char *)var.memadr;
  DGUSDisplay::WriteVariable(var.VP, tmp, var.size, true);
}

void send_string_P(DGUS_VP_Variable &var) {
  char *tmp = (char *)var.memadr;
  DGUSDisplay::WriteVariablePGM(var.VP, tmp, var.size, true);
}

#if ENABLED(PRINTCOUNTER)
void send_print_time(DGUS_VP_Variable &var) {
  // duration_t elapsed = Stopwatch::duration();
  char buf[21]{0}; // "YYYy DDDd HHh MMm SSs"
  ExtUI::getTotalPrintTime_str(buf);
  DGUSDisplay::write_variable(to_address(dgus::memory_layout::PrintStats::PrintTime), buf, sizeof(buf), false);
}
#endif

} // namespace handler

using dgus::swap16;

bool populate_VPVar(const uint16_t VP, DGUS_VP_Variable *const ramcopy) {
  // DEBUG_ECHOPAIR("populate_VPVar ", VP);
  const DGUS_VP_Variable *pvp = DGUSLCD_FindVPVar(VP);
  // DEBUG_ECHOLNPAIR(" pvp ", (uint16_t )pvp);
  if (!pvp)
    return false;
  memcpy_P(ramcopy, pvp, sizeof(DGUS_VP_Variable));
  return true;
}

void DGUSScreenVariableHandler::sendinfoscreen(const char *line1,
                                               const char *line2,
                                               const char *line3,
                                               const char *line4,
                                               bool l1inflash,
                                               bool l2inflash,
                                               bool l3inflash,
                                               bool l4inflash) {
  DGUS_VP_Variable ramcopy{.VP = 0, .memadr = nullptr, .size = 0};
  if (populate_VPVar(to_address(dgus::memory_layout::UiMessages::Message1), &ramcopy)) {
    ramcopy.memadr = (void *)line1;
    l1inflash ? DGUSScreenVariableHandler::DGUSLCD_SendStringToDisplayPGM(ramcopy)
              : DGUSScreenVariableHandler::DGUSLCD_SendStringToDisplay(ramcopy);
  }
  if (populate_VPVar(to_address(dgus::memory_layout::UiMessages::Message2), &ramcopy)) {
    ramcopy.memadr = (void *)line2;
    l2inflash ? DGUSScreenVariableHandler::DGUSLCD_SendStringToDisplayPGM(ramcopy)
              : DGUSScreenVariableHandler::DGUSLCD_SendStringToDisplay(ramcopy);
  }
  if (populate_VPVar(to_address(dgus::memory_layout::UiMessages::Message3), &ramcopy)) {
    ramcopy.memadr = (void *)line3;
    l3inflash ? DGUSScreenVariableHandler::DGUSLCD_SendStringToDisplayPGM(ramcopy)
              : DGUSScreenVariableHandler::DGUSLCD_SendStringToDisplay(ramcopy);
  }
  if (populate_VPVar(to_address(dgus::memory_layout::UiMessages::Message4), &ramcopy)) {
    ramcopy.memadr = (void *)line4;
    l4inflash ? DGUSScreenVariableHandler::DGUSLCD_SendStringToDisplayPGM(ramcopy)
              : DGUSScreenVariableHandler::DGUSLCD_SendStringToDisplay(ramcopy);
  }
}

void DGUSScreenVariableHandler::HandleUserConfirmationPopUp(uint16_t VP,
                                                            const char *line1,
                                                            const char *line2,
                                                            const char *line3,
                                                            const char *line4,
                                                            bool l1,
                                                            bool l2,
                                                            bool l3,
                                                            bool l4) {
  if (current_screen == DGUSLCD_SCREEN_CONFIRM) {
    // Already showing a pop up, so we need to cancel that first.
    PopToOldScreen();
  }

  ConfirmVP = VP;
  sendinfoscreen(line1, line2, line3, line4, l1, l2, l3, l4);
  DGUSScreenVariableHandler::GotoScreen(DGUSLCD_SCREEN_CONFIRM);
}

void DGUSScreenVariableHandler::setstatusmessage(const char *msg) {
  DGUS_VP_Variable ramcopy{.VP = 0, .memadr = nullptr, .size = 0};
  if (populate_VPVar(to_address(dgus::memory_layout::LcdMessage::M117), &ramcopy)) {
    ramcopy.memadr = (void *)msg;
    DGUSLCD_SendStringToDisplay(ramcopy);
  }
}

void DGUSScreenVariableHandler::setstatusmessagePGM(PGM_P const msg) {
  DGUS_VP_Variable ramcopy{.VP = 0, .memadr = nullptr, .size = 0};
  if (populate_VPVar(to_address(dgus::memory_layout::LcdMessage::M117), &ramcopy)) {
    ramcopy.memadr = (void *)msg;
    DGUSLCD_SendStringToDisplayPGM(ramcopy);
  }
}

#if ENABLED(SPEAKER)
void DGUSScreenVariableHandler::playToneSpeaker(uint8_t start_section, uint8_t sections_count, uint8_t volume) {
  struct {
    uint8_t start_section;
    uint8_t sections_count;
    uint8_t volume;
    uint8_t playback_status;
  } audio_command{
      .start_section = start_section, .sections_count = sections_count, .volume = volume, .playback_status = 0x02};
  DGUSDisplay::write_variable(0xa0, &audio_command, 4);
}
#else
void DGUSScreenVariableHandler::playToneBuzzer(uint8_t time_times_8ms, uint8_t volume) {
  struct {
    uint8_t _pad;
    uint8_t buzz_time_times_8ms;
    uint8_t volume;
    uint8_t playback_status;
  } audio_command{._pad = 0, .buzz_time_times_8ms = time_times_8ms, .volume = volume, .playback_status = 0x02};
  DGUSDisplay::WriteVariable(0xa0, &audio_command, 4);
}
#endif

// Send an 8 bit or 16 bit value to the display.
void DGUSScreenVariableHandler::DGUSLCD_SendWordValueToDisplay(DGUS_VP_Variable &var) {
  if (var.memadr) {
    // DEBUG_ECHOPAIR(" DGUS_LCD_SendWordValueToDisplay ", var.VP);
    // DEBUG_ECHOLNPAIR(" data ", *(uint16_t *)var.memadr);
    auto *tmp = (uint8_t *)var.memadr;
    uint16_t data_to_send = (tmp[0] << 8U);
    if (var.size >= 1)
      data_to_send |= tmp[1];
    DGUSDisplay::write_variable(var.VP, data_to_send);
  }
}

// TODO rubienr - check
// Send an 8 bit or 16 bit value to the display.
void DGUSScreenVariableHandler::DGUSLCD_SendWordValueToDisplayPGM(DGUS_VP_Variable &var) {
  if (var.memadr) {
    auto *tmp = (uint8_t *)var.memadr;
    uint16_t data_to_send = (tmp[0] << 8U);
    if (var.size >= 1)
      data_to_send |= tmp[1];
    DGUSDisplay::write_variable_P(var.VP, &data_to_send, (var.size >= 1) ? 2 : 1);
  }
}

// Send an uint8_t between 0 and 255 to the display, but scale to a percentage (0..100)
void DGUSScreenVariableHandler::DGUSLCD_SendPercentageToDisplay(DGUS_VP_Variable &var) {
  if (var.memadr == nullptr)
    return;
  uint16_t tmp = *static_cast<uint8_t *>(var.memadr);
  tmp = static_cast<uint16_t>(map(tmp, 0, 0xff, 0, 100));
  uint16_t data_to_send = swap16(tmp);
  DGUSDisplay::write_variable(var.VP, data_to_send);
}

// Send the current print progress to the display.
void DGUSScreenVariableHandler::DGUSLCD_SendPrintProgressToDisplay(DGUS_VP_Variable &var) {
  // DEBUG_ECHOPAIR(" DGUSLCD_SendPrintProgressToDisplay ", var.VP);
  uint16_t tmp = ExtUI::getProgress_percent();
  // DEBUG_ECHOLNPAIR(" data ", tmp);
  uint16_t data_to_send = swap16(tmp);
  DGUSDisplay::write_variable(var.VP, data_to_send);
}

// Send the current print time to the display.
// It is using a hex display for that: It expects BSD coded data in the format xxyyzz
void DGUSScreenVariableHandler::DGUSLCD_SendPrintTimeToDisplay(DGUS_VP_Variable &var) {
  duration_t elapsed = Stopwatch::duration();
  char buf[32];
  elapsed.toString(buf);
  DGUSDisplay::write_variable(to_address(dgus::memory_layout::PrintStats::PrintTime), buf, var.size, true);
}

// Send an uint8_t between 0 and 100 to a variable scale to 0..255
void DGUSScreenVariableHandler::DGUSLCD_PercentageToUint8(DGUS_VP_Variable &var, void *val_ptr) {
  if (var.memadr) {
    uint16_t value = swap16(*static_cast<uint16_t *>(val_ptr));
    *static_cast<uint8_t *>(var.memadr) = static_cast<uint8_t>(map(constrain(value, 0, 100), 0, 100, 0, 255));
  }
}

// Sends a (RAM located) string to the DGUS Display
// (Note: The DGUS Display does not clear after the \0, you have to
// overwrite the remainings with spaces.// var.size has the display buffer size!
void DGUSScreenVariableHandler::DGUSLCD_SendStringToDisplay(DGUS_VP_Variable &var) {
  char *tmp = (char *)var.memadr;
  DGUSDisplay::write_variable(var.VP, tmp, var.size, true);
}

// Sends a (flash located) string to the DGUS Display
// (Note: The DGUS Display does not clear after the \0, you have to
// overwrite the remainings with spaces.// var.size has the display buffer size!
void DGUSScreenVariableHandler::DGUSLCD_SendStringToDisplayPGM(DGUS_VP_Variable &var) {
  char *tmp = (char *)var.memadr;
  DGUSDisplay::write_variable_P(var.VP, tmp, var.size, true);
}

#if HAS_PID_HEATING
void DGUSScreenVariableHandler::DGUSLCD_SendTemperaturePID(DGUS_VP_Variable &var) {
  float value = *(float *)var.memadr;
  union {
    float as_float;
    int16_t as_int;
    uint16_t as_uint;
  } valuesend{.as_float = 0};
  switch (var.VP) {
    default:
      return;
#if HOTENDS >= 1
    case to_address(dgus::memory_layout::Pid::E0P):
      valuesend.as_float = value;
      break;
    case to_address(dgus::memory_layout::Pid::E0I):
      valuesend.as_float = unscalePID_i(value);
      break;
    case to_address(dgus::memory_layout::Pid::E0D):
      valuesend.as_float = unscalePID_d(value);
      break;
#endif
#if HOTENDS >= 2
    case to_address(dgus::memory_layout::Pid::E1P):
      valuesend.as_float = value;
      break;
    case to_address(dgus::memory_layout::Pid::E1I):
      valuesend.as_float = unscalePID_i(value);
      break;
    case to_address(dgus::memory_layout::Pid::E1D):
      valuesend.as_float = unscalePID_d(value);
      break;
#endif
#if HAS_HEATED_BED
    case to_address(dgus::memory_layout::Pid::BedP):
      valuesend.as_float = value;
      break;
    case to_address(dgus::memory_layout::Pid::BedI):
      valuesend.as_float = unscalePID_i(value);
      break;
    case to_address(dgus::memory_layout::Pid::BedD):
      valuesend.as_float = unscalePID_d(value);
      break;
#endif
  }

  valuesend.as_float *= 10;
  valuesend.as_int = static_cast<uint16_t>(roundf(valuesend.as_float));
  valuesend.as_uint = swap16(valuesend.as_uint);

  DGUSDisplay::write_variable(var.VP, &valuesend.as_uint, 2);
}
#endif

#if ENABLED(PRINTCOUNTER)

// Send the accumulate print time to the display.
// It is using a hex display for that: It expects BSD coded data in the format xxyyzz
void DGUSScreenVariableHandler::DGUSLCD_SendPrintAccTimeToDisplay(DGUS_VP_Variable &var) {
  printStatistics state = print_job_timer.getStats();
  char buf[21];
  duration_t elapsed = state.printTime;
  elapsed.toString(buf);
  DGUSDisplay::write_variable(to_address(dgus::memory_layout::PrintStats::PrintAccTime), buf, var.size, true);
}

void DGUSScreenVariableHandler::DGUSLCD_SendPrintsTotalToDisplay(DGUS_VP_Variable &var) {
  printStatistics state = print_job_timer.getStats();
  char buf[21];
  sprintf_P(buf, PSTR("%u"), state.totalPrints);
  DGUSDisplay::write_variable(to_address(dgus::memory_layout::PrintStats::PrintsTotal), buf, var.size, true);
}

#endif

// Send fan status value to the display.
#if FAN_COUNT > 0
void DGUSScreenVariableHandler::DGUSLCD_SendFanStatusToDisplay(DGUS_VP_Variable &var) {
  if (var.memadr) {
    DEBUG_ECHOPAIR(" DGUSLCD_SendFanStatusToDisplay ", var.VP);
    DEBUG_ECHOLNPAIR(" data ", *(uint8_t *)var.memadr);
    uint16_t data_to_send = 0;
    if (*(uint8_t *)var.memadr)
      data_to_send = 1;
    data_to_send = swap16(data_to_send);
    DGUSDisplay::write_variable(var.VP, data_to_send);
  }
}
#endif

// Send heater status value to the display.
void DGUSScreenVariableHandler::DGUSLCD_SendHeaterStatusToDisplay(DGUS_VP_Variable &var) {
  if (var.memadr) {
    DEBUG_ECHOPAIR(" DGUSLCD_SendHeaterStatusToDisplay ", var.VP);
    DEBUG_ECHOLNPAIR(" data ", *(int16_t *)var.memadr);
    uint16_t data_to_send = 0;
    if (*(int16_t *)var.memadr)
      data_to_send = 1;
    data_to_send = swap16(data_to_send);
    DGUSDisplay::write_variable(var.VP, data_to_send);
  }
}

#if ENABLED(DGUS_UI_WAITING)
void DGUSScreenVariableHandler::DGUSLCD_SendWaitingStatusToDisplay(DGUS_VP_Variable &var) {
  // In FYSETC UI design there are 10 statuses to loop
  static uint16_t period = 0;
  static uint16_t index = 0;
  // DEBUG_ECHOPAIR(" DGUSLCD_SendWaitingStatusToDisplay ", var.VP);
  // DEBUG_ECHOLNPAIR(" data ", swap16(index));
  if (period++ > DGUS_UI_WAITING_STATUS_PERIOD) {
    DGUSDisplay::WriteVariable(var.VP, swap16(index));
    // DEBUG_ECHOLNPAIR(" data ", swap16(index));
    if (++index >= DGUS_UI_WAITING_STATUS)
      index = 0;
    period = 0;
  }
}
#endif

#if ENABLED(SDSUPPORT)

void DGUSScreenVariableHandler::ScreenChangeHookIfSD(DGUS_VP_Variable &var, void *val_ptr) {
  // default action executed when there is a SD card, but not printing
  if (ExtUI::isMediaInserted() && !ExtUI::isPrintingFromMedia()) {
    ScreenChangeHook(var, val_ptr);
    dgusdisplay.RequestScreen(current_screen);
    return;
  }

  // if we are printing, we jump to two screens after the requested one.
  // This should host e.g a print pause / print abort / print resume dialog.
  // This concept allows to recycle this hook for other file
  if (ExtUI::isPrintingFromMedia() && !card.flag.abort_sd_printing) {
    GotoScreen(DGUSLCD_SCREEN_SDPRINTMANIPULATION);
    return;
  }

  // Don't let the user in the dark why there is no reaction.
  if (!ExtUI::isMediaInserted()) {
    setstatusmessagePGM(GET_TEXT(MSG_NO_MEDIA));
    return;
  }
  if (card.flag.abort_sd_printing) {
    setstatusmessagePGM(GET_TEXT(MSG_MEDIA_ABORTING));
    return;
  }
}

void DGUSScreenVariableHandler::DGUSLCD_SD_ScrollFilelist(DGUS_VP_Variable &var, void *val_ptr) {
  auto old_top = top_file;
  const int16_t scroll = (int16_t)swap16(*(uint16_t *)val_ptr);
  if (scroll) {
    top_file += scroll;
    DEBUG_ECHOPAIR("new topfile calculated:", top_file);
    if (top_file < 0) {
      top_file = 0;
      DEBUG_ECHOLNPGM("Top of filelist reached");
    } else {
      int16_t max_top = filelist.count() - to_uint16_t(dgus::memory_layout::SdFileListing::FilesPerScreen);
      NOLESS(max_top, 0);
      NOMORE(top_file, max_top);
    }
    DEBUG_ECHOPAIR("new topfile adjusted:", top_file);
  } else if (!filelist.isAtRootDir()) {
    filelist.upDir();
    top_file = 0;
    ForceCompleteUpdate();
  }

  if (old_top != top_file)
    ForceCompleteUpdate();
}

void DGUSScreenVariableHandler::DGUSLCD_SD_FileSelected(DGUS_VP_Variable &var, void *val_ptr) {
  uint16_t touched_nr = (int16_t)swap16(*(uint16_t *)val_ptr) + top_file;
  if (touched_nr > filelist.count())
    return;
  if (!filelist.seek(touched_nr))
    return;
  if (filelist.isDir()) {
    filelist.changeDir(filelist.filename());
    top_file = 0;
    ForceCompleteUpdate();
    return;
  }

#if ENABLED(DGUS_PRINT_FILENAME)
  // Send print filename
  DGUSDisplay::WriteVariable(VP_SD_Print_Filename, filelist.filename(), VP_SD_FileName_LEN, true);
#endif

  // Setup Confirmation screen
  file_to_print = touched_nr;
  HandleUserConfirmationPopUp(to_address(dgus::memory_layout::SdAction::FileSelectConfirm),
                              nullptr,
                              PSTR("Print file"),
                              filelist.filename(),
                              PSTR("from SD Card?"),
                              true,
                              true,
                              false,
                              true);
}

void DGUSScreenVariableHandler::DGUSLCD_SD_StartPrint(DGUS_VP_Variable &var, void *val_ptr) {
  if (!filelist.seek(file_to_print))
    return;
  ExtUI::printFile(filelist.shortFilename());
  ScreenHandler.GotoScreen(
#if ENABLED(DGUS_LCD_UI_ORIGIN)
      DGUSLCD_SCREEN_STATUS
#else
      DGUSLCD_SCREEN_SDPRINTMANIPULATION
#endif
  );
}

void DGUSScreenVariableHandler::DGUSLCD_SD_ResumePauseAbort(DGUS_VP_Variable &var, void *val_ptr) {
  if (!ExtUI::isPrintingFromMedia())
    return; // avoid race condition when user stays in this menu and printer finishes.
  switch (swap16(*(uint16_t *)val_ptr)) {
    case 0: // Resume
      if (ExtUI::isPrintingFromMediaPaused())
        ExtUI::resumePrint();
      break;
    case 1: // Pause
      if (!ExtUI::isPrintingFromMediaPaused())
        ExtUI::pausePrint();
      break;
    case 2: // Abort
      ScreenHandler.HandleUserConfirmationPopUp(to_address(dgus::memory_layout::SdAction::AbortPrintConfirmed),
                                                nullptr,
                                                PSTR("Abort printing"),
                                                filelist.filename(),
                                                PSTR("?"),
                                                true,
                                                true,
                                                false,
                                                true);
      break;
  }
}

void DGUSScreenVariableHandler::DGUSLCD_SD_ReallyAbort(DGUS_VP_Variable &var, void *val_ptr) {
  ExtUI::stopPrint();
  GotoScreen(DGUSLCD_SCREEN_MAIN);
}

void DGUSScreenVariableHandler::DGUSLCD_SD_PrintTune(DGUS_VP_Variable &var, void *val_ptr) {
  if (!ExtUI::isPrintingFromMedia())
    return; // avoid race condition when user stays in this menu and printer finishes.
  GotoScreen(DGUSLCD_SCREEN_SDPRINTTUNE);
}

void DGUSScreenVariableHandler::DGUSLCD_SD_SendFilename(DGUS_VP_Variable &var) {
  constexpr uint16_t filename_length{to_uint16_t(dgus::memory_layout::SdFileListing::FileNameBytes)};
  constexpr uint16_t files_per_screen{to_uint16_t(dgus::memory_layout::SdFileListing::FilesPerScreen)};

  uint16_t target_line = (var.VP - to_address(dgus::memory_layout::SdFileListing::FileName0)) / filename_length;
  if (target_line > files_per_screen)
    return;
  char tmpfilename[filename_length + 1] = "";
  var.memadr = (void *)tmpfilename;
  if (filelist.seek(top_file + target_line))
    snprintf_P(tmpfilename, filename_length, PSTR("%s%c"), filelist.filename(), filelist.isDir() ? '/' : 0);
  DGUSLCD_SendStringToDisplay(var);
}

void DGUSScreenVariableHandler::SDCardInserted() {
  top_file = 0;
  auto cs = ScreenHandler.getCurrentScreen();
  if (cs == DGUSLCD_SCREEN_MAIN || cs == DGUSLCD_SCREEN_STATUS)
    ScreenHandler.GotoScreen(DGUSLCD_SCREEN_SDFILELIST);
}

void DGUSScreenVariableHandler::SDCardRemoved() {
  if (current_screen == DGUSLCD_SCREEN_SDFILELIST ||
      (current_screen == DGUSLCD_SCREEN_CONFIRM &&
       (ConfirmVP == to_address(dgus::memory_layout::SdAction::AbortPrintConfirmed) ||
        ConfirmVP == to_address(dgus::memory_layout::SdAction::FileSelectConfirm))) ||
      current_screen == DGUSLCD_SCREEN_SDPRINTMANIPULATION)
    ScreenHandler.GotoScreen(DGUSLCD_SCREEN_MAIN);
}

void DGUSScreenVariableHandler::SDCardError() {
  DGUSScreenVariableHandler::SDCardRemoved();
  ScreenHandler.sendinfoscreen(PSTR("NOTICE"), nullptr, PSTR("SD card error"), nullptr, true, true, true, true);
  ScreenHandler.SetupConfirmAction(nullptr);
  ScreenHandler.GotoScreen(DGUSLCD_SCREEN_POPUP);
}

#endif // SDSUPPORT

void DGUSScreenVariableHandler::ScreenConfirmedOK(DGUS_VP_Variable &var, void *val_ptr) {
  DGUS_VP_Variable ramcopy{.VP = 0, .memadr = nullptr, .size = 0};
  if (!populate_VPVar(ConfirmVP, &ramcopy))
    return;
  if (ramcopy.set_by_display_handler)
    ramcopy.set_by_display_handler(ramcopy, val_ptr);
}

const uint16_t *DGUSLCD_FindScreenVPMapList(uint8_t screen) {
  const struct VPMapping *map{VPMap};
  const uint16_t *ret{static_cast<const uint16_t *>(pgm_read_ptr(&(map->VPList)))};
  while (ret) {
    if (pgm_read_byte(&(map->screen)) == screen)
      return ret;
    map++;
    ret = static_cast<const uint16_t *>(pgm_read_ptr(&(map->VPList)));
  }
  return nullptr;
}

const DGUS_VP_Variable *DGUSLCD_FindVPVar(const uint16_t vp) {
  const DGUS_VP_Variable *ret = ListOfVP;
  do {
    const uint16_t vpcheck = pgm_read_word(&(ret->VP));
    if (vpcheck == 0)
      break;
    if (vpcheck == vp)
      return ret;
    ++ret;
  } while (true);

  DEBUG_ECHOLNPAIR("FindVPVar NOT FOUND ", vp);
  return nullptr;
}

void DGUSScreenVariableHandler::ScreenChangeHookIfIdle(DGUS_VP_Variable &var, void *val_ptr) {
  if (!ExtUI::isPrinting()) {
    ScreenChangeHook(var, val_ptr);
  }
}

void DGUSScreenVariableHandler::ScreenChangeHook(DGUS_VP_Variable &var, void *val_ptr) {
  auto *tmp = (uint8_t *)val_ptr;

  // The keycode in target is coded as <from-frame><to-frame>, so 0x0100A means
  // from screen 1 (main) to 10 (temperature). DGUSLCD_SCREEN_POPUP is special,
  // meaning "return to previous screen"
  auto target = (DGUSLCD_Screens)tmp[1];

  if (target == DGUSLCD_SCREEN_POPUP) {
    // special handling for popup is to return to previous menu
    if (current_screen == DGUSLCD_SCREEN_POPUP && confirm_action_cb)
      confirm_action_cb();
    PopToOldScreen();
    return;
  }

  UpdateNewScreen(target);
  DGUSDisplay::RequestScreen(current_screen);

#ifdef DEBUG_DGUSLCD
  if (!DGUSLCD_FindScreenVPMapList(target))
    DEBUG_ECHOLNPAIR("WARNING: No screen Mapping found for ", target);
#endif
}

void DGUSScreenVariableHandler::HandleAllHeatersOff(DGUS_VP_Variable &var, void *val_ptr) {
  UNUSED(val_ptr);
  Temperature::disable_all_heaters();
  DGUSScreenVariableHandler::ForceCompleteUpdate(); // hint to send all data.
}

/*
void DGUSScreenVariableHandler::HandleTemperatureChanged(DGUS_VP_Variable &var, void *val_ptr) {
  uint16_t newvalue = swap16(*(uint16_t *)val_ptr);
  uint16_t acceptedvalue;

  switch (var.VP) {
    default:
      return;
#if HOTENDS >= 1
    case to_address(dgus::memory_layout::Temperatures::E0Set):
      Temperature::setTargetHotend(newvalue, 0);
      acceptedvalue = static_cast<uint16_t>(Temperature::temp_hotend[0].target);
      break;
#endif
#if HOTENDS >= 2
    case VP_T_E1_Set:
      thermalManager.setTargetHotend(newvalue, 1);
      acceptedvalue = thermalManager.temp_hotend[1].target;
      break;
#endif
#if HAS_HEATED_BED
    case to_address(dgus::memory_layout::Temperatures::BedSet):
      Temperature::setTargetBed(newvalue);
      acceptedvalue = static_cast<uint16_t>(Temperature::temp_bed.target);
      break;
#endif
  }

  // reply to display the new value to update the view if the new value was rejected by the Thermal Manager.
  if (newvalue != acceptedvalue && var.send_to_display_handler)
    var.send_to_display_handler(var);
  // don't overwrite value the next update time as the display might autoincrement in parallel
  DGUSScreenVariableHandler::skipVP = var.VP;
}
*/

/*
void DGUSScreenVariableHandler::HandleFlowRateChanged(DGUS_VP_Variable &var, void *val_ptr) {
#if EXTRUDERS
  uint16_t newvalue = swap16(*(uint16_t *)val_ptr);
  uint8_t target_extruder;
  switch (var.VP) {
    default:
      return;
#if HOTENDS >= 1
    case to_address(dgus::memory_layout::FlowRates::E0):
      target_extruder = 0;
      break;
#endif
#if HOTENDS >= 2
    case VP_Flowrate_E1:
      target_extruder = 1;
      break;
#endif
  }

  Planner::flow_percentage[target_extruder] = newvalue;
  Planner::refresh_e_factor(target_extruder);
  // don't overwrite value the next update time as the display might autoincrement in parallel
  DGUSScreenVariableHandler::skipVP = var.VP;
#else
  UNUSED(var);
  UNUSED(val_ptr);
#endif
}
*/

void DGUSScreenVariableHandler::HandleManualExtrude(DGUS_VP_Variable &var, void *val_ptr) {
  DEBUG_ECHOLNPGM("HandleManualExtrude");

  int16_t movevalue = swap16(*(uint16_t *)val_ptr);
  float target = movevalue * 0.01f;
  ExtUI::extruder_t target_extruder;

  switch (var.VP) {
#if HOTENDS >= 1
    case to_uint8_t(dgus::memory_layout::MoveE::E0):
      target_extruder = ExtUI::extruder_t::E0;
      break;
#endif
#if HOTENDS >= 2
    case to_uint8_t(dgus::memory_layout::MoveE::E1):
      target_extruder = ExtUI::extruder_t::E1;
      break;
#endif
          default : return;
  }

  target += ExtUI::getAxisPosition_mm(target_extruder);
  ExtUI::setAxisPosition_mm(target, target_extruder);
  skipVP = var.VP;
}

#if ENABLED(DGUS_UI_MOVE_DIS_OPTION)
void DGUSScreenVariableHandler::HandleManualMoveOption(DGUS_VP_Variable &var, void *val_ptr) {
  DEBUG_ECHOLNPGM("HandleManualMoveOption");
  *(uint16_t *)var.memadr = swap16(*(uint16_t *)val_ptr);
}
#endif

// TODO rubienr
/*
void DGUSScreenVariableHandler::HandleManualMove(DGUS_VP_Variable &var, void *val_ptr) {
  DEBUG_ECHOLNPGM("HandleManualMove");

  int16_t movevalue = swap16(*(uint16_t *)val_ptr);
#if ENABLED(DGUS_UI_MOVE_DIS_OPTION)
  const uint16_t choice = *(uint16_t *)var.memadr;
  movevalue = movevalue > 0 ? choice : -choice;
#endif
  char axiscode;
  unsigned int speed = 1500; // FIXME: get default feedrate for manual moves, dont hardcode.

  switch (var.VP) {
    default:
      return;

    case to_address(dgus::memory_layout::MoveAxis::X):
      axiscode = 'X';
      if (!ExtUI::canMove(ExtUI::axis_t::X))
        goto cannotmove;
      break;

    case to_address(dgus::memory_layout::MoveAxis::Y):
      axiscode = 'Y';
      if (!ExtUI::canMove(ExtUI::axis_t::Y))
        goto cannotmove;
      break;

    case to_address(dgus::memory_layout::MoveAxis::Z):
      axiscode = 'Z';
      speed = 300; // default to 5mm/s
      if (!ExtUI::canMove(ExtUI::axis_t::Z))
        goto cannotmove;
      break;

    case to_address(dgus::memory_layout::Homing::Control): // only used for homing
      axiscode = '\0';
      movevalue = 0; // ignore value sent from display, this VP is _ONLY_ for homing.
      break;
  }

  if (!movevalue) {
    // homing
    DEBUG_ECHOPAIR(" homing ", axiscode);
    char buf[6] = "G28 X";
    buf[4] = axiscode;
    // DEBUG_ECHOPAIR(" ", buf);
    GCodeQueue::enqueue_one_now(buf);
    // DEBUG_ECHOLNPGM(" ✓");
    DGUSScreenVariableHandler::ForceCompleteUpdate();
    return;
  } else {
    // movement
    DEBUG_ECHOPAIR(" move ", axiscode);
    bool old_relative_mode = relative_mode;
    if (!relative_mode) {
      // DEBUG_ECHOPGM(" G91");
      GCodeQueue::enqueue_now_P(PSTR("G91"));
      // DEBUG_ECHOPGM(" ✓ ");
    }
    char buf[32]; // G1 X9999.99 F12345
    auto backup_speed = static_cast<unsigned int>(roundf(MMS_TO_MMM(feedrate_mm_s)));
    char sign[] = "\0";
    int16_t value = movevalue / 100;
    if (movevalue < 0) {
      value = -value;
      sign[0] = '-';
    }
    int16_t fraction = ABS(movevalue) % 100;
    snprintf_P(buf, 32, PSTR("G0 %c%s%d.%02d F%d"), axiscode, sign, value, fraction, speed);
    // DEBUG_ECHOPAIR(" ", buf);
    GCodeQueue::enqueue_one_now(buf);
    // DEBUG_ECHOLNPGM(" ✓ ");
    if (backup_speed != speed) {
      snprintf_P(buf, 32, PSTR("G0 F%d"), backup_speed);
      GCodeQueue::enqueue_one_now(buf);
      // DEBUG_ECHOPAIR(" ", buf);
    }
    // while (!enqueue_and_echo_command(buf)) idle();
    // DEBUG_ECHOLNPGM(" ✓ ");
    if (!old_relative_mode) {
      // DEBUG_ECHOPGM("G90");
      GCodeQueue::enqueue_now_P(PSTR("G90"));
      // DEBUG_ECHOPGM(" ✓ ");
    }
  }

  DGUSScreenVariableHandler::ForceCompleteUpdate();
  DEBUG_ECHOLNPGM("manmv done.");
  return;

cannotmove:
  DEBUG_ECHOLNPAIR(" cannot move ", axiscode);
}
*/

/**
 * Enable or disable motors' driver.
 * @param var
 * @param val_ptr low byte: 0 - undefined, 1 - disable motors, 2 - enable motors; high byte: unused
 */
void DGUSScreenVariableHandler::HandleMotorLockUnlock(DGUS_VP_Variable &var, void *val_ptr) {
  // TODO rubienr - revert
  DEBUG_ECHOLNPGM("HandleMotorLockUnlock");

  const uint16_t lock = swap16(*(uint16_t *)val_ptr) & 0x00ffU;

  switch (lock) {
    default:
    case 0:
      return; // value unset
    case 1:   // disabe motors
      GCodeQueue::enqueue_now_P(PSTR("M18"));
      break;
    case 2: // enable motors
      GCodeQueue::enqueue_now_P(PSTR("M17"));
      break;
  }

  // TODO rubienr: save the current motor state to high byte for visualization on screen
  if (var.memadr)
    *(uint16_t *)var.memadr = 0;
}

#if ENABLED(POWER_LOSS_RECOVERY)

void DGUSScreenVariableHandler::HandlePowerLossRecovery(DGUS_VP_Variable &var, void *val_ptr) {
  uint16_t value = swap16(*(uint16_t *)val_ptr);
  if (value) {
    queue.inject_P(PSTR("M1000"));
    ScreenHandler.GotoScreen(DGUSLCD_SCREEN_SDPRINTMANIPULATION);
  } else {
    recovery.cancel();
    ScreenHandler.GotoScreen(DGUSLCD_SCREEN_STATUS);
  }
}

#endif
#if ENABLED(EEPROM_SETTINGS)
void DGUSScreenVariableHandler::HandleSettings(DGUS_VP_Variable &var, void *val_ptr) {
  DEBUG_ECHOLNPGM("HandleSettings");
  const uint16_t value = swap16(*(uint16_t *)val_ptr);

  constexpr uint16_t restore_to_factory_settings_flag{0x1};
  constexpr uint16_t load_from_eeprom_flag{0x2};
  constexpr uint16_t save_to_eeprom_flag{0x4};

  switch (value) {
    default:
      break;
    case restore_to_factory_settings_flag:
#if ENABLED(PRINTCOUNTER)
      print_job_timer.initStats();
#endif
      GCodeQueue::enqueue_now_P(PSTR("M502\nM500"));
      break;
    case load_from_eeprom_flag:
      GCodeQueue::enqueue_now_P(PSTR("M501"));
      break;
    case save_to_eeprom_flag:
      GCodeQueue::enqueue_now_P(PSTR("M500"));
      break;
  }
}
#endif // EEPROM_SETTINGS

void DGUSScreenVariableHandler::HandleStepPerMMChanged(DGUS_VP_Variable &var, void *val_ptr) {
  DEBUG_ECHOLNPGM("HandleStepPerMMChanged");

  uint16_t value_raw = swap16(*(uint16_t *)val_ptr);
  DEBUG_ECHOLNPAIR("value_raw:", value_raw);
  float value = (float)value_raw / 10;
  ExtUI::axis_t axis;
  switch (var.VP) {
    case to_address(dgus::memory_layout::StepsPerMm::X):
      axis = ExtUI::axis_t::X;
      break;
    case to_address(dgus::memory_layout::StepsPerMm::Y):
      axis = ExtUI::axis_t::Y;
      break;
    case to_address(dgus::memory_layout::StepsPerMm::Z):
      axis = ExtUI::axis_t::Z;
      break;
    default:
      return;
  }
  DEBUG_ECHOLNPAIR_F("value:", value);
  ExtUI::setAxisSteps_per_mm(value, axis);
  DEBUG_ECHOLNPAIR_F("value_set:", ExtUI::getAxisSteps_per_mm(axis));
  // don't overwrite value the next update time as the display might autoincrement in parallel
  DGUSScreenVariableHandler::skipVP = var.VP;
}

void DGUSScreenVariableHandler::HandleStepPerMMExtruderChanged(DGUS_VP_Variable &var, void *val_ptr) {
  DEBUG_ECHOLNPGM("HandleStepPerMMExtruderChanged");

  uint16_t value_raw = swap16(*(uint16_t *)val_ptr);
  DEBUG_ECHOLNPAIR("value_raw:", value_raw);
  float value = (float)value_raw / 10;
  ExtUI::extruder_t extruder;
  switch (var.VP) {
    default:
      return;
#if HOTENDS >= 1
    case to_address(dgus::memory_layout::StepsPerMm::E0):
      extruder = ExtUI::extruder_t::E0;
      break;
#endif
#if HOTENDS >= 2
    case to_address(dgus::memory_layout::StepsPerMm::E1):
      extruder = ExtUI::extruder_t::E1;
      break;
#endif
  }
  DEBUG_ECHOLNPAIR_F("value:", value);
  ExtUI::setAxisSteps_per_mm(value, extruder);
  DEBUG_ECHOLNPAIR_F("value_set:", ExtUI::getAxisSteps_per_mm(extruder));
  // don't overwrite value the next update time as the display might autoincrement in parallel
  DGUSScreenVariableHandler::skipVP = var.VP;
}

#if HAS_PID_HEATING
void DGUSScreenVariableHandler::HandleTemperaturePIDChanged(DGUS_VP_Variable &var, void *val_ptr) {
  uint16_t rawvalue = swap16(*(uint16_t *)val_ptr);
  DEBUG_ECHOLNPAIR("V1:", rawvalue);
  float value = (float)rawvalue / 10;
  DEBUG_ECHOLNPAIR("V2:", value);
  float newvalue = 0;

  switch (var.VP) {
    default:
      return;
#if HOTENDS >= 1
    case to_address(dgus::memory_layout::Pid::E0P):
      newvalue = value;
      break;
    case to_address(dgus::memory_layout::Pid::E0I):
      newvalue = scalePID_i(value);
      break;
    case to_address(dgus::memory_layout::Pid::E0D):
      newvalue = scalePID_d(value);
      break;
#endif
#if HOTENDS >= 2
    case to_address(dgus::memory_layout::Pid::E1P):
      newvalue = value;
      break;
    case to_address(dgus::memory_layout::Pid::E1I):
      newvalue = scalePID_i(value);
      break;
    case to_address(dgus::memory_layout::Pid::E1D):
      newvalue = scalePID_d(value);
      break;
#endif
#if HAS_HEATED_BED
    case to_address(dgus::memory_layout::Pid::BedP):
      newvalue = value;
      break;
    case to_address(dgus::memory_layout::Pid::BedI):
      newvalue = scalePID_i(value);
      break;
    case to_address(dgus::memory_layout::Pid::BedD):
      newvalue = scalePID_d(value);
      break;
#endif
  }

  DEBUG_ECHOLNPAIR_F("V3:", newvalue);
  *(float *)var.memadr = newvalue;
  // don't overwrite value the next update time as the display might autoincrement in parallel
  DGUSScreenVariableHandler::skipVP = var.VP;
}

void DGUSScreenVariableHandler::HandlePIDAutotune(DGUS_VP_Variable &var, void *val_ptr) {
  DEBUG_ECHOLNPGM("HandlePIDAutotune");
  UNUSED(val_ptr);
  char buf[32] = {0};

  switch (var.VP) {
    default:
      break;
#if ENABLED(PIDTEMP)
#if HOTENDS >= 1
    case to_address(dgus::memory_layout::PidAutotune::E0): // Autotune Extruder 0
      sprintf(buf, "M303 E%d C5 S210 U1", ExtUI::extruder_t::E0);
      break;
#endif
#if HOTENDS >= 2
    case to_address(dgus::memory_layout::PidAutotune::E1):
      sprintf(buf, "M303 E%d C5 S210 U1", ExtUI::extruder_t::E1);
      break;
#endif
#endif
// TODO rubienr
#if ENABLED(PIDTEMPBED_XXX)
    case VP_PID_AUTOTUNE_BED:
      sprintf(buf, "M303 E-1 C5 S70 U1");
      break;
#endif
  }

  if (buf[0])
    GCodeQueue::enqueue_one_now(buf);

#if ENABLED(DGUS_UI_WAITING)
  sendinfoscreen(PSTR("PID is autotuning"), PSTR("please wait"), NUL_STR, NUL_STR, true, true, true, true);
  GotoScreen(DGUSLCD_SCREEN_WAITING);
#endif
}
#endif

#if ENABLED(HAS_BED_PROBE)
void DGUSScreenVariableHandler::HandleProbeOffsetZChanged(DGUS_VP_Variable &var, void *val_ptr) {
  DEBUG_ECHOLNPGM("HandleProbeOffsetZChanged");
  union {
    float as_float;
    uint16_t as_uint;
  } z_offset_mm{.as_uint = swap16(*static_cast<uint16_t *>(val_ptr))};
  z_offset_mm.as_float = z_offset_mm.as_float / 100U;
  ExtUI::setZOffset_mm(z_offset_mm.as_float);
  // don't overwrite value the next update time as the display might autoincrement in parallel
  DGUSScreenVariableHandler::skipVP = var.VP;
}

#endif // HAS_BED_PROBE

#if ENABLED(BABYSTEPPING)
void DGUSScreenVariableHandler::HandleLiveAdjustZ(DGUS_VP_Variable &var, void *val_ptr) {
  DEBUG_ECHOLNPGM("HandleLiveAdjustZ");

  int16_t flag = swap16(*(uint16_t *)val_ptr);
  int16_t steps = flag ? -20 : 20;
  ExtUI::smartAdjustAxis_steps(steps, ExtUI::axis_t::Z, true);
  ScreenHandler.ForceCompleteUpdate();
  return;
}
#endif

#if FAN_COUNT > 0
void DGUSScreenVariableHandler::HandleFanControl(DGUS_VP_Variable &var, void *val_ptr) {
  DEBUG_ECHOLNPGM("HandleFanControl");
  UNUSED(val_ptr);
  *static_cast<uint8_t *>(var.memadr) =
      (*static_cast<uint8_t *>(var.memadr) > 0) ? static_cast<uint8_t>(0) : static_cast<uint8_t>(255);
}
#endif

/*
void DGUSScreenVariableHandler::HandleHeaterControl(DGUS_VP_Variable &var, void *val_ptr) {
  DEBUG_ECHOLNPGM("HandleHeaterControl");
  UNUSED(val_ptr);
  uint8_t preheat_temp = 0;
  switch (var.VP) {
    default:
      break;
#if HOTENDS >= 1
    case to_address(dgus::memory_layout::HeaterControl::E0):
#endif
#if HOTENDS >= 2
    case to_address(dgus::memory_layout::HeaterControl::E1):
#endif
#if HOTENDS >= 3
    case to_address(dgus::memory_layout::HeaterControl::E2):
#endif
      preheat_temp = PREHEAT_1_TEMP_HOTEND;
      break;

    case to_address(dgus::memory_layout::HeaterControl::Bed):
      preheat_temp = PREHEAT_1_TEMP_BED;
      break;
  }

  *(int16_t *)var.memadr = *(int16_t *)var.memadr > 0 ? 0 : preheat_temp;
}
*/
#if ENABLED(DGUS_PREHEAT_UI)
void DGUSScreenVariableHandler::HandlePreheat(DGUS_VP_Variable &var, void *val_ptr) {
  DEBUG_ECHOLNPGM("HandlePreheat");

  uint8_t e_temp = 0;
  uint8_t bed_temp = 0;
  const uint16_t preheat_option = swap16(*(uint16_t *)val_ptr);
  switch (preheat_option) {
    case 0: // Preheat PLA
#if defined(PREHEAT_1_TEMP_HOTEND) && defined(PREHEAT_1_TEMP_BED)
      e_temp = PREHEAT_1_TEMP_HOTEND;
      bed_temp = PREHEAT_1_TEMP_BED;
#endif
      break;
    case 1: // Preheat ABS
#if defined(PREHEAT_2_TEMP_HOTEND) && defined(PREHEAT_2_TEMP_BED)
      e_temp = PREHEAT_2_TEMP_HOTEND;
      bed_temp = PREHEAT_2_TEMP_BED;
#endif
      break;
    case 2: // Preheat PET
#if defined(PREHEAT_3_TEMP_HOTEND) && defined(PREHEAT_3_TEMP_BED)
      e_temp = PREHEAT_3_TEMP_HOTEND;
      bed_temp = PREHEAT_3_TEMP_BED;
#endif
      break;
    case 3: // Preheat FLEX
#if defined(PREHEAT_4_TEMP_HOTEND) && defined(PREHEAT_4_TEMP_BED)
      e_temp = PREHEAT_4_TEMP_HOTEND;
      bed_temp = PREHEAT_4_TEMP_BED;
#endif
      break;
    case 7: // Custom preheat
      break;
    case 9: // Cool down
      e_temp = 0;
      bed_temp = 0;
      break;
    default:
#if defined(PREHEAT_1_TEMP_HOTEND) && defined(PREHEAT_1_TEMP_BED)
      e_temp = PREHEAT_1_TEMP_HOTEND;
      bed_temp = PREHEAT_1_TEMP_BED;
#endif
      break;
  }

  switch (var.VP) {
    default:
      return;
#if HOTENDS >= 1
    case VP_E0_BED_PREHEAT:
      thermalManager.setTargetHotend(e_temp, 0);
#if HAS_HEATED_BED
      thermalManager.setTargetBed(bed_temp);
#endif
      break;
#endif
#if HOTENDS >= 2
    case VP_E1_BED_PREHEAT:
      thermalManager.setTargetHotend(e_temp, 1);
#if HAS_HEATED_BED
      thermalManager.setTargetBed(bed_temp);
#endif
      break;
#endif
  }

  // Go to the preheat screen to show the heating progress
  GotoScreen(DGUSLCD_SCREEN_PREHEAT);
}
#endif

#if ENABLED(DGUS_FILAMENT_LOADUNLOAD)
void DGUSScreenVariableHandler::HandleFilamentOption(DGUS_VP_Variable &var, void *val_ptr) {
  DEBUG_ECHOLNPGM("HandleFilamentOption");

  uint8_t e_temp = 0;
  filament_data.heated = false;
  uint16_t preheat_option = swap16(*(uint16_t *)val_ptr);
  if (preheat_option <= 8) // Load filament type
    filament_data.action = 1;
  else if (preheat_option >= 10) { // Unload filament type
    preheat_option -= 10;
    filament_data.action = 2;
    filament_data.purge_length = DGUS_FILAMENT_PURGE_LENGTH;
  } else // Cancel filament operation
    filament_data.action = 0;

  switch (preheat_option) {
    case 0: // Load PLA
#ifdef PREHEAT_1_TEMP_HOTEND
      e_temp = PREHEAT_1_TEMP_HOTEND;
#endif
      break;
    case 1: // Load ABS
#if ENABLED(PREHEAT_2_TEMP_HOTEND)
      e_temp = PREHEAT_2_TEMP_HOTEND;
#endif
      break;
    case 2: // Load PET
#ifdef PREHEAT_3_TEMP_HOTEND
      e_temp = PREHEAT_3_TEMP_HOTEND;
#endif
      break;
    case 3: // Load FLEX
#ifdef PREHEAT_4_TEMP_HOTEND
      e_temp = PREHEAT_4_TEMP_HOTEND;
#endif
      break;
    case 9: // Cool down
    default:
      e_temp = 0;
      break;
  }

  if (filament_data.action == 0) { // Go back to utility screen
#if HOTENDS >= 1
    thermalManager.setTargetHotend(e_temp, ExtUI::extruder_t::E0);
#endif
#if HOTENDS >= 2
    thermalManager.setTargetHotend(e_temp, ExtUI::extruder_t::E1);
#endif
    // TODO rubienr: missing hotends 3-6
    GotoScreen(DGUSLCD_SCREEN_UTILITY);
  } else { // Go to the preheat screen to show the heating progress
    switch (var.VP) {
      default:
        return;
#if HOTENDS >= 1
      case VP_E0_FILAMENT_LOAD_UNLOAD:
        filament_data.extruder = ExtUI::extruder_t::E0;
        thermalManager.setTargetHotend(e_temp, filament_data.extruder);
        break;
#endif
#if HOTENDS >= 2
      case VP_E1_FILAMENT_LOAD_UNLOAD:
        filament_data.extruder = ExtUI::extruder_t::E1;
        thermalManager.setTargetHotend(e_temp, filament_data.extruder);
        break;
#endif
    }
    GotoScreen(DGUSLCD_SCREEN_FILAMENT_HEATING);
  }
}

void DGUSScreenVariableHandler::HandleFilamentLoadUnload(DGUS_VP_Variable &var) {
  DEBUG_ECHOLNPGM("HandleFilamentLoadUnload");
  if (filament_data.action <= 0)
    return;

  // If we close to the target temperature, we can start load or unload the filament
  if (thermalManager.hotEnoughToExtrude(filament_data.extruder) &&
      thermalManager.targetHotEnoughToExtrude(filament_data.extruder)) {
    float movevalue = DGUS_FILAMENT_LOAD_LENGTH_PER_TIME;

    if (filament_data.action == 1) { // load filament
      if (!filament_data.heated) {
        GotoScreen(DGUSLCD_SCREEN_FILAMENT_LOADING);
        filament_data.heated = true;
      }
      movevalue = ExtUI::getAxisPosition_mm(filament_data.extruder) + movevalue;
    } else { // unload filament
      if (!filament_data.heated) {
        GotoScreen(DGUSLCD_SCREEN_FILAMENT_UNLOADING);
        filament_data.heated = true;
      }
      // Before unloading extrude to prevent jamming
      if (filament_data.purge_length >= 0) {
        movevalue = ExtUI::getAxisPosition_mm(filament_data.extruder) + movevalue;
        filament_data.purge_length -= movevalue;
      } else
        movevalue = ExtUI::getAxisPosition_mm(filament_data.extruder) - movevalue;
    }
    ExtUI::setAxisPosition_mm(movevalue, filament_data.extruder);
  }
}
#endif

void DGUSScreenVariableHandler::UpdateNewScreen(DGUSLCD_Screens newscreen, bool popup) {
  DEBUG_ECHOLNPAIR("SetNewScreen: ", newscreen);

  if (!popup) {
    memmove(&past_screens[1], &past_screens[0], sizeof(past_screens) - 1);
    past_screens[0] = current_screen;
  }

  current_screen = newscreen;
  skipVP = 0;
  ForceCompleteUpdate();
}

void DGUSScreenVariableHandler::PopToOldScreen() {
  DEBUG_ECHOLNPAIR("PopToOldScreen s=", past_screens[0]);
  GotoScreen(past_screens[0], true);
  memmove(&past_screens[0], &past_screens[1], sizeof(past_screens) - 1);
  past_screens[sizeof(past_screens) - 1] = DGUSLCD_SCREEN_MAIN;
}

void DGUSScreenVariableHandler::UpdateScreenVPData() {
  DEBUG_ECHOLNPAIR(" UpdateScreenVPData Screen: ", current_screen);

  const uint16_t *VPList = DGUSLCD_FindScreenVPMapList(current_screen);
  if (!VPList) {
    DEBUG_ECHOLNPAIR(" NO SCREEN FOR: ", current_screen);
    ScreenComplete = true;
    return; // nothing to do, likely a bug or boring screen.
  }

  // Round-robin updating of all VPs.
  VPList += update_ptr;

  bool sent_one = false;
  do {
    uint16_t VP = pgm_read_word(VPList);
    DEBUG_ECHOPAIR(" VP: ", VP);
    if (!VP) {
      update_ptr = 0;
      DEBUG_ECHOLNPGM(" UpdateScreenVPData done");
      ScreenComplete = true;
      return; // Screen completed.
    }

    if (VP == skipVP) {
      skipVP = 0;
      continue;
    }

    DGUS_VP_Variable rcpy{.VP = 0, .memadr = nullptr, .size = 0};
    if (populate_VPVar(VP, &rcpy)) {
      uint8_t expected_tx = rcpy.size + static_cast<uint8_t>(6); // expected overhead is 6 bytes + payload.
      // Send the VP to the display, but try to avoid overrunning the Tx Buffer.
      // But send at least one VP, to avoid getting stalled.
      if (rcpy.send_to_display_handler && (!sent_one || expected_tx <= DGUSDisplay::GetFreeTxBuffer())) {
        DEBUG_ECHOPAIR(" calling handler for ", rcpy.VP);
        sent_one = true;
        rcpy.send_to_display_handler(rcpy);
      } else {

#ifdef DEBUG_DGUSLCD
        size_t free_tx = dgusdisplay.GetFreeTxBuffer();
        UNUSED(free_tx);
        DEBUG_ECHOLNPAIR(" tx almost full: ", free_tx);
        DEBUG_ECHOPAIR(" update_ptr ", update_ptr);
#endif
        ScreenComplete = false;
        return; // please call again!
      }
    }

  } while (++update_ptr, ++VPList, true);
}

void DGUSDisplay::loop() {
  // protect against recursion… ProcessRx() may indirectly call idle() when injecting gcode commands.
  if (!no_reentrance) {
    no_reentrance = true;
    ProcessRx();
    no_reentrance = false;
  }
}

void DGUSDisplay::InitDisplay() {
  dgusserial.begin(DGUS_BAUDRATE);

  if (true
#if ENABLED(POWER_LOSS_RECOVERY)
      && !recovery.valid()
#endif
  )
    RequestScreen(
#if ENABLED(SHOW_BOOTSCREEN)
        DGUSLCD_SCREEN_BOOT
#else
        DGUSLCD_SCREEN_MAIN
#endif
    );
}

void DGUSDisplay::WriteVariable(uint16_t adr, const void *values, uint8_t valueslen, bool isstr) {
  const char *myvalues = static_cast<const char *>(values);
  bool strend = !myvalues;
  WriteHeader(adr, DGUS_CMD_WRITEVAR, valueslen);
  while (valueslen--) {
    char x{0};
    if (!strend)
      x = *myvalues++;
    if ((isstr && !x) || strend) {
      strend = true;
      x = ' ';
    }
    dgusserial.write(static_cast<uint8_t>(x));
  }
}

void DGUSDisplay::write_variable(uint16_t adr, const void *values, uint8_t valueslen, bool isstr) {
  const char *myvalues = static_cast<const char *>(values);
  bool strend = !myvalues;
  WriteHeader(adr, DGUS_CMD_WRITEVAR, valueslen);
  while (valueslen--) {
    char x{0};
    if (!strend)
      x = *myvalues++;
    if ((isstr && !x) || strend) {
      strend = true;
      x = ' ';
    }
    dgusserial.write(static_cast<uint8_t>(x));
  }
}

void DGUSDisplay::WriteVariablePGM(uint16_t adr, const void *values, uint8_t valueslen, bool isstr) {
  const char *myvalues = static_cast<const char *>(values);
  bool strend = !myvalues;
  WriteHeader(adr, DGUS_CMD_WRITEVAR, valueslen);
  while (valueslen--) {
    char x{0};
    if (!strend)
      x = pgm_read_byte(myvalues++);
    if ((isstr && !x) || strend) {
      strend = true;
      x = ' ';
    }
    dgusserial.write(static_cast<uint8_t>(x));
  }
}

void DGUSDisplay::write_variable_P(uint16_t adr, const void *values, uint8_t valueslen, bool isstr) {
  const char *myvalues = static_cast<const char *>(values);
  bool strend = !myvalues;
  WriteHeader(adr, DGUS_CMD_WRITEVAR, valueslen);
  while (valueslen--) {
    char x{0};
    if (!strend)
      x = pgm_read_byte(myvalues++);
    if ((isstr && !x) || strend) {
      strend = true;
      x = ' ';
    }
    dgusserial.write(static_cast<uint8_t>(x));
  }
}

void DGUSScreenVariableHandler::GotoScreen(DGUSLCD_Screens screen, bool ispopup) {
  DGUSDisplay::RequestScreen(screen);
  UpdateNewScreen(screen, ispopup);
}

bool DGUSScreenVariableHandler::loop() {
  dgusdisplay.loop();

  const millis_t ms = millis();
  static millis_t next_event_ms = 0;

  if (!IsScreenComplete() || ELAPSED(ms, next_event_ms)) {
    next_event_ms = ms + DGUS_UPDATE_INTERVAL_MS;
    UpdateScreenVPData();
  }

#if ENABLED(SHOW_BOOTSCREEN)
  static bool booted = false;
#if ENABLED(POWER_LOSS_RECOVERY)
  if (!booted && recovery.valid())
    booted = true;
#endif
  if (!booted && ELAPSED(ms, BOOTSCREEN_TIMEOUT)) {
    booted = true;
    GotoScreen(DGUSLCD_SCREEN_MAIN);
  }
#endif
  return IsScreenComplete();
}

void DGUSDisplay::RequestScreen(DGUSLCD_Screens screen) {
  DEBUG_ECHOLNPAIR("GotoScreen ", screen);
  const unsigned char gotoscreen[] = {0x5A, 0x01, (unsigned char)(screen >> 8U), (unsigned char)(screen & 0xFFU)};
  WriteVariable(0x84, gotoscreen, sizeof(gotoscreen));
}

void DGUSDisplay::ProcessRx() {

#if ENABLED(DGUS_SERIAL_STATS_RX_BUFFER_OVERRUNS)
  if (!dgusserial.available() && dgusserial.buffer_overruns()) {
    // Overrun, but reset the flag only when the buffer is empty
    // We want to extract as many as valid datagrams possible...
    DEBUG_ECHOPGM("OVFL");
    rx_datagram_state = DGUS_IDLE;
    // dgusserial.reset_rx_overun();
    dgusserial.flush();
  }
#endif

  uint8_t receivedbyte;
  while (dgusserial.available()) {
    switch (rx_datagram_state) {

      case DGUS_IDLE: // Waiting for the first header byte
        receivedbyte = static_cast<uint8_t>(dgusserial.read());
        // DEBUG_ECHOPAIR("< ",x);
        if (DGUS_HEADER1 == receivedbyte)
          rx_datagram_state = DGUS_HEADER1_SEEN;
        break;

      case DGUS_HEADER1_SEEN: // Waiting for the second header byte
        receivedbyte = static_cast<uint8_t>(dgusserial.read());
        // DEBUG_ECHOPAIR(" ",x);
        rx_datagram_state = (DGUS_HEADER2 == receivedbyte) ? DGUS_HEADER2_SEEN : DGUS_IDLE;
        break;

      case DGUS_HEADER2_SEEN: // Waiting for the length byte
        rx_datagram_len = static_cast<uint8_t>(dgusserial.read());
        DEBUG_ECHOPAIR(" (", rx_datagram_len, ") ");

        // Telegram min len is 3 (command and one word of payload)
        rx_datagram_state = WITHIN(rx_datagram_len, 3, DGUS_RX_BUFFER_SIZE) ? DGUS_WAIT_TELEGRAM : DGUS_IDLE;
        break;

      case DGUS_WAIT_TELEGRAM: // wait for complete datagram to arrive.
        if (dgusserial.available() < rx_datagram_len)
          return;

        Initialized = true; // We've talked to it, so we defined it as initialized.
        auto command = static_cast<uint8_t>(dgusserial.read());

        DEBUG_ECHOPAIR("# ", command);

        uint8_t readlen = rx_datagram_len - static_cast<uint8_t>(1); // command is part of len.
        unsigned char tmp[rx_datagram_len - 1];
        unsigned char *ptmp = tmp;
        while (readlen--) {
          receivedbyte = static_cast<uint8_t>(dgusserial.read());
          DEBUG_ECHOPAIR(" ", receivedbyte);
          *ptmp++ = receivedbyte;
        }
        DEBUG_ECHOPGM(" # ");
        // mostly we'll get this: 5A A5 03 82 4F 4B -- ACK on 0x82, so discard it.
        if (command == DGUS_CMD_WRITEVAR && 'O' == tmp[0] && 'K' == tmp[1]) {
          DEBUG_ECHOLNPGM(">");
          rx_datagram_state = DGUS_IDLE;
          break;
        }

        /* AutoUpload, (and answer to) Command 0x83 :
        |      tmp[0  1  2  3  4 ... ]
        | Example 5A A5 06 83 20 01 01 78 01 ……
        |          / /  |  |   \ /   |  \     \
        |        Header |  |    |    |   \_____\_ DATA (Words!)
        |     DatagramLen  /  VPAdr  |
        |           Command          DataLen (in Words) */
        if (command == DGUS_CMD_READVAR) {
          const uint16_t vp = tmp[0] << 8U | tmp[1];
          // const uint8_t dlen = tmp[2] << 1;  // Convert to Bytes. (Display works with words)
          // DEBUG_ECHOPAIR(" vp=", vp, " dlen=", dlen);
          DGUS_VP_Variable ramcopy{.VP = 0, .memadr = nullptr, .size = 0};
          if (populate_VPVar(vp, &ramcopy)) {
            if (ramcopy.set_by_display_handler)
              ramcopy.set_by_display_handler(ramcopy, &tmp[3]);
            else
              DEBUG_ECHOLNPGM(" VPVar found, no handler.");
          } else
            DEBUG_ECHOLNPAIR(" VPVar not found:", vp);

          rx_datagram_state = DGUS_IDLE;
          break;
        }

        // discard anything else
        rx_datagram_state = DGUS_IDLE;
    }
  }
}

size_t DGUSDisplay::GetFreeTxBuffer() { return DGUS_SERIAL_GET_TX_BUFFER_FREE(); }

void DGUSDisplay::WriteHeader(uint16_t adr, uint8_t cmd, uint8_t payloadlen) {
  dgusserial.write(DGUS_HEADER1);
  dgusserial.write(DGUS_HEADER2);
  dgusserial.write(payloadlen + static_cast<uint8_t>(3));
  dgusserial.write(cmd);
  dgusserial.write(static_cast<uint8_t>(adr >> 8U));
  dgusserial.write(static_cast<uint8_t>(adr & 0xFFU));
}

void DGUSDisplay::WritePGM(const char str[], uint8_t len) {
  while (len--)
    dgusserial.write(pgm_read_byte(str++));
}

// A SW memory barrier, to ensure GCC does not overoptimize loops
#define sw_barrier() asm volatile("" : : : "memory");

#endif // HAS_DGUS_LCD

} // namespace dgus
