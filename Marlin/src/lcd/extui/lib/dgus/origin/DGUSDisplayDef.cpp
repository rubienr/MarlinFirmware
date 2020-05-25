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

#include "../../../../../inc/MarlinConfig.h"

#if ENABLED(DGUS_LCD_UI_ORIGIN)

#include "../DGUSDisplayDef.h"
#include "../DGUSDisplay.h"

#include "../../../../../module/temperature.h"
#include "../../../../../module/motion.h"
#include "../../../../../module/planner.h"

#include "../../../../ultralcd.h"

#if ENABLED(DGUS_UI_MOVE_DIS_OPTION)
  uint16_t distanceToMove = 0.1;
#endif

const uint16_t VPList_Boot[] PROGMEM = {
  VP_MARLIN_VERSION,
  VP_UI_VERSION,
  0x0000
};

const uint16_t VPList_Main[] PROGMEM = {
  /* VP_M117, for completeness, but it cannot be auto-uploaded. */
  0x0000
};

const uint16_t VPList_Temp[] PROGMEM = {
  #if HOTENDS >= 1
    VP_T_E0_Is, VP_T_E0_Set,
  #endif
  #if HOTENDS >= 2
    VP_T_E1_Is, VP_T_E1_Set,
  #endif
  #if HOTENDS >= 3
    VP_T_E2_Is, VP_T_E2_Set,
  #endif
  #if HOTENDS >= 4
    VP_T_E3_Is, VP_T_E3_Set,
  #endif
  #if HOTENDS >= 5
    VP_T_E4_Is, VP_T_E4_Set,
  #endif
  #if HOTENDS >= 6
    VP_T_E5_Is, VP_T_E5_Set,
  #endif
  #if HAS_HEATED_BED
    VP_T_Bed_Is, VP_T_Bed_Set,
  #endif
  0x0000
};

const uint16_t VPList_Status[] PROGMEM = {
  /* VP_M117, for completeness, but it cannot be auto-uploaded */
  #if HOTENDS >= 1
    VP_T_E0_Is, VP_T_E0_Set,
  #endif
  #if HOTENDS >= 2
    VP_T_E1_Is, VP_T_E1_Set,
  #endif
  #if HOTENDS >= 3
    VP_T_E2_Is, VP_T_E2_Set,
  #endif
  #if HOTENDS >= 4
    VP_T_E3_Is, VP_T_E3_Set,
  #endif
  #if HOTENDS >= 5
    VP_T_E4_Is, VP_T_E4_Set,
  #endif
  #if HOTENDS >= 6
      VP_T_E5_Is, VP_T_E5_Set,
  #endif
  #if HAS_HEATED_BED
    VP_T_Bed_Is, VP_T_Bed_Set,
  #endif
  #if HAS_FAN0
    VP_Fan0_Percentage,
  #endif
  #if FAN_COUNT >= 2
    VP_Fan1_Percentage,
  #endif
  #if FAN_COUNT >= 3
    VP_Fan2_Percentage,
  #endif
  #if FAN_COUNT >= 4
    VP_Fan3_Percentage,
  #endif
  #if FAN_COUNT >= 5
    VP_Fan4_Percentage,
  #endif
  #if FAN_COUNT >= 6
    VP_Fan5_Percentage,
  #endif
  VP_XPos, VP_YPos, VP_ZPos,
  //VP_Fan0_Percentage, // TODO rubienr: duplicate?
  VP_Feedrate_Percentage,
  VP_PrintProgress_Percentage,
  0x0000
};

const uint16_t VPList_Status2[] PROGMEM = {
  /* VP_M117, for completeness, but it cannot be auto-uploaded */
  #if EXTRUDERS >= 1
    VP_Flowrate_E0,
  #endif
  #if EXTRUDERS >= 2
    VP_Flowrate_E1,
  #endif
  #if EXTRUDERS >= 3
    VP_Flowrate_E2,
  #endif
  #if EXTRUDERS >= 4
    VP_Flowrate_E3,
  #endif
  #if EXTRUDERS >= 5
    VP_Flowrate_E4,
  #endif
  #if EXTRUDERS >= 6
    VP_Flowrate_E5,
  #endif
  VP_PrintProgress_Percentage,
  VP_PrintTime,
  0x0000
};

const uint16_t VPList_ManualMove[] PROGMEM = {
  VP_XPos, VP_YPos, VP_ZPos,
  0x0000
};

const uint16_t VPList_ManualExtrude[] PROGMEM = {
  #if EXTRUDERS >= 1
    VP_E0Pos,
  #endif
  #if EXTRUDERS >= 2
    VP_E1Pos,
  #endif
  #if EXTRUDERS >= 3
    VP_E2Pos,
  #endif
  #if EXTRUDERS >= 4
    VP_E3Pos,
  #endif
  #if EXTRUDERS >= 5
    VP_E4Pos,
  #endif
  #if EXTRUDERS >= 6
    VP_E5Pos,
  #endif
  0x0000
};

const uint16_t VPList_FanAndFeedrate[] PROGMEM = {
  VP_Feedrate_Percentage,
  #if HAS_FAN0
    VP_Fan0_Percentage,
  #endif
  #if HAS_FAN1
    VP_Fan1_Percentage,
  #endif
  #if HAS_FAN2
    VP_Fan2_Percentage,
  #endif
  #if HAS_FAN3
    VP_Fan3_Percentage,
  #endif
  #if HAS_FAN4
    VP_Fan4_Percentage,
  #endif
  #if HAS_FAN5
    VP_Fan5_Percentage,
  #endif
  0x0000
};

const uint16_t VPList_SD_FlowRates[] PROGMEM = {
  #if EXTRUDERS >= 1
    VP_Flowrate_E0,
  #endif
  #if EXTRUDERS >= 2
    VP_Flowrate_E1,
  #endif
  #if EXTRUDERS >= 3
    VP_Flowrate_E2,
  #endif
  #if EXTRUDERS >= 4
    VP_Flowrate_E3,
  #endif
  #if EXTRUDERS >= 5
    VP_Flowrate_E4,
  #endif
  #if EXTRUDERS >= 6
    VP_Flowrate_E5,
  #endif
  0x0000
};

#if ENABLED(SDSUPPORT)
const uint16_t VPList_SDFileList[] PROGMEM = {
  VP_SD_FileName0,
  VP_SD_FileName1,
  VP_SD_FileName2,
  VP_SD_FileName3,
  VP_SD_FileName4,
  0x0000
};
#endif

const uint16_t VPList_SD_PrintManipulation[] PROGMEM = {
  VP_PrintProgress_Percentage, VP_PrintTime,
  0x0000
};

const uint16_t VPList_Info[] PROGMEM = {
    VP_MARLIN_DETAILED_VERSION,
    VP_MARLIN_DISTRIBUTION_DATE,
    VP_MARLIN_CONFIG_AUTHOR,
    VP_MARLIN_COMPILE_DATE,
    0x0000
};

const uint16_t VPList_Psu[] PROGMEM = {
    VP_PSU_CONTROL,
    0x0000
};

const uint16_t VPList_Motors[] PROGMEM = {
    VP_MOTOR_LOCK_UNLOK,
    0x0000
};

const uint16_t VPList_Lights[] PROGMEM = {
    VP_CASE_LIGHT_CONTROL,
    VP_CASE_COLOR_LED_CONTROL_0,
    VP_CASE_COLOR_LED_CONTROL_1,
    VP_CASE_COLOR_LED_CONTROL_2,
    0x0000
};

const uint16_t VPList_BedLevelingUbl[] PROGMEM = {
    VP_BED_LEVELING_PARAMETER__ON_OFF__DO_PROBE,
    VP_BED_LEVELING_PARAMETER__FADE_HEIGHT__SLOT_NUMBER,
    VP_BED_LEVELING_PARAMETER__LOAD_SAVE_SLOT,
    0x0000
};

const struct VPMapping VPMap[] PROGMEM = {
  { DGUSLCD_SCREEN_BOOT, VPList_Boot },
  { DGUSLCD_SCREEN_MAIN, VPList_Main },
  { DGUSLCD_SCREEN_TEMPERATURE, VPList_Temp },
  { DGUSLCD_SCREEN_STATUS, VPList_Status },
  { DGUSLCD_SCREEN_STATUS2, VPList_Status2 },
  { DGUSLCD_SCREEN_MANUALMOVE, VPList_ManualMove },
  { DGUSLCD_SCREEN_MANUALEXTRUDE, VPList_ManualExtrude },
  { DGUSLCD_SCREEN_FANANDFEEDRATE, VPList_FanAndFeedrate },
  { DGUSLCD_SCREEN_FLOWRATES, VPList_SD_FlowRates },
  { DGUSLCD_SCREEN_SDPRINTMANIPULATION, VPList_SD_PrintManipulation },
  { DGUSLCD_SCREEN_INFO, VPList_Info },
  { DGUSLCD_SCREEN_PSU, VPList_Psu },
  { DGUSLCD_SCREEN_MOTORS, VPList_Motors },
  { DGUSLCD_SCREEN_LIGHTS, VPList_Lights },
  { DGUSLCD_SCREEN_BED_LEVELING, VPList_BedLevelingUbl },

#if ENABLED(SDSUPPORT)
  { DGUSLCD_SCREEN_SDFILELIST, VPList_SDFileList },
#endif
  { 0 , nullptr } // List is terminated with an nullptr as table entry.
};

namespace {

const char MarlinVersion[] PROGMEM = SHORT_BUILD_VERSION;
const char MarlinDetailedVersion[] PROGMEM = DETAILED_BUILD_VERSION;
const char MarlinDistributionDate[] PROGMEM = STRING_DISTRIBUTION_DATE;
const char MarlinConfigurationAuthor[] PROGMEM = STRING_CONFIG_H_AUTHOR;
const char MarlinCompileDate[] PROGMEM = __DATE__;

struct DgusOriginVariables {
    uint16_t psu_control {0};
    uint16_t motors_control {0};
    uint16_t case_light_control {0};

    uint16_t color_led_control__on_off__intensity {0};
    uint16_t color_led_component__red__green {0};
    uint16_t color_led_component__blue__white {0};

    uint16_t bed_leveling__fade_height__slot_number {0};
    uint16_t bed_leveling__on_off__do_probe {0};
    uint16_t bed_leveling__load_save_slot {0};
} OriginVariables;

#if ENABLED(HAS_COLOR_LEDS)

void updateColorLeds() {
  const uint8_t enabled {static_cast<uint8_t>((OriginVariables.color_led_control__on_off__intensity & 0x00ff) >> 0)};
  switch (enabled) {
    default: return;
    case 0: return; // value unset, assume lights on request
    case 1: // disable color LEDs
      queue.enqueue_now_P(PSTR("M150 P0 R0 U0 B0 W0"));
      break;
    case 2: // enable color LEDs
    {
      char buf[32]{0};
      const uint8_t i{static_cast<uint8_t>((OriginVariables.color_led_control__on_off__intensity & 0xff00) >> 8)};
      const uint8_t r{static_cast<uint8_t>((OriginVariables.color_led_component__red__green & 0x00ff) >> 0)};
      const uint8_t g{static_cast<uint8_t>((OriginVariables.color_led_component__red__green & 0xff00) >> 8)};
      const uint8_t b{static_cast<uint8_t>((OriginVariables.color_led_component__blue__white & 0x00ff) >> 0)};
      const uint8_t w{static_cast<uint8_t>((OriginVariables.color_led_component__blue__white & 0xff00) >> 8)};
      sprintf(buf, "M150 P%d R%d U%d B%d W%d", i, r, g, b, w);
      queue.enqueue_one_now(buf);
    }
      break;
  }
}

uint16_t swap16(const uint16_t value) { return (value & 0xffU) << 8U | (value >> 8U); }

void HandleColorLedUpdate(DGUS_VP_Variable &var, void *val_ptr) {
  DEBUG_ECHOLNPGM("HandleColorLedUpdate");
  if (var.memadr)
  {
    *(uint16_t *) var.memadr = swap16(*(uint16_t *) val_ptr);
    updateColorLeds();
  }
}

#endif // HAS_COLOR_LEDS

#if ENABLED(AUTO_BED_LEVELING_UBL)
void HandleBedLevelingUbl() {
  DEBUG_ECHOLNPGM("HandleBedLevelingUbl");

  const uint8_t do_start_ubl = static_cast<uint8_t>((OriginVariables.bed_leveling__on_off__do_probe >> 8) & 0xff);
  const uint8_t on_off_state = static_cast<uint8_t>((OriginVariables.bed_leveling__on_off__do_probe >> 0) & 0xff);

  const uint8_t fade_height_mm_percent = static_cast<uint8_t>((OriginVariables.bed_leveling__fade_height__slot_number >> 0) & 0xff);
  const uint8_t slot_number = static_cast<uint8_t>((OriginVariables.bed_leveling__fade_height__slot_number >> 8) & 0xff);
  const uint8_t load_save_state = static_cast<uint8_t>((OriginVariables.bed_leveling__load_save_slot >> 0) & 0xff);

  // --- do UBL probing ---

  switch(do_start_ubl) {
    default:
      OriginVariables.bed_leveling__on_off__do_probe &= ~0xff00;
      break;
    case 0: break; // do nothing
    case 1: // do start
    {
      queue.enqueue_now_P(PSTR("G28 R X Y")); // home
      queue.enqueue_now_P(PSTR("G28 Z"));     // home
      queue.enqueue_now_P(PSTR("G29 P1"));    // probe mesh
      queue.enqueue_now_P(PSTR("G29 P3"));    // interpolate missing points

      OriginVariables.bed_leveling__on_off__do_probe &= ~0xff00; // clear request
    }
      break;
  }

  // --- enable/disable UBL ---

  switch(on_off_state) {
    default:
      OriginVariables.bed_leveling__on_off__do_probe &= ~0x00ff;
      break;
    case 0: break; // value unset
    case 1: // disable
      queue.enqueue_now_P(PSTR("M420 S0"));
      OriginVariables.bed_leveling__on_off__do_probe &= ~0x00ff;
      return;
    case 2: // enable
    {
      const uint8_t fade_height_mm = static_cast<uint8_t>(fade_height_mm_percent / 10U);
      const uint8_t fade_height_mm_fraction = static_cast<uint8_t>(fade_height_mm_percent % 10U);

      char buf[24]{0};
      sprintf_P(buf, PSTR("M420 S1 L%d Z%d.%02d"), slot_number, fade_height_mm, fade_height_mm_fraction);
      queue.enqueue_one_now(buf);

      OriginVariables.bed_leveling__on_off__do_probe &= ~0x00ff;
    }
      break;
  }

  // --- load mesh from slot ---

  switch(load_save_state) {
    default:
      OriginVariables.bed_leveling__load_save_slot &= ~0x00ff;
      break;
    case 0: break; // value unset
    case 1: // load
    {
      char buf[12]{0};
      sprintf_P(buf, PSTR("M420 L%d"), slot_number);
      queue.enqueue_one_now(buf);

      OriginVariables.bed_leveling__load_save_slot &= ~0x00ff;
    }
      break;
    case 2: // save
    {
      char buf[12]{0};
      sprintf(buf, "G29 S%d", slot_number);
      queue.enqueue_one_now(buf);
    }
      OriginVariables.bed_leveling__load_save_slot &= ~0x00ff;
      break;
  }

}

void HandleBedLevelingUblParameter(DGUS_VP_Variable &var, void *val_ptr) {
  DEBUG_ECHOLNPGM("HandleBedLevelingUblParameter");
  if (var.memadr)
  {
    *(uint16_t *) var.memadr = swap16(*(uint16_t *) val_ptr);
    HandleBedLevelingUbl();
  }
}

#endif // AUTO_BED_LEVELING_UBL

} // namespace

// Helper to define a DGUS_VP_Variable for common use cases.
#define VPHELPER(VPADR, VPADRVAR, RXFPTR, TXFPTR ) { .VP=VPADR, .memadr=VPADRVAR, .size=sizeof(VPADRVAR), \
  .set_by_display_handler = RXFPTR, .send_to_display_handler = TXFPTR }

// Helper to define a DGUS_VP_Variable when the sizeo of the var cannot be determined automaticalyl (eg. a string)
#define VPHELPER_STR(VPADR, VPADRVAR, STRLEN, RXFPTR, TXFPTR ) { .VP=VPADR, .memadr=VPADRVAR, .size=STRLEN, \
  .set_by_display_handler = RXFPTR, .send_to_display_handler = TXFPTR }

const struct DGUS_VP_Variable ListOfVP[] PROGMEM = {
  // Helper to detect touch events
  VPHELPER(VP_SCREENCHANGE, nullptr, DGUSScreenVariableHandler::ScreenChangeHook, nullptr),
  VPHELPER(VP_SCREENCHANGE_ASK, nullptr, DGUSScreenVariableHandler::ScreenChangeHookIfIdle, nullptr),
  #if ENABLED(SDSUPPORT)
    VPHELPER(VP_SCREENCHANGE_WHENSD, nullptr, DGUSScreenVariableHandler::ScreenChangeHookIfSD, nullptr),
  #endif
  VPHELPER(VP_CONFIRMED, nullptr, DGUSScreenVariableHandler::ScreenConfirmedOK, nullptr),

  VPHELPER(VP_TEMP_ALL_OFF, nullptr, &DGUSScreenVariableHandler::HandleAllHeatersOff, nullptr),

  #if ENABLED(DGUS_UI_MOVE_DIS_OPTION)
    VPHELPER(VP_MOVE_OPTION, &distanceToMove, &DGUSScreenVariableHandler::HandleManualMoveOption, nullptr),
  #endif
  #if ENABLED(DGUS_UI_MOVE_DIS_OPTION)
    VPHELPER(VP_MOVE_X, &distanceToMove, &DGUSScreenVariableHandler::HandleManualMove, nullptr),
    VPHELPER(VP_MOVE_Y, &distanceToMove, &DGUSScreenVariableHandler::HandleManualMove, nullptr),
    VPHELPER(VP_MOVE_Z, &distanceToMove, &DGUSScreenVariableHandler::HandleManualMove, nullptr),
    VPHELPER(VP_HOME_ALL, &distanceToMove, &DGUSScreenVariableHandler::HandleManualMove, nullptr),
  #else
    VPHELPER(VP_MOVE_X, nullptr, &DGUSScreenVariableHandler::HandleManualMove, nullptr),
    VPHELPER(VP_MOVE_Y, nullptr, &DGUSScreenVariableHandler::HandleManualMove, nullptr),
    VPHELPER(VP_MOVE_Z, nullptr, &DGUSScreenVariableHandler::HandleManualMove, nullptr),
    VPHELPER(VP_HOME_ALL, nullptr, &DGUSScreenVariableHandler::HandleManualMove, nullptr),
  #endif

  VPHELPER(VP_MOTOR_LOCK_UNLOK, &OriginVariables.motors_control, &DGUSScreenVariableHandler::HandleMotorLockUnlock, &DGUSScreenVariableHandler::DGUSLCD_SendWordValueToDisplay),
  #if ENABLED(POWER_LOSS_RECOVERY)
    VPHELPER(VP_POWER_LOSS_RECOVERY, nullptr, &DGUSScreenVariableHandler::HandlePowerLossRecovery, nullptr),
  #endif
  VPHELPER(VP_SETTINGS, nullptr, &DGUSScreenVariableHandler::HandleSettings, nullptr),

  // Boot and info screen
  { .VP = VP_MARLIN_VERSION, .memadr = (void*)MarlinVersion, .size = VP_MARLIN_VERSION_LEN, .set_by_display_handler = nullptr, .send_to_display_handler =&DGUSScreenVariableHandler::DGUSLCD_SendStringToDisplayPGM },
  // Boot screen
  { .VP = VP_UI_VERSION, .memadr = (void*)UI_VERSION, .size = UI_VERSION_LEN, .set_by_display_handler = nullptr, .send_to_display_handler =&DGUSScreenVariableHandler::DGUSLCD_SendStringToDisplayPGM },
  { .VP = VP_UI_FLAVOUR, .memadr = (void*)UI_FLAVOUR, .size = sizeof(UI_FLAVOUR), .set_by_display_handler = nullptr, .send_to_display_handler =&DGUSScreenVariableHandler::DGUSLCD_SendStringToDisplayPGM },
  // Info screen
  { .VP = VP_MARLIN_DETAILED_VERSION, .memadr = (void*)MarlinDetailedVersion, .size = VP_MARLIN_DETAILED_VERSION_LEN, .set_by_display_handler = nullptr, .send_to_display_handler =&DGUSScreenVariableHandler::DGUSLCD_SendStringToDisplayPGM },
  { .VP = VP_MARLIN_DISTRIBUTION_DATE, .memadr = (void*)MarlinDistributionDate, .size = VP_MARLIN_DISTRIBUTION_DATE_LEN, .set_by_display_handler = nullptr, .send_to_display_handler =&DGUSScreenVariableHandler::DGUSLCD_SendStringToDisplayPGM },
  { .VP = VP_MARLIN_COMPILE_DATE, .memadr = (void*)MarlinCompileDate, .size = VP_MARLIN_COMPILE_DATE_LEN, .set_by_display_handler = nullptr, .send_to_display_handler =&DGUSScreenVariableHandler::DGUSLCD_SendStringToDisplayPGM },
  { .VP = VP_MARLIN_CONFIG_AUTHOR, .memadr = (void*)MarlinConfigurationAuthor, .size = VP_MARLIN_CONFIG_AUTHOR_LEN, .set_by_display_handler = nullptr, .send_to_display_handler =&DGUSScreenVariableHandler::DGUSLCD_SendStringToDisplayPGM },
  // M117 LCD String (We don't need the string in memory but "just" push it to the display on demand, hence the nullptr
  { .VP = VP_M117, .memadr = nullptr, .size = VP_M117_LEN, .set_by_display_handler = nullptr, .send_to_display_handler =&DGUSScreenVariableHandler::DGUSLCD_SendStringToDisplay },

  // Temperature Data
  #if HOTENDS >= 1
    VPHELPER(VP_T_E0_Is, &thermalManager.temp_hotend[0].celsius, nullptr, DGUSScreenVariableHandler::DGUSLCD_SendFloatAsLongValueToDisplay<0>),
    VPHELPER(VP_T_E0_Set, &thermalManager.temp_hotend[0].target, DGUSScreenVariableHandler::HandleTemperatureChanged, &DGUSScreenVariableHandler::DGUSLCD_SendWordValueToDisplay),
    VPHELPER(VP_Flowrate_E0, nullptr, DGUSScreenVariableHandler::HandleFlowRateChanged, &DGUSScreenVariableHandler::DGUSLCD_SendWordValueToDisplay),
    VPHELPER(VP_E0Pos, &destination.e, nullptr, DGUSScreenVariableHandler::DGUSLCD_SendFloatAsLongValueToDisplay<2>),
    VPHELPER(VP_MOVE_E0, nullptr, &DGUSScreenVariableHandler::HandleManualExtrude, nullptr),
    VPHELPER(VP_E0_CONTROL, &thermalManager.temp_hotend[0].target, &DGUSScreenVariableHandler::HandleHeaterControl, nullptr),
    VPHELPER(VP_E0_STATUS, &thermalManager.temp_hotend[0].target, nullptr, &DGUSScreenVariableHandler::DGUSLCD_SendHeaterStatusToDisplay),
    #if ENABLED(DGUS_PREHEAT_UI)
      VPHELPER(VP_E0_BED_PREHEAT, nullptr, &DGUSScreenVariableHandler::HandlePreheat, nullptr),
    #endif
    #if ENABLED(PIDTEMP)
      VPHELPER(VP_E0_PID_P, &thermalManager.temp_hotend[0].pid.Kp, DGUSScreenVariableHandler::HandleTemperaturePIDChanged, DGUSScreenVariableHandler::DGUSLCD_SendTemperaturePID),
      VPHELPER(VP_E0_PID_I, &thermalManager.temp_hotend[0].pid.Ki, DGUSScreenVariableHandler::HandleTemperaturePIDChanged, DGUSScreenVariableHandler::DGUSLCD_SendTemperaturePID),
      VPHELPER(VP_E0_PID_D, &thermalManager.temp_hotend[0].pid.Kd, DGUSScreenVariableHandler::HandleTemperaturePIDChanged, DGUSScreenVariableHandler::DGUSLCD_SendTemperaturePID),
      VPHELPER(VP_PID_AUTOTUNE_E0, nullptr, &DGUSScreenVariableHandler::HandlePIDAutotune, nullptr),
    #endif
    #if ENABLED(DGUS_FILAMENT_LOADUNLOAD)
      VPHELPER(VP_E0_FILAMENT_LOAD_UNLOAD, nullptr, &DGUSScreenVariableHandler::HandleFilamentOption, &DGUSScreenVariableHandler::HandleFilamentLoadUnload),
    #endif
  #endif
  #if HOTENDS >= 2
    VPHELPER(VP_T_E1_Is, &thermalManager.temp_hotend[1].celsius, nullptr, DGUSLCD_SendFloatAsLongValueToDisplay<0>),
    VPHELPER(VP_T_E1_Set, &thermalManager.temp_hotend[1].target, DGUSScreenVariableHandler::HandleTemperatureChanged, &DGUSScreenVariableHandler::DGUSLCD_SendWordValueToDisplay),
    VPHELPER(VP_Flowrate_E1, nullptr, DGUSScreenVariableHandler::HandleFlowRateChanged, &DGUSScreenVariableHandler::DGUSLCD_SendWordValueToDisplay),
    VPHELPER(VP_MOVE_E1, nullptr, &DGUSScreenVariableHandler::HandleManualExtrude, nullptr),
    VPHELPER(VP_E1_CONTROL, &thermalManager.temp_hotend[1].target, &DGUSScreenVariableHandler::HandleHeaterControl, nullptr),
    VPHELPER(VP_E1_STATUS, &thermalManager.temp_hotend[1].target, nullptr, &DGUSScreenVariableHandler::DGUSLCD_SendHeaterStatusToDisplay),
    #if ENABLED(PIDTEMP)
      VPHELPER(VP_PID_AUTOTUNE_E1, nullptr, &DGUSScreenVariableHandler::HandlePIDAutotune, nullptr),
    #endif
  #endif
  #if HAS_HEATED_BED
    VPHELPER(VP_T_Bed_Is, &thermalManager.temp_bed.celsius, nullptr, DGUSScreenVariableHandler::DGUSLCD_SendFloatAsLongValueToDisplay<0>),
    VPHELPER(VP_T_Bed_Set, &thermalManager.temp_bed.target, DGUSScreenVariableHandler::HandleTemperatureChanged, &DGUSScreenVariableHandler::DGUSLCD_SendWordValueToDisplay),
    VPHELPER(VP_BED_CONTROL, &thermalManager.temp_bed.target, &DGUSScreenVariableHandler::HandleHeaterControl, nullptr),
    VPHELPER(VP_BED_STATUS, &thermalManager.temp_bed.target, nullptr, &DGUSScreenVariableHandler::DGUSLCD_SendHeaterStatusToDisplay),
    #if ENABLED(PIDTEMPBED)
      VPHELPER(VP_BED_PID_P, &thermalManager.temp_bed.pid.Kp, DGUSScreenVariableHandler::HandleTemperaturePIDChanged, DGUSScreenVariableHandler::DGUSLCD_SendTemperaturePID),
      VPHELPER(VP_BED_PID_I, &thermalManager.temp_bed.pid.Ki, DGUSScreenVariableHandler::HandleTemperaturePIDChanged, DGUSScreenVariableHandler::DGUSLCD_SendTemperaturePID),
      VPHELPER(VP_BED_PID_D, &thermalManager.temp_bed.pid.Kd, DGUSScreenVariableHandler::HandleTemperaturePIDChanged, DGUSScreenVariableHandler::DGUSLCD_SendTemperaturePID),
    #endif
  #endif

  // Fan Data
  #if FAN_COUNT
    #define FAN_VPHELPER(N) \
      VPHELPER(VP_Fan##N##_Percentage, &thermalManager.fan_speed[N], DGUSScreenVariableHandler::DGUSLCD_PercentageToUint8, &DGUSScreenVariableHandler::DGUSLCD_SendPercentageToDisplay), \
      VPHELPER(VP_FAN##N##_CONTROL, &thermalManager.fan_speed[N], &DGUSScreenVariableHandler::HandleFanControl, nullptr), \
      VPHELPER(VP_FAN##N##_STATUS, &thermalManager.fan_speed[N], nullptr, &DGUSScreenVariableHandler::DGUSLCD_SendFanStatusToDisplay),
    REPEAT(FAN_COUNT, FAN_VPHELPER)
  #endif

  // Feedrate
  VPHELPER(VP_Feedrate_Percentage, &feedrate_percentage, DGUSScreenVariableHandler::DGUSLCD_SetValueDirectly<int16_t>, &DGUSScreenVariableHandler::DGUSLCD_SendWordValueToDisplay ),

  // Position Data
  VPHELPER(VP_XPos, &current_position.x, nullptr, DGUSScreenVariableHandler::DGUSLCD_SendFloatAsLongValueToDisplay<2>),
  VPHELPER(VP_YPos, &current_position.y, nullptr, DGUSScreenVariableHandler::DGUSLCD_SendFloatAsLongValueToDisplay<2>),
  VPHELPER(VP_ZPos, &current_position.z, nullptr, DGUSScreenVariableHandler::DGUSLCD_SendFloatAsLongValueToDisplay<2>),

  // Print Progress
  VPHELPER(VP_PrintProgress_Percentage, nullptr, nullptr, DGUSScreenVariableHandler::DGUSLCD_SendPrintProgressToDisplay ),

  // Print Time
  VPHELPER_STR(VP_PrintTime, nullptr, VP_PrintTime_LEN, nullptr, DGUSScreenVariableHandler::DGUSLCD_SendPrintTimeToDisplay ),
  #if ENABLED(PRINTCOUNTER)
    VPHELPER_STR(VP_PrintAccTime, nullptr, VP_PrintAccTime_LEN, nullptr, DGUSScreenVariableHandler::DGUSLCD_SendPrintAccTimeToDisplay ),
    VPHELPER_STR(VP_PrintsTotal, nullptr, VP_PrintsTotal_LEN, nullptr, DGUSScreenVariableHandler::DGUSLCD_SendPrintsTotalToDisplay ),
  #endif

  VPHELPER(VP_X_STEP_PER_MM, &planner.settings.axis_steps_per_mm[X_AXIS], DGUSScreenVariableHandler::HandleStepPerMMChanged, DGUSScreenVariableHandler::DGUSLCD_SendFloatAsIntValueToDisplay<1>),
  VPHELPER(VP_Y_STEP_PER_MM, &planner.settings.axis_steps_per_mm[Y_AXIS], DGUSScreenVariableHandler::HandleStepPerMMChanged, DGUSScreenVariableHandler::DGUSLCD_SendFloatAsIntValueToDisplay<1>),
  VPHELPER(VP_Z_STEP_PER_MM, &planner.settings.axis_steps_per_mm[Z_AXIS], DGUSScreenVariableHandler::HandleStepPerMMChanged, DGUSScreenVariableHandler::DGUSLCD_SendFloatAsIntValueToDisplay<1>),
  #if HOTENDS >= 1
    VPHELPER(VP_E0_STEP_PER_MM, &planner.settings.axis_steps_per_mm[E_AXIS_N(0)], DGUSScreenVariableHandler::HandleStepPerMMExtruderChanged, DGUSScreenVariableHandler::DGUSLCD_SendFloatAsIntValueToDisplay<1>),
  #endif
  #if HOTENDS >= 2
    VPHELPER(VP_E1_STEP_PER_MM, &planner.settings.axis_steps_per_mm[E_AXIS_N(1)], DGUSScreenVariableHandler::HandleStepPerMMExtruderChanged, DGUSScreenVariableHandler::DGUSLCD_SendFloatAsIntValueToDisplay<1>),
  #endif

  // SDCard File listing.
  #if ENABLED(SDSUPPORT)
    VPHELPER(VP_SD_ScrollEvent, nullptr, DGUSScreenVariableHandler::DGUSLCD_SD_ScrollFilelist, nullptr),
    VPHELPER(VP_SD_FileSelected, nullptr, DGUSScreenVariableHandler::DGUSLCD_SD_FileSelected, nullptr),
    VPHELPER(VP_SD_FileSelectConfirm, nullptr, DGUSScreenVariableHandler::DGUSLCD_SD_StartPrint, nullptr),
    VPHELPER_STR(VP_SD_FileName0,  nullptr, VP_SD_FileName_LEN, nullptr, DGUSScreenVariableHandler::DGUSLCD_SD_SendFilename ),
    VPHELPER_STR(VP_SD_FileName1,  nullptr, VP_SD_FileName_LEN, nullptr, DGUSScreenVariableHandler::DGUSLCD_SD_SendFilename ),
    VPHELPER_STR(VP_SD_FileName2,  nullptr, VP_SD_FileName_LEN, nullptr, DGUSScreenVariableHandler::DGUSLCD_SD_SendFilename ),
    VPHELPER_STR(VP_SD_FileName3,  nullptr, VP_SD_FileName_LEN, nullptr, DGUSScreenVariableHandler::DGUSLCD_SD_SendFilename ),
    VPHELPER_STR(VP_SD_FileName4,  nullptr, VP_SD_FileName_LEN, nullptr, DGUSScreenVariableHandler::DGUSLCD_SD_SendFilename ),
    VPHELPER(VP_SD_ResumePauseAbort, nullptr, DGUSScreenVariableHandler::DGUSLCD_SD_ResumePauseAbort, nullptr),
    VPHELPER(VP_SD_AbortPrintConfirmed, nullptr, DGUSScreenVariableHandler::DGUSLCD_SD_ReallyAbort, nullptr),
    VPHELPER(VP_SD_Print_Setting, nullptr, DGUSScreenVariableHandler::DGUSLCD_SD_PrintTune, nullptr),
    #if HAS_BED_PROBE
      VPHELPER(VP_SD_Print_ProbeOffsetZ, &probe.offset.z, DGUSScreenVariableHandler::HandleProbeOffsetZChanged, &DGUSScreenVariableHandler::DGUSLCD_SendFloatAsIntValueToDisplay<2>),
      #if ENABLED(BABYSTEPPING)
        VPHELPER(VP_SD_Print_LiveAdjustZ, nullptr, DGUSScreenVariableHandler::HandleLiveAdjustZ, nullptr),
      #endif
    #endif
  #endif

  #if ENABLED(DGUS_UI_WAITING)
    VPHELPER(VP_WAITING_STATUS, nullptr, nullptr, DGUSScreenVariableHandler::DGUSLCD_SendWaitingStatusToDisplay),
  #endif

  // Messages for the User, shared by the popup and the kill screen. They cant be autouploaded as we do not buffer content.
  { .VP = VP_MSGSTR1, .memadr = nullptr, .size = VP_MSGSTR1_LEN, .set_by_display_handler = nullptr, .send_to_display_handler = &DGUSScreenVariableHandler::DGUSLCD_SendStringToDisplayPGM },
  { .VP = VP_MSGSTR2, .memadr = nullptr, .size = VP_MSGSTR2_LEN, .set_by_display_handler = nullptr, .send_to_display_handler = &DGUSScreenVariableHandler::DGUSLCD_SendStringToDisplayPGM },
  { .VP = VP_MSGSTR3, .memadr = nullptr, .size = VP_MSGSTR3_LEN, .set_by_display_handler = nullptr, .send_to_display_handler = &DGUSScreenVariableHandler::DGUSLCD_SendStringToDisplayPGM },
  { .VP = VP_MSGSTR4, .memadr = nullptr, .size = VP_MSGSTR4_LEN, .set_by_display_handler = nullptr, .send_to_display_handler = &DGUSScreenVariableHandler::DGUSLCD_SendStringToDisplayPGM },

  // Power
  #if ENABLED(PSU_CONTROL)
    VPHELPER(VP_PSU_CONTROL, &OriginVariables.psu_control, &DGUSScreenVariableHandler::HandlePsuOnOff, &DGUSScreenVariableHandler::DGUSLCD_SendWordValueToDisplay),
  #endif

  // Light
  VPHELPER(VP_CASE_LIGHT_CONTROL, &OriginVariables.case_light_control, &DGUSScreenVariableHandler::HandleCaseLight, &DGUSScreenVariableHandler::DGUSLCD_SendWordValueToDisplay),
  VPHELPER(VP_CASE_COLOR_LED_CONTROL_0, &OriginVariables.color_led_control__on_off__intensity, &HandleColorLedUpdate, &DGUSScreenVariableHandler::DGUSLCD_SendWordValueToDisplay),
  VPHELPER(VP_CASE_COLOR_LED_CONTROL_1, &OriginVariables.color_led_component__red__green, &HandleColorLedUpdate, &DGUSScreenVariableHandler::DGUSLCD_SendWordValueToDisplay),
  VPHELPER(VP_CASE_COLOR_LED_CONTROL_2, &OriginVariables.color_led_component__blue__white, &HandleColorLedUpdate, &DGUSScreenVariableHandler::DGUSLCD_SendWordValueToDisplay),

  // Bed leveling
  VPHELPER(VP_BED_LEVELING_PARAMETER__FADE_HEIGHT__SLOT_NUMBER, &OriginVariables.bed_leveling__fade_height__slot_number, &HandleBedLevelingUblParameter, &DGUSScreenVariableHandler::DGUSLCD_SendWordValueToDisplay),
  VPHELPER(VP_BED_LEVELING_PARAMETER__ON_OFF__DO_PROBE, &OriginVariables.bed_leveling__on_off__do_probe, &HandleBedLevelingUblParameter, &DGUSScreenVariableHandler::DGUSLCD_SendWordValueToDisplay),
  VPHELPER(VP_BED_LEVELING_PARAMETER__LOAD_SAVE_SLOT, &OriginVariables.bed_leveling__load_save_slot, &HandleBedLevelingUblParameter, &DGUSScreenVariableHandler::DGUSLCD_SendWordValueToDisplay),

  VPHELPER(0, 0, 0, 0)  // must be last entry.
};

#endif // DGUS_LCD_UI_ORIGIN
