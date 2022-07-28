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
  CUSTOM_KEYCODES_SAFE_RANGE
};

#ifndef APPLE_FN_ENABLE
#  define APPLE_FN KC_RALT
#  define APPLE_FF KC_RALT
#endif

// tap dance actions
enum tap_dance_action_index {
  TD_RALT_MO3,        // for HHKB,     Right Alt, on tap hold: MO(3)
  TD_APFF_EISU_KANA,  // for mac,      Apple Fn/Globe + FK override, on tap: 英数, on double tap かな
  TD_LCMD_EISU_KANA,  // for mac/HHKB, Left Cmd, on tap: 英数, on double tap かな
  TD_LOPT_APFF,       // for mac/HHKB, Left Option, on tap hold: Apple fn/globe + FK overrde
  TD_LWIN_LANG,       // for win/HHKB, Left Win, on double tap: Win + space
  TD_LALT_EISU_KANA,  // for win/HHKB, Left Alt, on tap: 英数, on double tap かな
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
