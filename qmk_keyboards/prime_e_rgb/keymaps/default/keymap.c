/* Copyright 2018 Holten Campbell
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

#define MO4_SPC LT(4, KC_SPC)
#define MO5_SPC LT(5, KC_SPC)

// tap dance
// [single tap, single hold, multi tap, tap hold, tapping term]
const tap_dance_entry_t PROGMEM tap_dance_predefined_entries[NUM_TAP_DANCE_PREDEFINED_ENTRIES] = {
  // TD(0) - LOPT + emacs command leader key
  [0] = {A(KC_X), KC_LALT, KC_LALT, KC_LALT, TAPPING_TERM},
  // TD(1) - LCMD + Spacemacs leader key
  [1] = {A(KC_M), KC_LGUI, KC_LGUI, KC_LGUI, TAPPING_TERM},
  // TD(2) - HHKB Fn + LANG2/LANG1
  [2] = {KC_LNG2, MO(2), KC_LNG1, MO(2), TAPPING_TERM},
  // TD(3) - RCMD + LANG2/LANG1
  [3] = {KC_LNG2, KC_RGUI, KC_LNG1, KC_RGUI, TAPPING_TERM},
  // TD(4) - Right Alt, Alt + layer switch
  [4] = {KC_RALT, KC_RALT, KC_RALT, MO(3), TAPPING_TERM}};

// since QMK 0.26, tap_dance_actions must be defined in keyma.c
tap_dance_action_t tap_dance_actions[TAP_DANCE_ENTRIES];

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  // mac base layer
  [0] = LAYOUT(
    KC_TAB,   KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,        KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_BSLS, KC_BSPC,
    KC_LCTL,  KC_A,    KC_S,    KC_D,    KC_F,    KC_G,        KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_ENT,
    KC_LSFT,  KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,        KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT,
    APPLE_FN, TD(0),                     TD(1),   MO4_SPC,     MO5_SPC, TD(2),                              TD(3),   TD(4)
  ),

  // win base layer
  [1] = LAYOUT(
    KC_TAB,   KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,        KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_BSLS, KC_BSPC,
    KC_LCTL,  KC_A,    KC_S,    KC_D,    KC_F,    KC_G,        KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_ENT,
    KC_LSFT,  KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,        KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT,
    APPLE_FN, TD(0),                     TD(1),   MO4_SPC,     MO5_SPC, MO(2),                              TD(2),   TD(3)
   ),

  // HHKB-like fn layer
  [2] = LAYOUT(
    KC_CAPS, KC_F1,   KC_F2,   KC_F3,   KC_F4,   _______,    KC_F18,  KC_PSCR, KC_SCRL, KC_PAUS, KC_UP,   KC_INS,  KC_DEL,
    _______, KC_F5,   KC_F6,   KC_F7,   KC_F8,   _______,    KC_F17,  KC_HOME, KC_PGUP, KC_LEFT, KC_RGHT, KC_PENT,
    _______, KC_F9,   KC_F10,  KC_F11,  KC_F12,  _______,    _______, KC_F18,  KC_END,  KC_PGDN, KC_DOWN, _______, _______,
    _______, _______,                   _______, _______,    _______, _______,                            _______, _______
  ),

  // settings, amethyst
  // row 2: normal settings, +shift key for unusual reversed setting
  // +Shift key for RGB decrement functions
  [3] = LAYOUT(
    QK_BOOT,  KC_F16,  KC_F17,  KC_F18,  _______, _______,    _______, _______, KC_F13,  KC_F14,  KC_F15, TERM_LCK,KC_ESC,
    _______, _______,  _______, _______, _______, _______,    _______, _______, RGB_TOG, RGB_VAI, RGB_HUI, _______,
    _______, RHID_OFF, AUT_ON,  MAC_ON,  USJ_OFF, NK_ON,      CL_NORM, AG_NORM, BS_NORM, RGB_MOD, RGB_SPI, RGB_SAI, _______,
    _______, AG_TOGG,                   _______, _______,     _______, _______,                            _______, _______
  ),

  // left thumb layer
  [4] = LAYOUT(
    KC_TILD, KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC,     KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    _______, KC_ESC,
    _______, KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN,     KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_QUOT,
    _______, KC_UNDS, KC_PLUS, KC_PIPE, KC_LCBR, KC_RCBR,     _______, KC_MINS, KC_EQL,  KC_BSLS, KC_LBRC, KC_RBRC, _______,
    _______, _______,                   _______, _______,     _______, _______,                            _______, _______
  ),

  // right thumb layer
  [5] = LAYOUT(
    KC_GRV,  KC_1,    KC_2,   KC_3,    KC_4,    KC_5,        KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC, _______, KC_ESC,
    _______, KC_6,    KC_7,   KC_8,    KC_9,    KC_0,        KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, KC_DQT,
    _______, KC_MINS, KC_EQL, KC_BSLS, KC_LBRC, KC_RBRC,     _______, KC_UNDS, KC_PLUS, KC_PIPE, KC_LCBR, KC_RCBR, _______,
    _______, _______,                  _______, _______,     _______, _______,                            _______, _______
  )
};
// clang-format on
