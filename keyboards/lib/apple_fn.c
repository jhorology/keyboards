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
#include QMK_KEYBOARD_H

#include "apple_fn.h"

#include "custom_config.h"
#include "custom_keycodes.h"

static bool process_apple_ff_fkey(uint16_t fkey_index, keyrecord_t *record);

// apple-like F1-12 keys
const uint16_t apple_like_fkeys[] = {
    KC_BRID,  // F1 decrease brightness
    KC_BRIU,  // F2 increase brightness
    KC_F3,    // TODO F3 mission control
    KC_F4,    // TODO F4 older mac: launchpad / newer mac: spotlight
    KC_F5,    // TODO F5 olfer mac: F5        / newer mac: dictaion
    KC_F6,    // TODO F6 older mac: F6        / newer mac: sleep
    KC_MPRV,  // F7 meda prev
    KC_MPLY,  // F8 media play/pause
    KC_MNXT,  // F9 media next
    KC_MUTE,  // F10 mute audio
    KC_VOLD,  // F11 decrease audio volume
    KC_VOLU   // F12 increase audio volume
};

static bool apple_ff;

bool process_apple_fn(uint16_t keycode, keyrecord_t *record) {
  switch (keycode) {
    case MAC_TOGG:
      if (record->event.pressed) {
        custom_config_toggle_mac();
      }
      return false;
    case MAC_ON:
      if (record->event.pressed) {
        custom_config_set_mac(true);
      }
      return false;
    case MAC_OFF:
      if (record->event.pressed) {
        custom_config_set_mac(false);
      }
      return false;
    case APPLE_FN:
      host_apple_send(record->event.pressed ? 1 : 0);
      return false;
    case APPLE_FF:
      apple_ff = record->event.pressed;
      host_apple_send(record->event.pressed ? 1 : 0);
      return false;
    case KC_1 ... KC_0:
      if (!process_apple_ff_fkey(keycode - KC_1, record)) return false;
      break;
    case KC_MINS:
      if (!process_apple_ff_fkey(10, record)) return false;
      break;
    case KC_EQL:
      if (!process_apple_ff_fkey(11, record)) return false;
      break;
  }
  return true;
}

static bool process_apple_ff_fkey(uint16_t fkey_index, keyrecord_t *record) {
  static uint16_t apple_ff_flags;
  uint16_t flag = 1 << fkey_index;
  if (record->event.pressed) {
    if (apple_ff) {
      apple_ff_flags |= flag;
      register_code16(custom_config_is_mac() ? KC_F1 + fkey_index : apple_like_fkeys[fkey_index]);
      return false;
    }
  } else {
    if (apple_ff_flags & flag) {
      apple_ff_flags &= ~flag;
      unregister_code16(custom_config_is_mac() ? KC_F1 + fkey_index : apple_like_fkeys[fkey_index]);
      return false;
    }
  }
  return true;
}
