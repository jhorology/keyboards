/* Copyright 2020 jhorology
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

// tap dance
// [single tap, single hold, multi tap, tap hold, tapping term]
const tap_dance_entry_t PROGMEM tap_dance_entries_default[] = {
  // Apple Fn key + IME switch
  [0] = {KC_LNG2, APPLE_FF, KC_LNG1, APPLE_FF, TAPPING_TERM},
  // Protect layer 3 from misstouch, MENU + MO(3)
  [1] = {KC_APP, KC_APP, KC_APP, MO(3), TAPPING_TERM},
};

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  // mac base layer
  [0] = LAYOUT
  (
   KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSLS, RC_BTN,
   KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSPC, RC_CW,
   KC_LCTL, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,          KC_ENT,  RC_CCW,
   KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH,          KC_LSFT, MO(2),   RC_FINE,
   TD(0),   KC_LALT, KC_LGUI,                   KC_SPC,                             KC_RGUI, KC_RALT, TD(1),   KC_MPRV, KC_MPLY, KC_MNXT
  ),
  // standard base layer
  [1] = LAYOUT
  (
   KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSLS, RC_BTN,
   KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSPC, RC_CW,
   KC_LCTL, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,          KC_ENT,  RC_CCW,
   KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH,          KC_LSFT, MO(2),   RC_FINE,
   TD(0),   KC_LALT, KC_LGUI,                   KC_SPC,                             KC_RGUI, KC_RALT, TD(1),   KC_MPRV, KC_MPLY, KC_MNXT
  ),
  // HHKB-like fn layer
  // +Shift key for RGB decrement functions
  [2] = LAYOUT
  (
   _______, KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  KC_INS,  KC_ESC,
   KC_CAPS, KC_F16,  KC_F17,  KC_F18,  _______, _______, _______, _______, KC_PSCR, KC_SCRL, KC_PAUS, KC_UP,   _______, KC_DEL,  _______,
   _______, RGB_TOGX,RGB_VAI, RGB_HUI, _______, _______, KC_PAST, KC_PSLS, KC_HOME, KC_PGUP, KC_LEFT, KC_RGHT,          KC_PENT, _______,
   _______, RGB_MOD, RGB_SPI, RGB_SAI, _______, _______, KC_PPLS, KC_PMNS, KC_END,  KC_PGDN, KC_DOWN,          _______, _______, _______,
   _______, _______, _______,                   _______,                            _______, _______, _______, _______, _______, _______
  ),
  // settings, application-specific keys
  // row 3: normal settings, +shift key for unusual reversed setting
  [3] = LAYOUT
  (
   QK_BOOT, KC_F13,   KC_F14, KC_F15,  KC_F16,  KC_F17,  KC_F18,  KC_F19,  KC_F20,  KC_F21,  KC_F22,  KC_F23,  KC_F24,  _______, TERM_LCK,
   _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
   _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______, _______,
   _______, RHID_OFF,MAC_ON,  USJ_OFF, NK_ON,   CL_NORM, AG_NORM, BS_NORM, _______, _______, _______,          _______, _______, _______,
   _______, _______, _______,                   _______,                            _______, _______, _______, _______, _______, _______
  )
};
// clang-format on
