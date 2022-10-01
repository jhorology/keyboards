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

#include "custom_keycodes.h"
#include "quantum.h"

// tap dance actions
enum tap_dance_action_index {
  TD_RALT_MO3,             // ight Alt, on tap hold: MO(3)
  TD_APFF_EISU_KANA,       // Apple Fn/Globe + FK override, on double tap: toggle send 英数/かな
  TD_LGUI_EISU_KANA,       // Left GUI, on tap: KC_LNG2(英数), on double tap: KC_LNG1(かな)
  TD_LALT_EISU_KANA,       // Left Alt, on tap: KC_LNG2(英数), on double tap: KC_LNG1(かな)
  TD_LALT_APFF,            // Left Alt, on tap hold: Apple fn/globe + FK overrde
  TD_LALT_APFF_EISU_KANA,  // Left Alt, on tap hold: Apple fn/globe + FK overrde on tap: KC_LNG2(英数), on double tap:
                           // KC_LNG1(かな)
  TAP_DANCE_PRE_DEFINED_LENGTH
};

#ifdef VIAL_ENABLE

#  ifdef VIAL_COMBO_ENABLE
// combo actions
enum vial_combo_action_index {
  // CB_TEST = 0,  // TEST dummy data,
  COMBO_PRE_DEFINED_LENGTH = 0
};
#  endif

#  ifdef VIAL_KEY_OVERRIDE_ENABLE
enum vial_key_override_action_index {
  // KO_TEST = 0,  // TEST dummy data,
  KEY_OVERRIDE_PRE_DEFINED_LENGTH = 0
};
#  endif
#endif
