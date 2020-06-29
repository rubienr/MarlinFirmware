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

#include "../../../../../inc/MarlinConfig.h"

#if ENABLED(DGUS_LCD_UI_ORIGIN)

#include "../../../../../module/planner.h"
#include "../../../../../module/temperature.h"
#include "../DGUSDisplay.h"
#include "../DGUSDisplayDef.h"
#include "macros.h"

#if ENABLED(DGUS_UI_MOVE_DIS_OPTION)
uint16_t distanceToMove = 0.1;
#endif

const struct VPMapping VPMap[] PROGMEM {
  {DGUSLCD_SCREEN_BOOT, dgus_origin::boot::screen_variables},
      {DGUSLCD_SCREEN_MAIN, dgus_origin::main::screen_variables},
#if ENABLED(DGUS_ORIGIN_TEMPERATURES)
      {DGUSLCD_SCREEN_TEMPERATURE, dgus_origin::temperatures::screen_variables},
#endif
#if ENABLED(DGUS_ORIGIN_STATUS)
      {DGUSLCD_SCREEN_STATUS, dgus_origin::status::screen_variables_1},
      {DGUSLCD_SCREEN_STATUS2, dgus_origin::status::screen_variables_2},
#endif
#if ENABLED(DGUS_ORIGIN_MANUAL_MOVE)
      {DGUSLCD_SCREEN_MANUALMOVE, dgus_origin::manual_move::screen_variables},
#endif
#if ENABLED(DGUS_ORIGIN_MANUAL_EXTRUDE)
      {DGUSLCD_SCREEN_MANUALEXTRUDE, dgus_origin::manual_extrude::screen_variables},
#endif
#if ENABLED(DGUS_ORIGIN_SPEED_RATES)
      {DGUSLCD_SCREEN_SPEED_RATES, dgus_origin::speedrates::screen_variables},
#endif
#if ENABLED(DGUS_ORIGIN_SDPRINT_MANIPULATION)
      {DGUSLCD_SCREEN_SDPRINTMANIPULATION, dgus_origin::sdprint_manipulation::screen_variables},
#endif
#if ENABLED(DGUS_ORIGIN_INFO)
      {DGUSLCD_SCREEN_INFO, dgus_origin::info::screen_variables},
#endif
#if ENABLED(DGUS_ORIGIN_TOOLS)
      {DGUSLCD_SCREEN_TOOLS, dgus_origin::tools::screen_variables},
#endif
#if ENABLED(DGUS_ORIGIN_PSU_CONTROL)
      {DGUSLCD_SCREEN_PSU, dgus_origin::psu_control::screen_variables},
#endif
#if ENABLED(AUTO_BED_LEVELING_UBL)
      {DGUSLCD_SCREEN_BED_LEVELING, dgus_origin::bed_leveling_ubl::screen_variables},
#endif
#if ENABLED(DGUS_ORIGIN_DRIVER_CONTROL)
      {DGUSLCD_SCREEN_MOTORS, dgus_origin::driver_control::screen_variables},
#endif
#if ANY(DGUS_ORIGIN_LIGHTS)
      {DGUSLCD_SCREEN_LIGHTS, dgus_origin::lights::screen_variables},
#endif
#if ENABLED(HAS_BED_PROBE)
      {DGUSLCD_SCREEN_PROBE_OFFSET, dgus_origin::nozzle_offset::screen_variables},
#endif
#if ENABLED(EEPROM_SETTINGS)
      {DGUSLCD_SCREEN_EEPROM, dgus_origin::eeprom::screen_variables},
#endif
#if ENABLED(DGUS_ORIGIN_FILAMENT_LOAD_UNLOAD)
      {DGUSLCD_SCREEN_FILAMENT_LOAD_UNLOAD, dgus_origin::filament::screen_variables},
#endif
#if ENABLED(DGUS_ORIGIN_HOMING)
      {DGUSLCD_SCREEN_HOMING, dgus_origin::homing::screen_variables},

#endif
#if ENABLED(DGUS_ORIGIN_SDFILES)
      {DGUSLCD_SCREEN_SDFILELIST, dgus_origin::sdfiles::screen_variables},
#endif
  {
    0, nullptr
  } // List is terminated with an nullptr as table entry.
};

const struct DGUS_VP_Variable ListOfVP[] PROGMEM {
  // helper to detect touch events
  VPHELPER(
      dgus::memory_layout::ScreenAction::ScreenChange, nullptr, DGUSScreenVariableHandler::ScreenChangeHook, nullptr),
      VPHELPER(dgus::memory_layout::ScreenAction::ScreenChangeAsk,
               nullptr,
               DGUSScreenVariableHandler::ScreenChangeHookIfIdle,
               nullptr),

#if ENABLED(DGUS_ORIGIN_SDFILES)
      VPHELPER(VP_SCREENCHANGE_WHENSD, nullptr, DGUSScreenVariableHandler::ScreenChangeHookIfSD, nullptr),
#endif
      VPHELPER(dgus::memory_layout::ScreenAction::UserConfirmed,
               nullptr,
               DGUSScreenVariableHandler::ScreenConfirmedOK,
               nullptr),

      VPHELPER(dgus::memory_layout::ScreenAction::AllHeaderOff,
               nullptr,
               &DGUSScreenVariableHandler::HandleAllHeatersOff,
               nullptr),

#if ENABLED(DGUS_ORIGIN_MANUAL_MOVE)
      VPHELPER(dgus::memory_layout::MoveAxis::Distance,
               &dgus_origin::manual_move::cached_state.distance_10_um,
               &dgus_origin::manual_move::handle_move_argument_update,
               &dgus::handler::send_word),
      VPHELPER(dgus::memory_layout::MoveAxis::Command,
               &dgus_origin::manual_move::cached_state.move_command,
               &dgus_origin::manual_move::handle_move_argument_update,
               &dgus::handler::send_word),
#endif
#if ENABLED(DGUS_ORIGIN_HOMING)
      VPHELPER(dgus::memory_layout::Homing::Control,
               &dgus_origin::homing::cached_state,
               &dgus_origin::homing::handle_homing_command,
               &dgus_origin::homing::handle_send_homing_status),
#endif
      VPHELPER(dgus::memory_layout::DriverControl::LockUnlockControl,
               &dgus_origin::driver_control::cached_state,
               &dgus_origin::driver_control::handle_motor_lock_unlock,
               &dgus::handler::send_word),
#if ENABLED(POWER_LOSS_RECOVERY)
      VPHELPER(VP_POWER_LOSS_RECOVERY, nullptr, &DGUSScreenVariableHandler::HandlePowerLossRecovery, nullptr),
#endif

#if ENABLED(EEPROM_SETTINGS)
      VPHELPER(dgus::memory_layout::Eeprom::Control,
               &dgus_origin::eeprom::cached_state,
               &dgus_origin::eeprom::handle_eeprom_store_restore,
               &dgus::handler::send_word),
#endif

      // boot screen
      {.VP = to_address(dgus::memory_layout::UiVersion::MajorMinor),
       .memadr = const_cast<uint16_t *>(&dgus_origin::versions::ui_version.low_word),
       .size = 2,
       .set_by_display_handler = nullptr,
       .send_to_display_handler = &dgus::handler::send_word},
      {.VP = to_address(dgus::memory_layout::UiVersion::Patch),
       .memadr = const_cast<uint16_t *>(&dgus_origin::versions::ui_version.high_word),
       .size = 2,
       .set_by_display_handler = nullptr,
       .send_to_display_handler = &dgus::handler::send_word},
      {.VP = to_address(dgus::memory_layout::UiVersion::Flavour),
       .memadr = const_cast<dgus_origin::versions::UiFlavor *>(&dgus_origin::versions::ui_flavor),
       .size = to_uint8_t(dgus::memory_layout::UiVersion::FlavourBytes),
       .set_by_display_handler = nullptr,
       .send_to_display_handler = &DGUSScreenVariableHandler::DGUSLCD_SendStringToDisplayPGM},

      // boot and info screen
      {.VP = to_address(dgus::memory_layout::MarlinVersion::Version),
       .memadr = (void *)dgus_origin::versions::marlin_version::MarlinVersion,
       .size = constexpr_strlen(dgus_origin::versions::marlin_version::MarlinVersion),
       .set_by_display_handler = nullptr,
       .send_to_display_handler = &DGUSScreenVariableHandler::DGUSLCD_SendStringToDisplayPGM},

      // info screen
      {.VP = to_address(dgus::memory_layout::MarlinVersion::DetailedVersion),
       .memadr = (void *)dgus_origin::versions::marlin_version::MarlinDetailedVersion,
       .size = constexpr_strlen(dgus_origin::versions::marlin_version::MarlinDetailedVersion),
       .set_by_display_handler = nullptr,
       .send_to_display_handler = &DGUSScreenVariableHandler::DGUSLCD_SendStringToDisplayPGM},
      {.VP = to_address(dgus::memory_layout::MarlinVersion::DistributionDate),
       .memadr = (void *)dgus_origin::versions::marlin_version::MarlinDistributionDate,
       .size = constexpr_strlen(dgus_origin::versions::marlin_version::MarlinDistributionDate),
       .set_by_display_handler = nullptr,
       .send_to_display_handler = &DGUSScreenVariableHandler::DGUSLCD_SendStringToDisplayPGM},
      {.VP = to_address(dgus::memory_layout::MarlinVersion::CompileDate),
       .memadr = (void *)dgus_origin::versions::marlin_version::MarlinCompileDate,
       .size = constexpr_strlen(dgus_origin::versions::marlin_version::MarlinCompileDate),
       .set_by_display_handler = nullptr,
       .send_to_display_handler = &DGUSScreenVariableHandler::DGUSLCD_SendStringToDisplayPGM},
      {.VP = to_address(dgus::memory_layout::MarlinVersion::ConfigAuthor),
       .memadr = (void *)dgus_origin::versions::marlin_version::MarlinConfigurationAuthor,
       .size = constexpr_strlen(dgus_origin::versions::marlin_version::MarlinConfigurationAuthor),
       .set_by_display_handler = nullptr,
       .send_to_display_handler = &DGUSScreenVariableHandler::DGUSLCD_SendStringToDisplayPGM},

      // M117 LCD String (We don't need the string in memory but "just" push it to
      // the display on demand, hence the nullptr
      {.VP = to_address(dgus::memory_layout::LcdMessage::M117),
       .memadr = nullptr,
       .size = to_uint8_t(dgus::memory_layout::LcdMessage::M117Bytes),
       .set_by_display_handler = nullptr,
       .send_to_display_handler = &DGUSScreenVariableHandler::DGUSLCD_SendStringToDisplay},

// hotends - temperature data and control
#if ENABLED(DGUS_ORIGIN_TEMPERATURES)
      VPHELPER(dgus::memory_layout::TemperatureControl::Control,
               &dgus_origin::temperatures::cached_state.control,
               &dgus_origin::temperatures::handle_temperature_control_command,
               &dgus::handler::send_word),
      VPHELPER(dgus::memory_layout::Temperature::ExtruderNSet,
               &dgus_origin::temperatures::cached_state.temperatures.hotend_target_temperature.celsius,
               &dgus_origin::temperatures::handle_set_hotend_temperature,
               &dgus::handler::send_word),
      VPHELPER(dgus::memory_layout::Temperature::BedSet,
               &Temperature::temp_bed.target,
               &dgus_origin::temperatures::handle_set_bed_temperature,
               &dgus::handler::send_word),
      VPHELPER(dgus::memory_layout::Temperature::ExtruderNIs,
               nullptr,
               nullptr,
               &dgus_origin::temperatures::handle_send_hotend_temperature_to_display),
#if ENABLED(HAS_HEATED_BED)
      VPHELPER(dgus::memory_layout::Temperature::BedIs,
               &Temperature::temp_bed.celsius,
               nullptr,
               &dgus::handler::send_uint_word_from_float),
#endif
#if ENABLED(HAS_HEATED_CHAMBER)
      VPHELPER(dgus::memory_layout::Temperature::ChamberSet,
               &Temperature::temp_bed.target,
               &dgus_origin::temperatures::handle_set_chamber_temperature,
               &dgus::handler::send_word),
      VPHELPER(dgus::memory_layout::Temperature::ChamberIs,
               &Temperature::temp_chamber.celsius,
               nullptr,
               &dgus::handler::send_uint_word_from_float),
#endif
      VPHELPER(dgus::memory_layout::Temperature::Status,
               &dgus_origin::temperatures::cached_state.status,
               &dgus_origin::temperatures::handle_heating_status,
               &dgus::handler::send_word),
#endif // ENABLED(DGUS_ORIGIN_TEMPERATURES)

// hotends - enabled PID temp
#if HOTENDS_XXX
#if ENABLED(PIDTEMP)
#define HOTENDS_VPHELPER(N)                                            \
  VPHELPER(dgus::memory_layout::Pid::E##N##P,                          \
           &thermalManager.temp_hotend[N].pid.Kp,                      \
           DGUSScreenVariableHandler::HandleTemperaturePIDChanged,     \
           DGUSScreenVariableHandler::DGUSLCD_SendTemperaturePID),     \
      VPHELPER(dgus::memory_layout::Pid::E##N##I,                      \
               &thermalManager.temp_hotend[N].pid.Ki,                  \
               DGUSScreenVariableHandler::HandleTemperaturePIDChanged, \
               DGUSScreenVariableHandler::DGUSLCD_SendTemperaturePID), \
      VPHELPER(dgus::memory_layout::Pid::E##N##D,                      \
               &thermalManager.temp_hotend[N].pid.Kd,                  \
               DGUSScreenVariableHandler::HandleTemperaturePIDChanged, \
               DGUSScreenVariableHandler::DGUSLCD_SendTemperaturePID), \
      VPHELPER(                                                        \
          dgus::memory_layout::PidAutotune::E##N, nullptr, &DGUSScreenVariableHandler::HandlePIDAutotune, nullptr),
      REPEAT(HOTENDS, HOTENDS_VPHELPER)
#undef HOTENDS_VPHELPER
#endif // ENABLED(PIDTEMP)
#endif // HOTENDS

// hotends - manual extrude
#if HOTENDS
#define HOTENDS_VPHELPER(N)                                                                                          \
  VPHELPER(dgus::memory_layout::PositionE::E##N, nullptr, &DGUSScreenVariableHandler::HandleManualExtrude, nullptr), \
      VPHELPER(dgus::memory_layout::MoveE::E##N, nullptr, &DGUSScreenVariableHandler::HandleManualExtrude, nullptr),
          REPEAT(HOTENDS, HOTENDS_VPHELPER)
#undef HOTENDS_VPHELPER
#endif // HOTENDS

// hotends - steps per mm
#if HOTENDS
#define HOTENDS_VPHELPER(N)                                           \
  VPHELPER(dgus::memory_layout::StepsPerMm::E##N,                     \
           &planner.settings.axis_steps_per_mm[E_AXIS_N(N)],          \
           DGUSScreenVariableHandler::HandleStepPerMMExtruderChanged, \
           DGUSScreenVariableHandler::DGUSLCD_SendFloatAsIntValueToDisplay<1>),
              REPEAT(HOTENDS, HOTENDS_VPHELPER)
#undef HOTENDS_VPHELPER
#endif // HOTENDS

// hotends control, rate and status
#if HOTENDS > 0
                  VPHELPER(dgus::memory_layout::FlowRate::ExtruderN,
                           &dgus_origin::speedrates::cached_state_flow.rate,
                           &dgus_origin::speedrates::handle_set_flow_rate,
                           &dgus::handler::send_word),

      VPHELPER(dgus::memory_layout::FlowRate::Control,
               &dgus_origin::speedrates::cached_state_flow.control,
               &dgus_origin::speedrates::handle_flow_control_command,
               &dgus::handler::send_word),

      VPHELPER(dgus::memory_layout::FlowRate::Status,
               &dgus_origin::speedrates::cached_state_flow.status,
               nullptr,
               &dgus::handler::send_word),
#endif // HOTENDS

// TODO rubienr - fix define
#if ENABLED(DGUS_PREHEAT_UI)
      VPHELPER(VP_E0_BED_PREHEAT, nullptr, &DGUSScreenVariableHandler::HandlePreheat, nullptr),
#endif

#if HAS_HEATED_BED

#if ENABLED(PIDTEMPBED)
      VPHELPER(VP_BED_PID_P,
               &thermalManager.temp_bed.pid.Kp,
               DGUSScreenVariableHandler::HandleTemperaturePIDChanged,
               DGUSScreenVariableHandler::DGUSLCD_SendTemperaturePID),
      VPHELPER(VP_BED_PID_I,
               &thermalManager.temp_bed.pid.Ki,
               DGUSScreenVariableHandler::HandleTemperaturePIDChanged,
               DGUSScreenVariableHandler::DGUSLCD_SendTemperaturePID),
      VPHELPER(VP_BED_PID_D,
               &thermalManager.temp_bed.pid.Kd,
               DGUSScreenVariableHandler::HandleTemperaturePIDChanged,
               DGUSScreenVariableHandler::DGUSLCD_SendTemperaturePID),
#endif
#endif

// fan control, rate and status
#if FAN_COUNT > 0
      VPHELPER(dgus::memory_layout::FanSpeed::FanN,
               &dgus_origin::speedrates::cached_state_fan.rate,
               &dgus_origin::speedrates::handle_set_fan_rate,
               &dgus::handler::send_word),
      VPHELPER(dgus::memory_layout::FanSpeed::Control,
               &dgus_origin::speedrates::cached_state_fan.control,
               &dgus_origin::speedrates::handle_fan_control_command,
               &dgus::handler::send_word),
      VPHELPER(dgus::memory_layout::FanSpeed::Status,
               &dgus_origin::speedrates::cached_state_fan.status,
               nullptr,
               &dgus::handler::send_word),
#endif

      // feed rate
      VPHELPER(dgus::memory_layout::FeedRate::Percentage,
               &feedrate_percentage,
               &dgus::handler::assign_int_from_int_word,
               &dgus::handler::send_word_from_int),

      // TODO rubienr - wrong assumption (float)?
      //  if yes then implement template function to send position depending on axis type
      // axis position data
      VPHELPER(dgus::memory_layout::PositionAxis::X,
               &current_position.x,
               nullptr,
               DGUSScreenVariableHandler::DGUSLCD_SendFloatAsLongValueToDisplay<2>),
      VPHELPER(dgus::memory_layout::PositionAxis::Y,
               &current_position.y,
               nullptr,
               DGUSScreenVariableHandler::DGUSLCD_SendFloatAsLongValueToDisplay<2>),
      VPHELPER(dgus::memory_layout::PositionAxis::Z,
               &current_position.z,
               nullptr,
               DGUSScreenVariableHandler::DGUSLCD_SendFloatAsLongValueToDisplay<2>),
      VPHELPER(dgus::memory_layout::PositionAxis::E,
#if ENABLED(LCD_SHOW_E_TOTAL)
               &e_move_accumulator,
#else
               &current_position.e,
#endif
               nullptr,
               DGUSScreenVariableHandler::DGUSLCD_SendFloatAsLongValueToDisplay<2>),

      // print progress
      VPHELPER(dgus::memory_layout::PrintStats::PrintProgressPercentage,
               nullptr,
               nullptr,
               DGUSScreenVariableHandler::DGUSLCD_SendPrintProgressToDisplay),

  // print time
#if ENABLED(LCD_SET_PROGRESS_MANUALLY)
      VPHELPER_STR(dgus::memory_layout::PrintStats::PrintTime, nullptr, 0, nullptr, &dgus::handler::send_print_time),
#endif
#if ENABLED(PRINTCOUNTER)
      VPHELPER_STR(VP_PrintAccTime,
                   nullptr,
                   VP_PrintAccTime_LEN,
                   nullptr,
                   DGUSScreenVariableHandler::DGUSLCD_SendPrintAccTimeToDisplay),
      VPHELPER_STR(VP_PrintsTotal,
                   nullptr,
                   VP_PrintsTotal_LEN,
                   nullptr,
                   DGUSScreenVariableHandler::DGUSLCD_SendPrintsTotalToDisplay),
#endif

      // TODO rubienr - not used on any screen: dgus::memory_layout::StepsPerMm::*
      VPHELPER(dgus::memory_layout::StepsPerMm::X,
               &planner.settings.axis_steps_per_mm[X_AXIS],
               DGUSScreenVariableHandler::HandleStepPerMMChanged,
               DGUSScreenVariableHandler::DGUSLCD_SendFloatAsIntValueToDisplay<1>),
      VPHELPER(dgus::memory_layout::StepsPerMm::Y,
               &planner.settings.axis_steps_per_mm[Y_AXIS],
               DGUSScreenVariableHandler::HandleStepPerMMChanged,
               DGUSScreenVariableHandler::DGUSLCD_SendFloatAsIntValueToDisplay<1>),
      VPHELPER(dgus::memory_layout::StepsPerMm::Z,
               &planner.settings.axis_steps_per_mm[Z_AXIS],
               DGUSScreenVariableHandler::HandleStepPerMMChanged,
               DGUSScreenVariableHandler::DGUSLCD_SendFloatAsIntValueToDisplay<1>),

// SD card file listing
// TODO rubienr
#if ENABLED(DGUS_ORIGIN_SDFILES)
      VPHELPER(VP_SD_ScrollEvent, nullptr, DGUSScreenVariableHandler::DGUSLCD_SD_ScrollFilelist, nullptr),
      VPHELPER(VP_SD_FileSelected, nullptr, DGUSScreenVariableHandler::DGUSLCD_SD_FileSelected, nullptr),
      VPHELPER(VP_SD_FileSelectConfirm, nullptr, DGUSScreenVariableHandler::DGUSLCD_SD_StartPrint, nullptr),
      VPHELPER_STR(
          VP_SD_FileName0, nullptr, VP_SD_FileName_LEN, nullptr, DGUSScreenVariableHandler::DGUSLCD_SD_SendFilename),
      VPHELPER_STR(
          VP_SD_FileName1, nullptr, VP_SD_FileName_LEN, nullptr, DGUSScreenVariableHandler::DGUSLCD_SD_SendFilename),
      VPHELPER_STR(
          VP_SD_FileName2, nullptr, VP_SD_FileName_LEN, nullptr, DGUSScreenVariableHandler::DGUSLCD_SD_SendFilename),
      VPHELPER_STR(
          VP_SD_FileName3, nullptr, VP_SD_FileName_LEN, nullptr, DGUSScreenVariableHandler::DGUSLCD_SD_SendFilename),
      VPHELPER_STR(
          VP_SD_FileName4, nullptr, VP_SD_FileName_LEN, nullptr, DGUSScreenVariableHandler::DGUSLCD_SD_SendFilename),
      VPHELPER(VP_SD_ResumePauseAbort, nullptr, DGUSScreenVariableHandler::DGUSLCD_SD_ResumePauseAbort, nullptr),
      VPHELPER(VP_SD_AbortPrintConfirmed, nullptr, DGUSScreenVariableHandler::DGUSLCD_SD_ReallyAbort, nullptr),
      VPHELPER(VP_SD_Print_Setting, nullptr, DGUSScreenVariableHandler::DGUSLCD_SD_PrintTune, nullptr),
#if HAS_BED_PROBE
#if ENABLED(BABYSTEPPING)
      VPHELPER(VP_SD_Print_LiveAdjustZ, nullptr, DGUSScreenVariableHandler::HandleLiveAdjustZ, nullptr),
#endif
#endif
#endif

// TODO rubienr - DGUSLCD_SendWaitingStatusToDisplay implementation is missing
#if ENABLED(DGUS_UI_WAITING)
      VPHELPER(dgus::memory_layout::ScreenStatus::Waiting,
               nullptr,
               nullptr,
               &DGUSScreenVariableHandler::DGUSLCD_SendWaitingStatusToDisplay),
#endif

// Messages for the User, shared by the popup and the kill screen.
// They can not be autouploaded as we do not buffer content.
#define MESSAGE_VPHELPER(N)                                                \
  {.VP = to_address(dgus::memory_layout::UiMessages::Message##N),          \
   .memadr = nullptr,                                                      \
   .size = to_uint8_t(dgus::memory_layout::UiMessages::Message##N##Bytes), \
   .set_by_display_handler = nullptr,                                      \
   .send_to_display_handler = &DGUSScreenVariableHandler::DGUSLCD_SendStringToDisplayPGM},
      REPEAT_S(1, 5, MESSAGE_VPHELPER)
#undef MESSAGE_VPHELPER

// power supply
#if ENABLED(DGUS_ORIGIN_PSU_CONTROL)
          VPHELPER(dgus::memory_layout::PowerSupplyUnit::Control,
                   &dgus_origin::psu_control::cached_state,
                   &dgus_origin::psu_control::handle_psu_on_off,
                   &dgus::handler::send_word),
#endif

// light
#if ENABLED(CASE_LIGHT_ENABLE)
      VPHELPER(dgus::memory_layout::CaseLight::Control,
               &dgus_origin::lights::cached_state.case_light_control,
               &dgus_origin::lights::handle_case_light,
               &dgus::handler::send_word),
#endif
#if ENABLED(HAS_COLOR_LEDS)
#define COLOR_LEDS_VPHELPER(N)                                       \
  VPHELPER(dgus::memory_layout::ColorLeds::Control##N,               \
           &dgus_origin::lights::cached_state.color_led_control_##N, \
           &dgus_origin::lights::handle_color_led,                   \
           &dgus::handler::send_word),
      REPEAT(3, COLOR_LEDS_VPHELPER)
#undef COLOR_LEDS_VPHELPER
#endif

// bed leveling
#if ENABLED(AUTO_BED_LEVELING_UBL)
          VPHELPER(dgus::memory_layout::Ubl::RequestFlags,
                   &dgus_origin::bed_leveling_ubl::cached_state.request_flags,
                   &dgus_origin::bed_leveling_ubl::handle_parameter_load_save_probe,
                   &dgus::handler::send_word),
      VPHELPER(dgus::memory_layout::Ubl::FadeHeightSlotNnumber,
               &dgus_origin::bed_leveling_ubl::cached_state.fade_height_slot_number,
               &dgus_origin::bed_leveling_ubl::handle_parameter_fade_slot,
               &dgus::handler::send_word),
      VPHELPER(dgus::memory_layout::Ubl::OnOoffUnused,
               &dgus_origin::bed_leveling_ubl::cached_state.on_off_state,
               &dgus_origin::bed_leveling_ubl::handle_parameter_load_save_probe,
               &dgus::handler::send_word),
#endif

// nozzle to probe offset
#if ENABLED(DGUS_ORIGIN_NOZZLE_OFFSET)
      // TODO rubienr - handle multiple nozzles
      VPHELPER(dgus::memory_layout::OffsetNozzleToProbe::Nozzle0Control,
               &dgus_origin::nozzle_offset::cached_state,
               &dgus_origin::nozzle_offset::handle_probe_offset_request,
               &dgus::handler::send_word),
      VPHELPER(dgus::memory_layout::OffsetNozzleToProbe::Nozzle0X,
               &probe.offset.x,
               &dgus_origin::nozzle_offset::handle_set_probe_offset_axis<ExtUI::X>,
               &DGUSScreenVariableHandler::DGUSLCD_SendFloatAsIntValueToDisplay<2>),
      VPHELPER(dgus::memory_layout::OffsetNozzleToProbe::Nozzle0Y,
               &probe.offset.y,
               &dgus_origin::nozzle_offset::handle_set_probe_offset_axis<ExtUI::Y>,
               &DGUSScreenVariableHandler::DGUSLCD_SendFloatAsIntValueToDisplay<2>),
      VPHELPER(dgus::memory_layout::OffsetNozzleToProbe::Nozzle0Z,
               &probe.offset.z,
               &dgus_origin::nozzle_offset::handle_set_probe_offset_z_axis,
               &DGUSScreenVariableHandler::DGUSLCD_SendFloatAsIntValueToDisplay<2>),
#endif

// filament load/unload screen using native capability via M701/M702
#if ENABLED(DGUS_ORIGIN_FILAMENT_LOAD_UNLOAD)
      VPHELPER(dgus::memory_layout::Filament::LoadUnloadControl,
               &dgus_origin::filament::cached_state.load_unload,
               &dgus_origin::filament::handle_filament_move,
               &dgus::handler::send_word),
      VPHELPER(dgus::memory_layout::Filament::ExtrudeRetractControl,
               &dgus_origin::filament::cached_state.extrude_retract,
               &dgus_origin::filament::handle_filament_move,
               &dgus::handler::send_word),
#endif
      // terminating list with nullptr
      VPHELPER(nullptr, nullptr, nullptr, nullptr)
};

#endif // DGUS_LCD_UI_ORIGIN
