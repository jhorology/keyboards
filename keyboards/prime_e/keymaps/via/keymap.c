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

#define MO1_SPC LT(1, KC_SPC)
#define SFT_SPC RSFT_T(KC_SPC)

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [0] = LAYOUT
  (
   KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,        KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_BSLS, KC_BSPC,
   KC_LCTL, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,        KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_ENT,
   KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,        KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT,
   KC_APFN, KC_LGUI,                   KC_LALT, MO1_SPC,     SFT_SPC, MO(2),                              KC_RGUI,   MO(3)
   ),

  [1] = LAYOUT
  (
   KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,        KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,
   KC_TILD, KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC,     KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, KC_QUOT,
   _______, _______, _______, _______, _______, _______,     _______, _______, _______, KC_LBRC, KC_RBRC, _______, _______,
   _______, _______,                   _______, _______,     _______, _______,                            _______, _______
   ),

  [2] = LAYOUT
  (
   KC_CAPS, KC_F1,   KC_F2,   KC_F3,   KC_F4,   _______,    _______, KC_PSCR, KC_SLCK, KC_PAUS, KC_UP,   KC_INS,  KC_DEL,
   RGB_TOG, KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_PAST,    KC_PSLS, KC_HOME, KC_PGUP, KC_LEFT, KC_RGHT, KC_PENT,
   _______, KC_F9,   KC_F10,  KC_F11,  KC_F12,  _______,    KC_PPLS, KC_PMNS, KC_END,  KC_PGDN, KC_DOWN, _______, _______,
   _______, _______,                   _______, _______,    _______, _______,                            _______, _______
   ),

  [3] = LAYOUT
  (
   KC_ESC,  _______, _______, _______, _______, _______,     _______, KC_F16,  KC_F17,  KC_F18,  _______, _______, RESET,
   _______, KC_VOLD, KC_VOLU, KC_MUTE, KC_EJCT, _______,     RGB_TOG, RGB_VAI, RGB_HUI, RGB_SAI, _______, _______,
   _______, _______, _______, _______, _______, _______,     RGB_MOD, RGB_VAD, RGB_HUD, RGB_SAD, _______, _______, _______,
   _______, _______,                   _______, _______,     _______, _______,                            _______, _______
   )
};

void matrix_init_user(void) {
  // set CapsLock LED to output and low
  setPinOutput(B1);
  writePinLow(B1);
  // #layer LED
  setPinOutput(B2);
  writePinLow(B2);
  setPinOutput(B3);
  writePinLow(B3);
}

void led_set_user(uint8_t usb_led) {
  writePin(B1, IS_LED_ON(usb_led, USB_LED_CAPS_LOCK));
}

//function for layer indicator LED
layer_state_t layer_state_set_user(layer_state_t state) {
  // indicate #layer
  layer_state_t highestLayer = get_highest_layer(state);
  writePin(B2, (highestLayer & 0x01) != 0);
  writePin(B3, (highestLayer & 0x02) != 0);
  return state;
}
