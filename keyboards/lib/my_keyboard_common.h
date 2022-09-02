/* Copyright 2022 Msafumi
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once

#include "quantum.h"
#include "via.h"
#ifdef VIAL_ENABLE
#  include "vial.h"
#endif

//  constants
//------------------------------------------

enum custom_user_keycodes {
  MAC_TOGG = USER00,  // Toggle enabling fake apple mode with switching base layer 0(apple mode) or 1.
  MAC_ON,             // Enable fake apple mode with switching base layer 0.
  MAC_OFF,            // Disable fake apple mode with switching base layer 1.
  APPLE_FN,           // Apple fn/globe key
  APPLE_FF,           // Apple fn/globe with remapping F1-12
  EJ_TOGG,            // Toggle send 英数 and かな
  USJ_TOGG,           // T oggle enabling conversion for ANSI layout on JIS
  USJ_ON,             // Enable conversion for ANSI layout on JIS.
  USJ_OFF,            // Disable conversion for ANSI layout on JIS.
#ifdef RADIAL_CONTROLLER_ENABLE
  RADIAL_BUTTON,         // State of the button located on radial controller
  RADIAL_LEFT,           // Relative counter clock wise rotation of the radial controller
  RADIAL_RIGHT,          // Relative clock wise rotation of the radial controller
  RADIAL_LEFT_CONTINUE,  // Continuous counter clock wise rotation of the radial controller
  RADIAL_RIGHT_CONINUE,  // Continuous clock wise rotation of the radial controller
#endif
  CUSTOM_KEYCODES_SAFE_RANGE
};

#ifndef APPLE_FN_ENABLE
#  define APPLE_FN KC_RALT
#  define APPLE_FF KC_RALT
#endif

#ifdef RADIAL_CONTROLLER_ENABLE
#  define DIAL_BUT RADIAL_BUTTON
#  define DIAL_L RADIAL_LEFT
#  define DIAL_R RADIAL_RIGHT
#  define DIAL_LC RADIAL_LEFT_CONTINUE
#  define DIAL_RC RADIAL_RIGHT_CONINUE
#endif

// tap dance actions
enum tap_dance_action_index {
  TD_RALT_MO3,             // ight Alt, on tap hold: MO(3)
  TD_APFF_EISU_KANA,       // Apple Fn/Globe + FK override, on double tap: toggle send 英数/かな
  TD_LGUI_EISU_KANA,       // Left GUI, on tap: KC_LNG2(英数), on double tap: KC_LNG1(かな)
  TD_LALT_EISU_KANA,       // Left Alt, on tap: KC_LNG2(英数), on double tap: KC_LNG1(かな)
  TD_LALT_APFF,            // Left Alt, on tap hold: Apple fn/globe + FK overrde
  TD_LALT_APFF_EISU_KANA,  // Left Alt, on tap hold: Apple fn/globe + FK overrde on tap: KC_LNG2(英数), on double tap:
                           // KC_LNG1(かな)
  TAP_DANCE_ACTIONS_DEFAULT_LENGTH
};

#ifdef VIAL_ENABLE

#  ifdef VIAL_COMBO_ENABLE
// combo actions
enum vial_combo_action_index {
  // CB_TEST = 0,  // TEST dummy data,
  VIAL_COMBO_ACTIONS_DEFAULT_LENGTH = 0
};
#  endif

#  ifdef VIAL_KEY_OVERRIDE_ENABLE
enum vial_key_override_action_index {
  // KO_TEST = 0,  // TEST dummy data,
  VIAL_KEY_OVERRIDE_ACTIONS_DEFAULT_LENGTH = 0
};
#  endif
#endif

//  type definitions
//------------------------------------------

typedef union {
  uint32_t raw;
  struct {
    bool mac : 1;  // mac mode.
    bool usj : 1;  // ANSI layou on JIS.
  };
} common_kb_config_t;

//   global variavles
//------------------------------------------

extern common_kb_config_t g_common_kb_config;
