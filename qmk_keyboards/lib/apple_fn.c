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
#include "apple_fn.h"

#include "custom_config.h"
#include "custom_keycodes.h"

static bool process_fkey_override(uint16_t keycode, keyrecord_t *record);
static bool process_non_mac_fn(uint16_t keycode, keyrecord_t *record);
#ifdef APPLE_FN_OVERRIDE_F456
static bool process_override_f456(uint8_t keycode, keyrecord_t *record);
#endif
static void send_dictation(bool pressed);
static void send_do_not_disturb(bool pressed);
static uint8_t apple_ff_cnt;

#define IS_FN_PRESSED() host_apple_is_pressed(1 << USAGE_INDEX_AVT_KEYBOARD_FN)

#define SEND_FN(pressed) host_apple_send(pressed, USAGE_INDEX_AVT_KEYBOARD_FN)

bool process_apple_fn(uint16_t keycode, keyrecord_t *record) {
  switch (keycode) {
    case APPLE_FN:
      SEND_FN(record->event.pressed);
      return false;
    case APPLE_FF:
      apple_ff_cnt += record->event.pressed ? 1 : -1;
      SEND_FN(record->event.pressed);
      if (!IS_FN_PRESSED()) apple_ff_cnt = 0;
      return false;
#ifdef APPLE_EXTRA_KEY_ENABLE
    case AVT_ILLUMINATION_UP ... AVK_LANGUAGE:
      host_apple_send(record->event.pressed,
                      USAGE_INDEX_AVT_ILLUMINATION_UP + (keycode - AVT_ILLUMINATION_UP));
      return false;
#endif
    default:
      return (
#ifdef APPLE_FN_OVERRIDE_F456
        process_override_f456(keycode, record) &&
#endif
        process_fkey_override(keycode, record) && process_non_mac_fn(keycode, record));
  }
}

static bool process_fkey_override(uint16_t keycode, keyrecord_t *record) {
  static uint16_t fkey_override_flags;
  // ignore generated key
  if (record->keycode || (apple_ff_cnt == 0 && record->event.pressed)) {
    return true;
  }

  uint8_t fkey_index;
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
  uint8_t fkey = KC_F1 + fkey_index;
  if (record->event.pressed) {
    if (fkey_override_flags & flag) {
      return true;
    }
    fkey_override_flags |= flag;
  } else {
    if (!(fkey_override_flags & flag)) {
      return true;
    }
    fkey_override_flags &= ~flag;
  }
  if (process_non_mac_fn(fkey, record)
#ifdef APPLE_FN_OVERRIDE_F456
      && process_override_f456(fkey, record)
#endif
  ) {
    if (record->event.pressed) {
      register_code(fkey);
    } else {
      unregister_code(fkey);
    }
  }
  return false;
}

static bool process_non_mac_fn(uint16_t keycode, keyrecord_t *record) {
  static uint32_t fn_override_flags;

  if (custom_config_mac_is_enable()) return true;

  // ignore generated key
  if (record->keycode) return true;

  if (!IS_FN_PRESSED() && record->event.pressed) return true;
  if (!fn_override_flags && !record->event.pressed) return true;

  non_mac_fn_key_t fn_key = FN_UNKNOWN;
  if (custom_config_non_mac_fn_fkey_is_enable()) {
    switch (keycode) {
      case KC_F1 ... KC_F12:
        fn_key = FN_F1 + keycode - KC_F1;
        break;
    }
  }
  if (custom_config_non_mac_fn_alpha_is_enable()) {
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
        fn_key = FN_A;
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
  if (custom_config_non_mac_fn_cursor_is_enable()) {
    switch (keycode) {
      case KC_BSPC:
        fn_key = FN_BSPC;
        break;
      case KC_UP:
        fn_key = FN_UP;
        break;
      case KC_DOWN:
        fn_key = FN_DOWN;
        break;
      case KC_LEFT:
        fn_key = FN_LEFT;
        break;
      case KC_RIGHT:
        fn_key = FN_RIGHT;
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

#ifdef APPLE_FN_OVERRIDE_F456
static bool process_override_f456(uint8_t keycode, keyrecord_t *record) {
  if (custom_config_mac_is_enable() && IS_FN_PRESSED()) {
    switch (keycode) {
      case KC_F4:  // F4 Spotlight
        host_apple_send(record->event.pressed, USAGE_INDEX_AVK_SPOTLIGHT);
        return false;
      case KC_F5:  // F5 Dictation
        send_dictation(record->event.pressed);
        return false;
      case KC_F6:  // F6 Do Not Disturb
        send_do_not_disturb(record->event.pressed);
        return false;
    }
  }
  return true;
}
#endif

static void send_dictation(bool pressed) {
  // Voice Command doesn't work with apple fn.
  bool apple_fn_pressed = IS_FN_PRESSED();
  // TODO don't use Fn key Twice in shortcut setting
  if (pressed && apple_fn_pressed) SEND_FN(false);
  host_consumer_send(pressed ? 0x00cf : 0);
  if (pressed && apple_fn_pressed) SEND_FN(true);
}

static void send_do_not_disturb(bool pressed) {
  // Do Not Disturb doesn't work with apple fn.
  bool apple_fn_pressed = IS_FN_PRESSED();
  // TODO don't use Fn key Twice in shortcut setting
  if (pressed && apple_fn_pressed) SEND_FN(false);
  host_system_send(pressed ? 0x009b : 0);
  if (pressed && apple_fn_pressed) SEND_FN(true);
}
