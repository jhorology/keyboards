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
  APPLE_FN = USER00,  // Apple fn/globe key
  APPLE_FF,           // Apple fn/globe wotb remapping F1-12
  DD_TOGG,            // Toggle USB device descriptor default or alternate.
  DD_NRML,            // Use default USB device descriptor.
  DD_ALT,             // Use alternate USB device descriptor.
  EJ_TOGG,            // to ggle send 英数 and /かな
  USJ_TOGG,           // toggle enabling conversion for ANSI layout on JIS
  USJ_ON,             // enable conversion for ANSI layout on JIS.
  USJ_OFF,            // disable conversion for ANSI layout on JIS.
  CUSTOM_KEYCODES_SAFE_RANGE
};

#ifndef APPLE_FN_ENABLE
#  define APPLE_FN KC_RALT
#  define APPLE_FF KC_RALT
#endif

// tap dance actions
enum tap_dance_action_index {
  TD_LALT_IME = 0,    // LALT, on tap: alt + ~
  TD_LGUI_EISU,       // LGUI, on tap: mac 英数
  TD_RGUI_KANA,       // LGUI, on tap: mac かな
  TD_LGUI_EISU_KANA,  // LGUI, on tap: mac toggle 英数/かな
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
    union {
      uint16_t common;
      struct {
        bool usb_alternate : 1;  // enable alternate USB device descriptor.
        bool usj_enabled : 1;    // ANSI->JIS convertion enabled state.
      };
    };
    uint16_t kb;
  };
} user_config_t;

//   global variavles
//------------------------------------------

extern user_config_t g_user_config;

//   keyboard-spcific hook functions
//------------------------------------------

extern void init_with_config_user_kb(void);
extern bool process_record_user_kb(uint16_t keycode, keyrecord_t *record);
