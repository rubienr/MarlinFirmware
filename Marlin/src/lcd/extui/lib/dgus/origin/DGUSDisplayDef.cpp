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

const struct VPMapping VPMap[] PROGMEM = {
    {DGUSLCD_SCREEN_BOOT, dgus_origin::boot::VPScreenList},
    {DGUSLCD_SCREEN_MAIN, dgus_origin::main::VPScreenList},
#if ENABLED(DGUS_ORIGIN_TEMPERATURES)
    {DGUSLCD_SCREEN_TEMPERATURE, dgus_origin::temperatures::VPScreenList},
#endif
#if ENABLED(DGUS_ORIGIN_STATUS)
    {DGUSLCD_SCREEN_STATUS, dgus_origin::status::VPScreenList1},
    {DGUSLCD_SCREEN_STATUS2, dgus_origin::status::VPScreenList2},
#endif
#if ENABLED(DGUS_ORIGIN_MANUAL_MOVE)
    {DGUSLCD_SCREEN_MANUALMOVE, dgus_origin::manual_move::VPScreenList},
#endif
#if ENABLED(DGUS_ORIGIN_MANUAL_EXTRUDE)
    {DGUSLCD_SCREEN_MANUALEXTRUDE, dgus_origin::manual_extrude::VPScreenList},
#endif
#if ENABLED(DGUS_ORIGIN_FEEDRATES)
    {DGUSLCD_SCREEN_FANANDFEEDRATE, dgus_origin::feedrates::VPScreenList},
#endif
#if ENABLED(DGUS_ORIGIN_FLOWRATES)
    {DGUSLCD_SCREEN_FLOWRATES_1, dgus_origin::flowrates::VPScreenList1},
    {DGUSLCD_SCREEN_FLOWRATES_2, dgus_origin::flowrates::VPScreenList2},
#endif
#if ENABLED(DGUS_ORIGIN_SDPRINT_MANIPULATION)
    {DGUSLCD_SCREEN_SDPRINTMANIPULATION, dgus_origin::sdprint_manipulation::VPScreenList},
#endif
#if ENABLED(DGUS_ORIGIN_INFO)
    {DGUSLCD_SCREEN_INFO, dgus_origin::info::VPScreenList},
#endif
#if ENABLED(DGUS_ORIGIN_TOOLS)
    {DGUSLCD_SCREEN_TOOLS, dgus_origin::tools::VPScreenList},
#endif
#if ENABLED(DGUS_ORIGIN_PSU_CONTROL)
    {DGUSLCD_SCREEN_PSU, dgus_origin::psu_control::VPScreenList},
#endif
#if ENABLED(AUTO_BED_LEVELING_UBL)
    {DGUSLCD_SCREEN_BED_LEVELING, dgus_origin::bed_leveling_ubl::screen_variables},
#endif
#if ENABLED(DGUS_ORIGIN_DRIVER_CONTROL)
    {DGUSLCD_SCREEN_MOTORS, dgus_origin::driver_control::VPScreenList},
#endif
#if ANY(DGUS_ORIGIN_LIGHTS)
    {DGUSLCD_SCREEN_LIGHTS, dgus_origin::filament::VPScreenList},
#endif
#if ENABLED(HAS_BED_PROBE)
    {DGUSLCD_SCREEN_PROBE_OFFSET, dgus_origin::nozzle_offset::VPScreenList},
#endif
#if ENABLED(EEPROM_SETTINGS)
    {DGUSLCD_SCREEN_EEPROM, dgus_origin::eeprom::VPScreenList},
#endif
#if ENABLED(DGUS_ORIGIN_FILAMENT_LOAD_UNLOAD)
    {DGUSLCD_SCREEN_FILAMENT_LOAD_UNLOAD, dgus_origin::filament::VPScreenList},
#endif
#if ENABLED(DGUS_ORIGIN_SDFILES)
    {DGUSLCD_SCREEN_SDFILELIST, dgus_origin::sdfiles::VPScreenList},
#endif
    {0, nullptr} // List is terminated with an nullptr as table entry.
};

const struct DGUS_VP_Variable ListOfVP[] PROGMEM = {
    // Helper to detect touch events
    VPHELPER(dgus::memory_layout::ScreenAction::ScreenChange,
             nullptr,
             DGUSScreenVariableHandler::ScreenChangeHook,
             nullptr),
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

#if ENABLED(DGUS_UI_MOVE_DIS_OPTION)
    VPHELPER(VP_MOVE_OPTION, &distanceToMove, &DGUSScreenVariableHandler::HandleManualMoveOption, nullptr),
#endif
#if ENABLED(DGUS_UI_MOVE_DIS_OPTION)
    VPHELPER(VP_MOVE_X, &distanceToMove, &DGUSScreenVariableHandler::HandleManualMove, nullptr),
    VPHELPER(VP_MOVE_Y, &distanceToMove, &DGUSScreenVariableHandler::HandleManualMove, nullptr),
    VPHELPER(VP_MOVE_Z, &distanceToMove, &DGUSScreenVariableHandler::HandleManualMove, nullptr),
    VPHELPER(VP_HOME_ALL, &distanceToMove, &DGUSScreenVariableHandler::HandleManualMove, nullptr),
#else
    VPHELPER(dgus::memory_layout::Position::X, nullptr, &DGUSScreenVariableHandler::HandleManualMove, nullptr),
    VPHELPER(dgus::memory_layout::Position::Y, nullptr, &DGUSScreenVariableHandler::HandleManualMove, nullptr),
    VPHELPER(dgus::memory_layout::Position::Z, nullptr, &DGUSScreenVariableHandler::HandleManualMove, nullptr),
    VPHELPER(dgus::memory_layout::Homing::Control, nullptr, &DGUSScreenVariableHandler::HandleManualMove, nullptr),
#endif

    VPHELPER(dgus::memory_layout::DriverControl::LockUnlockControl,
             &dgus_origin::driver_control::cached_state,
             &dgus_origin::driver_control::handle_motor_lock_unlock,
             &DGUSScreenVariableHandler::DGUSLCD_SendWordValueToDisplay),
#if ENABLED(POWER_LOSS_RECOVERY)
    VPHELPER(VP_POWER_LOSS_RECOVERY, nullptr, &DGUSScreenVariableHandler::HandlePowerLossRecovery, nullptr),
#endif

#if ENABLED(EEPROM_SETTINGS)
    VPHELPER(dgus::memory_layout::Eeprom::Control, nullptr, &dgus_origin::eeprom::handle_settings, nullptr),
#endif

    // Boot screen
    {.VP = to_address(dgus::memory_layout::UiVersion::MajorMinor),
     .memadr = const_cast<uint16_t *>(&dgus_origin::versions::ui_version.low_word),
     .size = 2,
     .set_by_display_handler = nullptr,
     .send_to_display_handler = &DGUSScreenVariableHandler::DGUSLCD_SendWordValueToDisplay},
    {.VP = to_address(dgus::memory_layout::UiVersion::Patch),
     .memadr = const_cast<uint16_t *>(&dgus_origin::versions::ui_version.high_word),
     .size = 2,
     .set_by_display_handler = nullptr,
     .send_to_display_handler = &DGUSScreenVariableHandler::DGUSLCD_SendWordValueToDisplay},
    {.VP = to_address(dgus::memory_layout::UiVersion::Flavour),
     .memadr = const_cast<dgus_origin::versions::UiFlavor *>(&dgus_origin::versions::ui_flavor),
     .size = to_uint8_t(dgus::memory_layout::UiVersion::FlavourBytes),
     .set_by_display_handler = nullptr,
     .send_to_display_handler = &DGUSScreenVariableHandler::DGUSLCD_SendStringToDisplayPGM},

    // Boot and info screen
    {.VP = to_address(dgus::memory_layout::MarlinVersion::Version),
     .memadr = (void *)dgus_origin::versions::marlin_version::MarlinVersion,
     .size = constexpr_strlen(dgus_origin::versions::marlin_version::MarlinVersion),
     .set_by_display_handler = nullptr,
     .send_to_display_handler = &DGUSScreenVariableHandler::DGUSLCD_SendStringToDisplayPGM},

    // Info screen
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

// manual extrude
#if HOTENDS >= 1
    VPHELPER(dgus::memory_layout::PositionE::E0,
             nullptr,
             &DGUSScreenVariableHandler::HandleManualExtrude,
             nullptr),
    VPHELPER(
        dgus::memory_layout::MoveE::E0, nullptr, &DGUSScreenVariableHandler::HandleManualExtrude, nullptr),
#endif
#if HOTENDS >= 2
    VPHELPER(dgus::memory_layout::PoseitionE::E1,
             nullptr,
             &DGUSScreenVariableHandler::HandleManualExtrude,
             nullptr),
    VPHELPER(
        dgus::memory_layout::MoveE::E1, nullptr, &DGUSScreenVariableHandler::HandleManualExtrude, nullptr),
#endif
#if HOTENDS >= 3
    VPHELPER(dgus::memory_layout::PoseitionE::E2,
             nullptr,
             &DGUSScreenVariableHandler::HandleManualExtrude,
             nullptr),
    VPHELPER(
        dgus::memory_layout::MoveE::E2, nullptr, &DGUSScreenVariableHandler::HandleManualExtrude, nullptr),
#endif
#if HOTENDS >= 4
    VPHELPER(dgus::memory_layout::PoseitionE::E3,
             nullptr,
             &DGUSScreenVariableHandler::HandleManualExtrude,
             nullptr),
    VPHELPER(
        dgus::memory_layout::MoveE::E3, nullptr, &DGUSScreenVariableHandler::HandleManualExtrude, nullptr),
#endif
#if HOTENDS >= 5
    VPHELPER(dgus::memory_layout::PoseitionE::E4,
             nullptr,
             &DGUSScreenVariableHandler::HandleManualExtrude,
             nullptr),
    VPHELPER(
        dgus::memory_layout::MoveE::E4, nullptr, &DGUSScreenVariableHandler::HandleManualExtrude, nullptr),
#endif
#if HOTENDS >= 6
    VPHELPER(dgus::memory_layout::PoseitionE::E5,
             nullptr,
             &DGUSScreenVariableHandler::HandleManualExtrude,
             nullptr),
    VPHELPER(
        dgus::memory_layout::MoveE::E5, nullptr, &DGUSScreenVariableHandler::HandleManualExtrude, nullptr),
#endif

// Temperature Data
#if HOTENDS >= 1
    VPHELPER(dgus::memory_layout::Temperatures::E0Is,
             &thermalManager.temp_hotend[0].celsius,
             nullptr,
             DGUSScreenVariableHandler::DGUSLCD_SendFloatAsLongValueToDisplay<0>),
    VPHELPER(dgus::memory_layout::Temperatures::E0Set,
             &thermalManager.temp_hotend[0].target,
             DGUSScreenVariableHandler::HandleTemperatureChanged,
             &DGUSScreenVariableHandler::DGUSLCD_SendWordValueToDisplay),
    VPHELPER(dgus::memory_layout::Flowrates::E0,
             nullptr,
             DGUSScreenVariableHandler::HandleFlowRateChanged,
             &DGUSScreenVariableHandler::DGUSLCD_SendWordValueToDisplay),
    VPHELPER(dgus::memory_layout::PositionE::E0,
             &destination.e,
             nullptr,
             DGUSScreenVariableHandler::DGUSLCD_SendFloatAsLongValueToDisplay<2>),
    VPHELPER(dgus::memory_layout::HeaterControl::E0,
             &thermalManager.temp_hotend[0].target,
             &DGUSScreenVariableHandler::HandleHeaterControl,
             nullptr),
    VPHELPER(dgus::memory_layout::HeaterStatus::E0,
             &thermalManager.temp_hotend[0].target,
             nullptr,
             &DGUSScreenVariableHandler::DGUSLCD_SendHeaterStatusToDisplay),
// TODO rubienr - fix define
#if ENABLED(DGUS_PREHEAT_UI)
    VPHELPER(VP_E0_BED_PREHEAT, nullptr, &DGUSScreenVariableHandler::HandlePreheat, nullptr),
#endif
#if ENABLED(PIDTEMP)
    VPHELPER(dgus::memory_layout::Pid::E0P,
             &thermalManager.temp_hotend[0].pid.Kp,
             DGUSScreenVariableHandler::HandleTemperaturePIDChanged,
             DGUSScreenVariableHandler::DGUSLCD_SendTemperaturePID),
    VPHELPER(dgus::memory_layout::Pid::E0I,
             &thermalManager.temp_hotend[0].pid.Ki,
             DGUSScreenVariableHandler::HandleTemperaturePIDChanged,
             DGUSScreenVariableHandler::DGUSLCD_SendTemperaturePID),
    VPHELPER(dgus::memory_layout::Pid::E0D,
             &thermalManager.temp_hotend[0].pid.Kd,
             DGUSScreenVariableHandler::HandleTemperaturePIDChanged,
             DGUSScreenVariableHandler::DGUSLCD_SendTemperaturePID),
    VPHELPER(dgus::memory_layout::PidAutotune::E0,
             nullptr,
             &DGUSScreenVariableHandler::HandlePIDAutotune,
             nullptr),
#endif

#endif

// TODO rubienr
#if HOTENDS >= 2
    VPHELPER(dgus::memory_layout::Temperatures::E1Is,
             &thermalManager.temp_hotend[1].celsius,
             nullptr,
             DGUSLCD_SendFloatAsLongValueToDisplay<0>),
    VPHELPER(dgus::memory_layout::Temperatures::E1Set,
             &thermalManager.temp_hotend[1].target,
             DGUSScreenVariableHandler::HandleTemperatureChanged,
             &DGUSScreenVariableHandler::DGUSLCD_SendWordValueToDisplay),
    VPHELPER(VP_Flowrate_E1,
             nullptr,
             DGUSScreenVariableHandler::HandleFlowRateChanged,
             &DGUSScreenVariableHandler::DGUSLCD_SendWordValueToDisplay),

    VPHELPER(
        VP_E1_CONTROL, &thermalManager.temp_hotend[1].target, &DGUSScreenVariableHandler::HandleHeaterControl, nullptr),
    VPHELPER(VP_E1_STATUS,
             &thermalManager.temp_hotend[1].target,
             nullptr,
             &DGUSScreenVariableHandler::DGUSLCD_SendHeaterStatusToDisplay),
#if ENABLED(PIDTEMP)
    VPHELPER(dgus::memory_layout::Pid::E1P,
             &thermalManager.temp_hotend[0].pid.Kp,
             DGUSScreenVariableHandler::HandleTemperaturePIDChanged,
             DGUSScreenVariableHandler::DGUSLCD_SendTemperaturePID),
    VPHELPER(dgus::memory_layout::Pid::E1I,
             &thermalManager.temp_hotend[0].pid.Ki,
             DGUSScreenVariableHandler::HandleTemperaturePIDChanged,
             DGUSScreenVariableHandler::DGUSLCD_SendTemperaturePID),
    VPHELPER(dgus::memory_layout::Pid::E1D,
             &thermalManager.temp_hotend[0].pid.Kd,
             DGUSScreenVariableHandler::HandleTemperaturePIDChanged,
             DGUSScreenVariableHandler::DGUSLCD_SendTemperaturePID),
    VPHELPER(dgus::memory_layout::PidAutotune::E1,
             nullptr,
             &DGUSScreenVariableHandler::HandlePIDAutotune,
             nullptr),
#endif
#endif
// TODO rubienr: missing hotend 3-5 temp handling
// TODO rubienr: consolidate all #if HOTENDS != N sections
#if HAS_HEATED_BED
    VPHELPER(dgus::memory_layout::Temperatures::BedIs,
             &thermalManager.temp_bed.celsius,
             nullptr,
             DGUSScreenVariableHandler::DGUSLCD_SendFloatAsLongValueToDisplay<0>),
    VPHELPER(dgus::memory_layout::Temperatures::BedSet,
             &thermalManager.temp_bed.target,
             DGUSScreenVariableHandler::HandleTemperatureChanged,
             &DGUSScreenVariableHandler::DGUSLCD_SendWordValueToDisplay),
    VPHELPER(dgus::memory_layout::HeaterControl::Bed,
             &thermalManager.temp_bed.target,
             &DGUSScreenVariableHandler::HandleHeaterControl,
             nullptr),
    VPHELPER(dgus::memory_layout::HeaterStatus::Bed,
             &thermalManager.temp_bed.target,
             nullptr,
             &DGUSScreenVariableHandler::DGUSLCD_SendHeaterStatusToDisplay),
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

// Fan Data
#if FAN_COUNT
#define FAN_VPHELPER(N)                                                  \
  VPHELPER(dgus::memory_layout::FanSpeed::Fan##N##Percentage,            \
           &thermalManager.fan_speed[N],                                 \
           DGUSScreenVariableHandler::DGUSLCD_PercentageToUint8,         \
           &DGUSScreenVariableHandler::DGUSLCD_SendPercentageToDisplay), \
      VPHELPER(dgus::memory_layout::FanControl::Fan##N,                  \
               &thermalManager.fan_speed[N],                             \
               &DGUSScreenVariableHandler::HandleFanControl,             \
               nullptr),                                                 \
      VPHELPER(dgus::memory_layout::FanStatus::Fan##N,                   \
               &thermalManager.fan_speed[N],                             \
               nullptr,                                                  \
               &DGUSScreenVariableHandler::DGUSLCD_SendFanStatusToDisplay),
    REPEAT(FAN_COUNT, FAN_VPHELPER)
#endif

    // feed rate
    VPHELPER(dgus::memory_layout::FeedRate::Percentage,
             &feedrate_percentage,
             DGUSScreenVariableHandler::DGUSLCD_SetValueDirectly<int16_t>,
             &DGUSScreenVariableHandler::DGUSLCD_SendWordValueToDisplay),

    // position data
    VPHELPER(dgus::memory_layout::Position::X,
             &current_position.x,
             nullptr,
             DGUSScreenVariableHandler::DGUSLCD_SendFloatAsLongValueToDisplay<2>),
    VPHELPER(dgus::memory_layout::Position::Y,
             &current_position.y,
             nullptr,
             DGUSScreenVariableHandler::DGUSLCD_SendFloatAsLongValueToDisplay<2>),
    VPHELPER(dgus::memory_layout::Position::Z,
             &current_position.z,
             nullptr,
             DGUSScreenVariableHandler::DGUSLCD_SendFloatAsLongValueToDisplay<2>),

    // print progress
    VPHELPER(
        dgus::memory_layout::PrintStats::PrintProgressPercentage, nullptr, nullptr, DGUSScreenVariableHandler::DGUSLCD_SendPrintProgressToDisplay),

    // Print Time
    VPHELPER_STR(
        dgus::memory_layout::PrintStats::PrintTime, nullptr, to_uint8_t(dgus::memory_layout::PrintStats::PrintTimeBytes), nullptr, DGUSScreenVariableHandler::DGUSLCD_SendPrintTimeToDisplay),
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

    // todo rubienr: on which screen is VP_N_STEP_PER_MM used?
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
#if HOTENDS >= 1
    VPHELPER(dgus::memory_layout::StepsPerMm::E0,
             &planner.settings.axis_steps_per_mm[E_AXIS_N(0)],
             DGUSScreenVariableHandler::HandleStepPerMMExtruderChanged,
             DGUSScreenVariableHandler::DGUSLCD_SendFloatAsIntValueToDisplay<1>),
#endif
#if HOTENDS >= 2
    VPHELPER(dgus::memory_layout::StepsPerMm::E1,
             &planner.settings.axis_steps_per_mm[E_AXIS_N(1)],
             DGUSScreenVariableHandler::HandleStepPerMMExtruderChanged,
             DGUSScreenVariableHandler::DGUSLCD_SendFloatAsIntValueToDisplay<1>),
#endif

// SDCard File listing.
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

// TODO rubienr DGUSLCD_SendWaitingStatusToDisplay is missing
#if ENABLED(DGUS_UI_WAITING)
    VPHELPER(dgus::memory_layout::ScreenStatus::Waiting, nullptr, nullptr, &DGUSScreenVariableHandler::DGUSLCD_SendWaitingStatusToDisplay),
#endif

    // Messages for the User, shared by the popup and the kill screen. They cant
    // be autouploaded as we do not buffer content.
    {.VP = to_address(dgus::memory_layout::UiMessages::Message1),
     .memadr = nullptr,
     .size = to_uint8_t(dgus::memory_layout::UiMessages::Message1Bytes),
     .set_by_display_handler = nullptr,
     .send_to_display_handler = &DGUSScreenVariableHandler::DGUSLCD_SendStringToDisplayPGM},
    {.VP = to_address(dgus::memory_layout::UiMessages::Message2),
     .memadr = nullptr,
     .size = to_uint8_t(dgus::memory_layout::UiMessages::Message2Bytes),
     .set_by_display_handler = nullptr,
     .send_to_display_handler = &DGUSScreenVariableHandler::DGUSLCD_SendStringToDisplayPGM},
    {.VP = to_address(dgus::memory_layout::UiMessages::Message3),
     .memadr = nullptr,
     .size = to_uint8_t(dgus::memory_layout::UiMessages::Message3Bytes),
     .set_by_display_handler = nullptr,
     .send_to_display_handler = &DGUSScreenVariableHandler::DGUSLCD_SendStringToDisplayPGM},
    {.VP = to_address(dgus::memory_layout::UiMessages::Message4),
     .memadr = nullptr,
     .size = to_uint8_t(dgus::memory_layout::UiMessages::Message4Bytes),
     .set_by_display_handler = nullptr,
     .send_to_display_handler = &DGUSScreenVariableHandler::DGUSLCD_SendStringToDisplayPGM},

// power supply
#if ENABLED(DGUS_ORIGIN_PSU_CONTROL)
    VPHELPER(dgus::memory_layout::PowerSupplyUnit::Control,
             &dgus_origin::psu_control::cached_state,
             &dgus_origin::psu_control::handle_psu_on_off,
             &DGUSScreenVariableHandler::DGUSLCD_SendWordValueToDisplay),
#endif

// light
#if ENABLED(CASE_LIGHT_ENABLE)
    VPHELPER(dgus::memory_layout::CaseLight::Control,
             &dgus_origin::lights::cached_state.case_light_control,
             &dgus_origin::lights::handle_case_light,
             &DGUSScreenVariableHandler::DGUSLCD_SendWordValueToDisplay),
#endif
#if ENABLED(HAS_COLOR_LEDS)
    VPHELPER(dgus::memory_layout::ColorLeds::Control0,
             &dgus_origin::lights::cached_state.color_led_control_0,
             &dgus_origin::lights::handle_color_led_update,
             &DGUSScreenVariableHandler::DGUSLCD_SendWordValueToDisplay),
    VPHELPER(dgus::memory_layout::ColorLeds::Control1,
             &dgus_origin::lights::cached_state.color_led_control_1,
             &dgus_origin::lights::handle_color_led_update,
             &DGUSScreenVariableHandler::DGUSLCD_SendWordValueToDisplay),
    VPHELPER(dgus::memory_layout::ColorLeds::Control2,
             &dgus_origin::lights::cached_state.color_led_control_2,
             &dgus_origin::lights::handle_color_led_update,
             &DGUSScreenVariableHandler::DGUSLCD_SendWordValueToDisplay),
#endif

// bed leveling
#if ENABLED(AUTO_BED_LEVELING_UBL)
    VPHELPER(dgus::memory_layout::Ubl::RequestFlags,
             &dgus_origin::bed_leveling_ubl::cached_state.request_flags,
             &dgus_origin::bed_leveling_ubl::handle_parameter_save,
             &DGUSScreenVariableHandler::DGUSLCD_SendWordValueToDisplay),
    VPHELPER(dgus::memory_layout::Ubl::FadeHeightSlotNnumber,
             &dgus_origin::bed_leveling_ubl::cached_state.fade_height_slot_number,
             &dgus_origin::bed_leveling_ubl::handle_parameter_fade_slot,
             &DGUSScreenVariableHandler::DGUSLCD_SendWordValueToDisplay),
    VPHELPER(dgus::memory_layout::Ubl::OnOoffUnused,
             &dgus_origin::bed_leveling_ubl::cached_state.on_off,
             &dgus_origin::bed_leveling_ubl::handle_parameter_save,
             &DGUSScreenVariableHandler::DGUSLCD_SendWordValueToDisplay),
#endif

// nozzle to probe offset
#if ENABLED(DGUS_ORIGIN_NOZZLE_OFFSET)

    VPHELPER(dgus::memory_layout::OffsetNozzleToProbe::Nozzle0Control,
             &dgus_origin::nozzle_offset::cached_state,
             &dgus_origin::nozzle_offset::handle_probe_offset,
             &DGUSScreenVariableHandler::DGUSLCD_SendWordValueToDisplay),
    VPHELPER(dgus::memory_layout::OffsetNozzleToProbe::Nozzle0X,
             &probe.offset.x,
             &dgus_origin::nozzle_offset::handle_probe_offset_axis<ExtUI::X>,
             &DGUSScreenVariableHandler::DGUSLCD_SendFloatAsIntValueToDisplay<2>),
    VPHELPER(dgus::memory_layout::OffsetNozzleToProbe::Nozzle0Y,
             &probe.offset.y,
             &dgus_origin::nozzle_offset::handle_probe_offset_axis<ExtUI::Y>,
             &DGUSScreenVariableHandler::DGUSLCD_SendFloatAsIntValueToDisplay<2>),
    VPHELPER(dgus::memory_layout::OffsetNozzleToProbe::Nozzle0Z,
             &probe.offset.z,
             &dgus_origin::nozzle_offset::handle_probe_offset_z_axis,
             &DGUSScreenVariableHandler::DGUSLCD_SendFloatAsIntValueToDisplay<2>),
#endif

// Filament load/unload screen using native capability via M701/M702
#if ENABLED(DGUS_ORIGIN_FILAMENT_LOAD_UNLOAD)
    VPHELPER(dgus::memory_layout::Filament::LoadUnloadControl,
             &dgus_origin::filament::cached_state,
             &dgus_origin::filament::handle_filament_load_unload,
             &DGUSScreenVariableHandler::DGUSLCD_SendWordValueToDisplay),
#endif
    // terminating list with nullptr
    VPHELPER(nullptr, nullptr, nullptr, nullptr)
};

#endif // DGUS_LCD_UI_ORIGIN
