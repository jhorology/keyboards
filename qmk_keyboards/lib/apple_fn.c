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
#include "custom_config.h"
#include QMK_KEYBOARD_H

#include "apple_fn.h"
#include "lib/custom_config.h"
#include "lib/custom_keycodes.h"

static bool process_fkey_override(uint16_t keycode, keyrecord_t *record);
static bool process_non_mac_fn(uint16_t keycode, keyrecord_t *record);

// apple-like F1-12 keys
const uint16_t PROGMEM none_mac_fn_functions_default[] = {
    [FN_F1] = KC_BRID,   // F1 decrease brightness
    [FN_F2] = KC_BRIU,   // F2 increase brightness
    [FN_F3] = KC_F3,     // TODO F3 mission control
    [FN_F4] = KC_F4,     // TODO F4 older mac: launchpad / newer mac: spotlight
    [FN_F5] = KC_F5,     // TODO F5 olfer mac: F5        / newer mac: dictaion
    [FN_F6] = KC_F6,     // TODO F6 older mac: F6        / newer mac: sleep
    [FN_F7] = KC_MPRV,   // F7 meda prev
    [FN_F8] = KC_MPLY,   // F8 media play/pause
    [FN_F9] = KC_MNXT,   // F9 media next
    [FN_F10] = KC_MUTE,  // F10 mute audio
    [FN_F11] = KC_VOLD,  // F11 decrease audio volume
    [FN_F12] = KC_VOLU   // F12 increase audio volume
};

static bool apple_fn;
static bool apple_ff;

bool process_apple_fn(uint16_t keycode, keyrecord_t *record) {
  switch (keycode) {
    case APPLE_FN:
      apple_fn = record->event.pressed;
      host_apple_send(apple_fn ? 1 : 0);
      return false;
    case APPLE_FF:
      apple_ff = record->event.pressed;
      host_apple_send(apple_ff ? 1 : 0);
      return false;
    default:
      return process_fkey_override(keycode, record) && process_non_mac_fn(keycode, record);
  }
}

static bool process_fkey_override(uint16_t keycode, keyrecord_t *record) {
  static uint16_t fkey_override_flags;

  if (!apple_ff && record->event.pressed) return true;
  if (!fkey_override_flags && !record->event.pressed) return true;

  uint16_t fkey_index;
  switch (keycode) {
    case KC_1 ... KC_0:
      fkey_index = keycode - KC_1;
      break;
    case KC_MINS:
      fkey_index = 10;
      break;
    case KC_EQL:
      fkey_index = 11;
      break;
    default:
      return true;
  }
  uint16_t flag = (1 << fkey_index);
  uint16_t fkey = KC_F1 + fkey_index;
  if (record->event.pressed && !(fkey_override_flags & flag)) {
    fkey_override_flags |= flag;
    if (!custom_config_mac_is_enable() && !process_non_mac_fn(fkey, record)) {
      return false;
    }
    register_code(fkey);
    return false;
  } else if (!record->event.pressed && (fkey_override_flags & flag)) {
    fkey_override_flags &= ~flag;
    if (!custom_config_mac_is_enable() && !process_non_mac_fn(fkey, record)) {
      return false;
    }
    unregister_code(fkey);
    return false;
  }
  return true;
}

static bool process_non_mac_fn(uint16_t keycode, keyrecord_t *record) {
  static uint32_t fn_override_flags;

  if (custom_config_mac_is_enable()) return true;
  if (!(apple_fn || apple_ff) && record->event.pressed) return true;
  if (!fn_override_flags && !record->event.pressed) return true;

  non_mac_fn_key_t fn_key = FN_UNKNOWN;
  if (custom_config_non_mac_fn_get_mode() & NON_MAC_FN_MODE_FKEY_MASK) {
    switch (keycode) {
      case KC_F1 ... KC_F12:
        fn_key = FN_F1 + keycode - KC_F1;
        break;
    }
  }
  if (custom_config_non_mac_fn_get_mode() & NON_MAC_FN_MODE_ALPHA_MASK) {
    switch (keycode) {
      case KC_SPC:
        fn_key = FN_SPC;
        break;
      case KC_Q:
        fn_key = FN_Q;
        break;
      case KC_E:
        fn_key = FN_E;
        break;
      case KC_A:
        fn_key = FN_D;
        break;
      case KC_D:
        fn_key = FN_D;
        break;
      case KC_F:
        fn_key = FN_F;
        break;
      case KC_H:
        fn_key = FN_H;
        break;
      case KC_C:
        fn_key = FN_C;
        break;
      case KC_N:
        fn_key = FN_N;
        break;
      case KC_M:
        fn_key = FN_M;
        break;
    }
  }
  if (fn_key == FN_UNKNOWN) return true;

  uint32_t flag = (1 << fn_key);
  uint16_t fn_keycode = KC_NO;
  if (record->event.pressed && !(fn_override_flags & flag)) {
    fn_override_flags |= flag;
    fn_keycode = dynamic_non_mac_fn_keycode(fn_key);
  } else if (!record->event.pressed && fn_override_flags & flag) {
    fn_override_flags &= ~flag;
    fn_keycode = dynamic_non_mac_fn_keycode(fn_key);
  }
  if (fn_keycode != KC_NO) {
    record->keycode = fn_keycode;
    process_record(record);
    return false;
  }
  return true;
}
