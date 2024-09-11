/* Copyright 2023 msasafumi
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

#include QMK_KEYBOARD_H

#ifdef EC_DEBUG_ENABLE
#  define _EC_DBG0 EC_DBG0
#  define _EC_DBG1 EC_DBG1
#  define _EC_DBG2 EC_DBG2
#else
#  define _EC_DBG0 _______
#  define _EC_DBG1 _______
#  define _EC_DBG2 _______
#endif

// tap dance
// [single tap, single hold, multi tap, tap hold, tapping term]
const tap_dance_entry_t PROGMEM tap_dance_predefined_entries[NUM_TAP_DANCE_PREDEFINED_ENTRIES] = {
  // TD(0) - LCMD + Spacemacs leader key
  [0] = {A(KC_M), KC_LGUI, KC_LGUI, KC_LGUI, TAPPING_TERM},
  // TD(1) - RCMD + LANG2/LANG1
  [1] = {KC_LNG2, KC_RGUI, KC_LNG1, KC_RGUI, TAPPING_TERM},
  // TD(2) - Protect layer 3 from misstouch, MENU + MO(3)
  [2] = {KC_APP, KC_APP, KC_APP, MO(3), TAPPING_TERM}};

// since QMK 0.26, tap_dance_actions must be defined in keyma.c
tap_dance_action_t tap_dance_actions[TAP_DANCE_ENTRIES];

// clang-format off
const uint16_t PROGMEM ec_bottoming_reading_default[MATRIX_ROWS][MATRIX_COLS] = {
  {0x03ca,0x03f6,0x03e0,0x03fc,0x03fa,0x03f6,0x03e9,0x03c5,0x03c7,0x038f,0x03e7,0x03b7,0x03fb,0x028d,0x0381},
  {0x03db,0x03f5,0x03fd,0x03fc,0x03f1,0x03fc,0x03d7,0x03ab,0x03fd,0x03fa,0x03cf,0x03c0,0x0344,0x02d3,0x03ff},
  {0x0294,0x03fa,0x03fc,0x03fd,0x03c9,0x03fc,0x03ef,0x03fc,0x03fb,0x03eb,0x03c9,0x03db,0x03ff,0x03ac,0x03ff},
  {0x02ba,0x03ff,0x038e,0x03fb,0x03fb,0x03e9,0x0395,0x03fb,0x03fc,0x03e0,0x03f7,0x03c2,0x03ff,0x026f,0x032e},
  {0x02ff,0x0314,0x02a0,0x03ff,0x03ff,0x03ff,0x030c,0x03ff,0x03ff,0x03ff,0x0234,0x0266,0x02b4,0x03ff,0x03ff}
};
const uint16_t PROGMEM ec_noise_floor_default[MATRIX_ROWS][MATRIX_COLS] = {
  {0x0111,0x0148,0x00fb,0x0124,0x0163,0x015d,0x012c,0x010e,0x00bc,0x00ca,0x00ff,0x00d7,0x0114,0x00cd,0x0137},
  {0x00f4,0x00ef,0x0106,0x0147,0x0128,0x0137,0x0163,0x00cb,0x013c,0x012d,0x00fd,0x0137,0x009f,0x00bd,0x0000},
  {0x00e4,0x0107,0x0143,0x013b,0x00f1,0x0184,0x014c,0x011d,0x0168,0x013e,0x015c,0x0132,0x0000,0x009c,0x0000},
  {0x00a2,0x0000,0x011e,0x017d,0x01a6,0x00fe,0x010d,0x0185,0x019b,0x0183,0x0167,0x00ec,0x0000,0x00dd,0x00cc},
  {0x00ee,0x00e7,0x00d0,0x0000,0x0000,0x0000,0x00dc,0x0000,0x0000,0x0000,0x00d9,0x00d9,0x009e,0x0000,0x0000}
};
// clang-format on

// clang-format off
const ec_preset_t PROGMEM ec_presets_default[EC_NUM_PRESETS] = {
  // EC Preset 0 ⬜️ - default
  [0] = EC_STATIC_PRESET_DEFAULT,
  // EC Preset 1 🟩 - alphas
  [1] = EC_DYNAMIC_PRESET_PERC(15, 15, 15),
  // EC Preset 2 🟨 - none-alphas
  [2] = EC_DYNAMIC_PRESET_DEFAULT,
  // EC Preset 3 🟧 - mods
  [3] = EC_STATIC_PRESET_DEFAULT,
  // EC Preset 4 🟥 - little finger home,Ctrl/HHKB fn
  [4] = EC_STATIC_PRESET_DEFAULT,
  // EC Preset 5 🟦 - enter key
  [5] = EC_DYNAMIC_PRESET_PERC(20, 20, 15),
  // EC Preset 6 🟪 - spacebar
  [6] = EC_DYNAMIC_PRESET_PERC(20, 20, 15),
  // EC Preset 7 ⬛️ - prevent mistouch
  [7] = EC_STATIC_PRESET_PERC(60, 30),
};
// clang-format on

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  // mac base layer
  [0] = LAYOUT_60_tsangan(
    KC_GRV,   KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSLS, KC_ESC,
    KC_TAB,   KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC,          KC_BSPC,
    KC_LCTL,  KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,                   KC_ENT,
    KC_LSFT,           KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH,          KC_RSFT, MO(2),
    APPLE_FF, KC_LALT, TD(0),                              KC_SPC,                                               TD(1),   KC_RALT, TD(2)
  ),
  // standard base layer
  [1] = LAYOUT_60_tsangan(
    KC_GRV,   KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSLS, KC_ESC,
    KC_TAB,   KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC,          KC_BSPC,
    KC_LCTL,  KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,                   KC_ENT,
    KC_LSFT,           KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH,          KC_RSFT, MO(2),
    APPLE_FF, KC_LALT, TD(0),                              KC_SPC,                                               TD(1),   KC_RALT, TD(2)
  ),
  // HHKB-like fn layer
  [2] = LAYOUT_60_tsangan(
    _______, KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  KC_INS,  KC_DEL,
    KC_CAPS, KC_F16,  KC_F17,  KC_F18,  _______, _______, _______, _______, KC_PSCR, KC_SCRL, KC_PAUS, KC_UP,   _______,          _______,
    _______, KC_VOLD, KC_VOLU, KC_MUTE, KC_EJCT, _______, KC_PAST, KC_PSLS, KC_HOME, KC_PGUP, KC_LEFT, KC_RGHT,                   KC_PENT,
    _______,          _______, _______, _______, _______, _______, KC_PPLS, KC_PMNS, KC_END,  KC_PGDN, KC_DOWN,          _______, _______,
    _______, _______, _______,                            _______,                                              _______, _______, _______
  ),
  // settings, application-specific keys
  // row 1 - EC data
  // row 2 - Select EC Prrest Map
  // row 3: normal settings, +shift key for unusual reversed setting
  [3] = LAYOUT_60_tsangan(
    QK_BOOT, KC_F13,  KC_F14,  KC_F15,  KC_F16,  KC_F17,  KC_F18,  KC_F19,  KC_F20,  KC_F21,  KC_F22,  KC_F23,  KC_F24,  _______, TERM_LCK,
    _______, EC_CALD, EC_PSET, EC_SCAL,_EC_DBG0,_EC_DBG1,_EC_DBG2,_______, _______, _______, _______, _______, TEST_OS,          _______,
    _______, ECM(0),  ECM(1),  ECM(2),  ECM(3),  _______, _______, _______, _______, _______, _______, _______,                   _______,
    _______,          RHID_OFF,AUT_ON,  MAC_ON,  USJ_OFF, NK_ON,   CL_NORM, AG_NORM, BS_NORM, _______, _______,          _______, _______,
    _______, _______, _______,                            _______,                                              _______, _______, _______
  ),

  // EC Preset map 0
  // EC(0) default
  // EC(1) alphas
  // EC(2) none-alphas
  // EC(3) mods
  // EC(4) little finger home Ctrl/hhkb fn
  // EC(5) enter
  // EC(6) spacebar
  // EC(7) prevent mistouch
  [EC_PRESET_MAP(0)] = LAYOUT_60_tsangan(
    EC(1), EC(1), EC(1), EC(1), EC(1), EC(1), EC(1), EC(1), EC(1), EC(1), EC(1), EC(1), EC(1), EC(1), EC(2),
    EC(2), EC(1), EC(1), EC(1), EC(1), EC(1), EC(1), EC(1), EC(1), EC(1), EC(1), EC(1), EC(1),        EC(2),
    EC(4), EC(1), EC(1), EC(1), EC(1), EC(1), EC(1), EC(1), EC(1), EC(1), EC(1), EC(1),               EC(5),
    EC(3),        EC(1), EC(1), EC(1), EC(1), EC(1), EC(1), EC(1), EC(1), EC(1), EC(1),        EC(3), EC(4),
    EC(7), EC(3), EC(3),                      EC(6),                                    EC(3), EC(3), EC(7)
  ),

  // EC Preset map 1
  [EC_PRESET_MAP(1)] = LAYOUT_60_tsangan(
    EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0),
    EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0),        EC(0),
    EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0),               EC(0),
    EC(0),        EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0),        EC(0), EC(0),
    EC(0), EC(0), EC(0),                      EC(0),                                    EC(0), EC(0), EC(0)
  ),

  // EC Preset map 2
  [EC_PRESET_MAP(2)] = LAYOUT_60_tsangan(
    EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0),
    EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0),        EC(0),
    EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0),               EC(0),
    EC(0),        EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0),        EC(0), EC(0),
    EC(0), EC(0), EC(0),                      EC(0),                                    EC(0), EC(0), EC(0)
  ),

  // EC Preset map 3
  [EC_PRESET_MAP(3)] = LAYOUT_60_tsangan(
    EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0),
    EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0),        EC(0),
    EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0),               EC(0),
    EC(0),        EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0), EC(0),        EC(0), EC(0),
    EC(0), EC(0), EC(0),                      EC(0),                                    EC(0), EC(0), EC(0)
  )
};
// clang-format on
