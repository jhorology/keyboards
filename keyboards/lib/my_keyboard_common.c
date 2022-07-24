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

#include "my_keyboard_common.h"

#include "layout_util.h"

//   local functions
//------------------------------------------

void set_usj_enabled(bool value);

#ifdef APPLE_FN_ENABLE
bool process_apple_ff_fkey(uint16_t fkey_index, keyrecord_t *record);
#endif

#ifdef VIAL_ENABLE
void pgm_memcpy(uint8_t *dest, uint8_t *src, size_t len);
#endif

//   global variavles
//------------------------------------------

user_config_t g_user_config;

#ifdef VIAL_ENABLE

#  ifdef VIAL_TAP_DANCE_ENABLE
const vial_tap_dance_entry_t PROGMEM vial_tap_dance_actions_default[] = {
    // tap, hold,  double_tap, tap_hold, tapping_term
    [TD_LALT_IME] = {KC_LALT, KC_LALT, LALT(KC_GRV), KC_LALT, TAPPING_TERM},
    [TD_LGUI_EISU] = {KC_LGUI, KC_LGUI, KC_LNG2, KC_LGUI, TAPPING_TERM},
    [TD_RGUI_KANA] = {KC_RGUI, KC_RGUI, KC_LNG1, KC_RGUI, TAPPING_TERM},
    [TD_LGUI_EISU_KANA] = {KC_LGUI, KC_LGUI, EJ_TOGG, KC_LGUI, TAPPING_TERM}};
#  endif
#  ifdef VIAL_COMBO_ENABLE
const vial_combo_entry_t PROGMEM vial_combo_actions_default[] = {};
#  endif
#  ifdef VIAL_KEY_OVERRIDE_ENABLE
const vial_combo_entry_t PROGMEM vial_key_override_actions_default[] = {};
#  endif
#else
qk_tap_dance_action_t tap_dance_actions[] = {
    // Tap once for standard key, twice to toggle layers
    [TD_LALT_IME] = ACTION_TAP_DANCE_DOUBLE(KC_LALT, LALT(KC_GRV)),
    [TD_LGUI_EISU] = ACTION_TAP_DANCE_DOUBLE(KC_LGUI, KC_LNG2),
    [TD_RGUI_KANA] = ACTION_TAP_DANCE_DOUBLE(KC_RGUI, KC_LNG1),
    [TD_LGUI_EISU_KANA] = ACTION_TAP_DANCE_DOUBLE(KC_LGUI, EJ_TOGG)};
#endif

//   local variables
//------------------------------------------

#ifdef APPLE_FN_ENABLE
/*
  bit0: apple_fn pressed state
  biti-12: 1-0, minus, equla keyes remapped state
 */
static uint16_t apple_ff_flags;
#endif

typedef union {
  uint16_t raw;
  struct {
    bool eisu_kana : 1;
  };
} volatile_state_t;
volatile_state_t volatile_state;

//  keyboard spcific hook functsions
//------------------------------------------

__attribute__((weak)) void init_with_config_kb(void) {}
__attribute__((weak)) bool process_record_user_kb(uint16_t keycode, keyrecord_t *record) { return true; }

//  qmk/vial custom hook functsions
//------------------------------------------

void keyboard_post_init_kb(void) {
  g_user_config.raw = eeconfig_read_user();
  init_with_config_kb();
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  if (!process_record_user_kb(keycode, record)) return false;
  if (g_user_config.usj_enabled) {
    if (!process_ansi_layout_on_apple_jis(keycode, record)) return false;
  }
  bool result = true;
  switch (keycode) {
#ifdef APPLE_FN_ENABLE
    case APPLE_FN:
      if (record->event.pressed) {
        register_code(KC_APFN);
      } else {
        unregister_code(KC_APFN);
      }
      return false;
    case APPLE_FF:
      if (record->event.pressed) {
        apple_ff_flags |= 1;
        register_code(KC_APFN);
      } else {
        apple_ff_flags &= 0xfffe;
        unregister_code(KC_APFN);
      }
      return false;
    case EJ_TOGG:
      if (record->event.pressed) {
        volatile_state.eisu_kana = !volatile_state.eisu_kana;
        // KC_LNG1 かな
        // KC_LNG2 英数
        register_code(volatile_state.eisu_kana ? KC_LNG1 : KC_LANG2);
      } else {
        unregister_code(volatile_state.eisu_kana ? KC_LNG1 : KC_LANG2);
      }
      return false;
    case KC_1 ... KC_0:
      result = process_apple_ff_fkey(keycode - KC_1, record);
      break;
    case KC_MINS:
      result = process_apple_ff_fkey(10, record);
      break;
    case KC_EQL:
      result = process_apple_ff_fkey(11, record);
      break;
#endif
    case USJ_TOGG:
      set_usj_enabled(!g_user_config.usj_enabled);
      return false;
    case USJ_ON:
      set_usj_enabled(true);
      return false;
    case USJ_OFF:
      set_usj_enabled(false);
      return false;
  }
  return result;
}

#ifdef RGB_MATRIX_ENABLE
void suspend_power_down_kb(void) {
  rgb_matrix_set_suspend_state(true);
  suspend_power_down_user();
}

void suspend_wakeup_init_kb(void) {
  suspend_wakeup_init_user();
  rgb_matrix_set_suspend_state(false);
}
#endif

#ifdef VIAL_ENABLE
#  ifdef VIAL_TAP_DANCE_ENABLE
void vial_tap_dance_reset_user(uint8_t index, vial_tap_dance_entry_t *entry) {
  if (index < TAP_DANCE_ACTIONS_DEFAULT_LENGTH) {
    pgm_memcpy((uint8_t *)entry, (uint8_t *)&vial_tap_dance_actions_default[index], sizeof(vial_tap_dance_entry_t));
  }
}
#  endif

#  ifdef VIAL_COMBO_ENABLE
void vial_combo_reset_user(uint8_t index, vial_combo_entry_t *entry) {
  if (index < VIAL_COMBO_ACTIONS_DEFAULT_LENGTH) {
    pgm_memcpy((uint8_t *)entry, (uint8_t *)&vial_combo_actions_default[index], sizeof(vial_combo_entry_t));
  }
}
#  endif

#  ifdef VIAL_KEY_OVERRIDE_ENABLE
void vial_key_override_reset_user(uint8_t index, vial_key_override_entry_t *entry) {
  if (index < VIAL_KEY_OVERRIDE_ACTIONS_DEFAULT_LENGTH) {
    pgm_memcpy((uint8_t *)entry, (uint8_t *)&vial_key_override_actions_default[index],
               sizeof(vial_key_override_entry_t));
  }
}
#  endif
#endif

//   local functions
//------------------------------------------

bool process_apple_ff_fkey(uint16_t fkey_index, keyrecord_t *record) {
  uint16_t flag = 1 << (fkey_index + 1);
  if (record->event.pressed) {
    if (apple_ff_flags & 1) {
      apple_ff_flags |= flag;
      register_code(KC_F1 + fkey_index);
      return false;
    }
  } else {
    if (apple_ff_flags & flag) {
      apple_ff_flags &= ~flag;
      unregister_code(KC_F1 + fkey_index);
      return false;
    }
  }
  return true;
}

void set_usj_enabled(bool value) {
  if (value != g_user_config.usj_enabled) {
    g_user_config.usj_enabled = value;
    eeconfig_update_user(g_user_config.raw);
  }
}

#ifdef VIAL_ENABLE
void pgm_memcpy(uint8_t *dest, uint8_t *src, size_t len) {
  for (size_t i = 0; i < len; i++) {
    *dest++ = pgm_read_byte(&(*src++));
  }
}
#endif
