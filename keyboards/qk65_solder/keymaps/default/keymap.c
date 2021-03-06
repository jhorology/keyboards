/*
Copyright 2022 qwertykeys

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include QMK_KEYBOARD_H

#define LALT_IME TD(TD_LALT_IME)        // LALT + ` on double tap, KC_LALT in other case.
#define CMD_EISU TD(TD_LGUI_EISU)       // KC_LNG2(英数) on double tap, KC_LGUI in other case.
#define CMD_KANA TD(TD_RGUI_KANA)       // KC_LNG1(かな) on double tap, KC_LGUI in other case.
#define CMD_TGEJ TD(TD_LGUI_EISU_KANA)  // Toogle send KC_LNG1(かな) and KC_LNG2(英数)
                                        // on double tap, KC_LGUI in other case.

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

  [0] = LAYOUT_65_ansi_blocker_tsangan_split_bs(
   //   0      1        2        3        4        5        6        7        8        9        A           B       C         D       E
    KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSLS, KC_ESC,  KC_HOME,
    KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSPC, KC_PGUP,
    KC_LCTL, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,          KC_ENT,  KC_PGDN,
    KC_LSFT,          KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT, MO(1),   KC_END,
    APPLE_FF,KC_LALT, CMD_TGEJ,                           KC_SPC,                    MO(2),                     KC_MPRV, KC_MPLY, KC_MNXT
  ),

  [1] = LAYOUT_65_ansi_blocker_tsangan_split_bs(
    _______, KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  KC_INS,  KC_DEL,  _______,
    KC_CAPS, _______, _______, _______, _______, _______, _______, _______, KC_PSCR, KC_SLCK, KC_PAUS, KC_UP,   _______, _______, _______,
    _______, KC_VOLD, KC_VOLU, KC_MUTE, KC_EJCT, _______, KC_PAST, KC_PSLS, KC_HOME, KC_PGUP, KC_LEFT, KC_RGHT,          KC_PENT, _______,
    _______,          _______, _______, _______, _______, _______, KC_PPLS, KC_PMNS, KC_END,  KC_PGDN, KC_DOWN, _______, _______, _______,
    _______, _______, _______,                            _______,                   _______,                   _______, _______, _______
  ),

  [2] = LAYOUT_65_ansi_blocker_tsangan_split_bs(
    _______, KC_F13,   KC_F14, KC_F15,  KC_F16,  KC_F17,  KC_F18,  KC_F19,  KC_F20,  KC_F21,  KC_F22,  KC_F23,  KC_F24,  _______, QK_BOOT,   _______,
    _______, _______, _______, _______, _______, _______, _______, _______, KC_F16,  KC_F17,  KC_F18,  _______, _______, _______, _______,
    CL_TOGG, USJ_OFF, _______, _______, _______, _______, _______, USJ_ON,  _______, _______, _______, _______,          _______, _______,
    _______,          _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, AG_TOGG,                            _______,                   _______,                   _______, _______, _______
  ),

  [3] = LAYOUT_65_ansi_blocker_tsangan_split_bs(
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______, _______,
    _______,          _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______,                            _______,                   _______,                   _______, _______, _______
  )

};
// clang-format on
