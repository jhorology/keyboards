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
#ifdef RADIAL_CONTROLLER_ENABLE
#  include "radial_controller.h"
#endif
#if !defined(VIAL_ENABLE) && defined(TAP_DANCE_ENABLE)
#  include "tap_dance.h"
#endif

//   local functions
//------------------------------------------

static void set_mac(bool value);
static void set_usj(bool value);

static bool process_apple_ff_fkey(uint16_t fkey_index, keyrecord_t *record);

#ifdef VIAL_ENABLE
static void pgm_memcpy(void *dest, const void *src, size_t len);
#endif

//   global variavles
//------------------------------------------

common_kb_config_t g_common_kb_config;

/*
 * pre-defined vial tap dabce
 */
#if !defined(VIAL_ENABLE) && defined(TAP_DANCE_ENABLE)
typedef struct {
  uint16_t on_single_tap;
  uint16_t on_single_hold;
  uint16_t on_multi_tap;
  uint16_t on_tap_hold;
  uint16_t tapping_term;
} tap_dance_entry_t;
const tap_dance_entry_t PROGMEM via_tap_dance_entries_default[] = {
#else
const vial_tap_dance_entry_t PROGMEM vial_tap_dance_actions_default[] = {
#endif
    // clang-format off
  //                          tap,      hold,   double_tap, tap_hold, tapping_term
  // for HHKB, Right Alt, on  tap hold: MO(3)
  [TD_RALT_MO3]            = { KC_RALT,  KC_RALT, KC_RALT, MO(3),    TAPPING_TERM },
  // for mac,  Apple Fn/Globe + FK override, on tap: KC_LNG2(英数), on double tap: KC_LNG1(かな)
  [TD_APFF_EISU_KANA]      = { KC_LNG2, APPLE_FF, KC_LNG1, APPLE_FF, TAPPING_TERM },
  // Left GUI, on tap: KC_LNG2(英数), on double tap: KC_LNG1(かな)
  [TD_LGUI_EISU_KANA]      = { KC_LNG2, KC_LGUI,  KC_LNG1, KC_LGUI,  TAPPING_TERM },
  // Left Alt, on tap: KC_LNG2(英数), on double tap: KC_LNG1(かな)
  [TD_LALT_EISU_KANA]      = { KC_LNG2, KC_LALT,  KC_LNG1, KC_LALT,  TAPPING_TERM },
  // Left Alt, on tap hold: Apple fn/globe + FK overrde
  [TD_LALT_APFF]           = { KC_LALT,  KC_LALT, KC_LALT, APPLE_FF, TAPPING_TERM },
  // Left Alt, on tap hold: Apple fn/globe + FK overrde,  on tap: KC_LNG2(英数), on double tap: KC_LNG1(かな)
  [TD_LALT_APFF_EISU_KANA] = { KC_LNG2,  KC_LALT, KC_LNG1, APPLE_FF, TAPPING_TERM }
    // clang-format on
};

#ifdef VIAL_COMBO_ENABLE
/*
 * pre-defined vial combo
 */
const vial_combo_entry_t PROGMEM vial_combo_actions_default[] = {};
#endif

#if !defined(VIAL_ENABLE) && defined(TAP_DANCE_ENABLE)
combo_t key_combos[COMBO_COUNT] = {};
#endif

#ifdef VIAL_KEY_OVERRIDE_ENABLE
/*
 * pre-defined key overrride
 */
const vial_combo_entry_t PROGMEM vial_key_override_actions_default[] = {};
#endif

//   local variables
//------------------------------------------

typedef union {
  uint16_t raw;
  struct {
    bool apple_ff : 1;
    bool capslock_old : 1;
    uint16_t apple_ff_flags : 12;
    bool eisu_kana : 1;
  };
} volatile_state_t;
volatile_state_t volatile_state;

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

//  qmk/vial custom hook functsions
//------------------------------------------

void via_init_kb(void) {
  if (!via_eeprom_is_valid()) {
    eeconfig_init_kb();
  }
}

void eeconfig_init_kb(void) {
  g_common_kb_config.raw = 0;
  g_common_kb_config.mac = true;
  eeconfig_update_kb(g_common_kb_config.raw);
  eeconfig_init_user();
}

void keyboard_pre_init_kb(void) {
  // need none-volatile settings before initilize USB
  g_common_kb_config.raw = eeconfig_read_kb();
  keyboard_pre_init_user();
}

void keyboard_post_init_kb(void) {
  default_layer_set(g_common_kb_config.mac ? 1 : 2);

  // TODO support VIA v3 UI
#if !defined(VIAL_ENABLE) && defined(TAP_DANCE_ENABLE)
  tap_dance_actions_init();
#endif

  keyboard_post_init_user();
}

bool process_record_kb(uint16_t keycode, keyrecord_t *record) {
  if (!process_record_user(keycode, record)) return false;
  bool result = true;
  switch (keycode) {
#if !defined(VIAL_ENABLE) && defined(TAP_DANCE_ENABLE)
    case QK_TAP_DANCE ... QK_TAP_DANCE_MAX:
      tap_dance_store_keyevent(TD_INDEX(keycode), &record->event);
      return true;
#endif
    case MAC_TOGG:
      if (record->event.pressed) {
        set_mac(!g_common_kb_config.mac);
      }
      return false;
    case MAC_ON:
      if (record->event.pressed) {
        set_mac(true);
      }
      return false;
    case MAC_OFF:
      if (record->event.pressed) {
        set_mac(false);
      }
      return false;
    case APPLE_FN:
      if (record->event.pressed) {
        host_apple_send(1);
      } else {
        host_apple_send(0);
      }
      return false;
    case APPLE_FF:
      if (record->event.pressed) {
        volatile_state.apple_ff = true;
        host_apple_send(1);
      } else {
        volatile_state.apple_ff = false;
        host_apple_send(0);
      }
      return false;
    case KC_1 ... KC_0:
      if (volatile_state.apple_ff) result = process_apple_ff_fkey(keycode - KC_1, record);
      break;
    case KC_MINS:
      if (volatile_state.apple_ff) result = process_apple_ff_fkey(10, record);
      break;
    case KC_EQL:
      if (volatile_state.apple_ff) result = process_apple_ff_fkey(11, record);
      break;
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
    case USJ_TOGG:
      if (record->event.pressed) {
        set_usj(!g_common_kb_config.usj);
      }
      return false;
    case USJ_ON:
      if (record->event.pressed) {
        set_usj(true);
      }
      return false;
    case USJ_OFF:
      if (record->event.pressed) {
        set_usj(false);
      }
      return false;
#ifdef RADIAL_CONTROLLER_ENABLE
    case DIAL_BUT:
      if (record->event.pressed) {
        radial_controller_button_update(true);
      } else {
        radial_controller_button_update(false);
      }
      return false;
    case DIAL_L:
      if (record->event.pressed) {
        radial_controller_dial_update(false, false);
      }
      return false;
    case DIAL_R:
      if (record->event.pressed) {
        radial_controller_dial_update(true, false);
      }
      return false;
    case DIAL_LC:
      if (record->event.pressed) {
        radial_controller_dial_update(false, true);
      } else {
        radial_controller_dial_finished();
      }
      return false;
    case DIAL_RC:
      if (record->event.pressed) {
        radial_controller_dial_update(true, true);
      } else {
        radial_controller_dial_finished();
      }
      return false;
#endif
  }
  if (g_common_kb_config.usj && result) {
    result = process_ansi_layout_on_jis(keycode, record);
  }
  return result;
}

#ifdef RGB_MATRIX_ENABLE

#  ifdef CAPS_LOCK_LED
void rgb_matrix_indicators_kb(void) {
  bool capslock = host_keyboard_led_state().caps_lock;
  if (capslock || volatile_state.capslock_old) {
    rgb_matrix_set_color(CAPS_LOCK_LED, capslock ? rgb_matrix_get_val() : 0, 0, 0);
  }
  volatile_state.capslock_old = capslock;

  rgb_matrix_indicators_user();
}
#  endif

void suspend_power_down_kb(void) {
  rgb_matrix_set_suspend_state(true);
  suspend_power_down_user();
}

void suspend_wakeup_init_kb(void) {
  suspend_wakeup_init_user();
  rgb_matrix_set_suspend_state(false);
}
#endif

#if !defined(VIAL_ENABLE) && defined(TAP_DANCE_ENABLE)

// TODO VIA v3 UI
uint16_t dynamic_tap_dance_keycode(uint16_t index, tap_dance_state_t state) {
  if (index >= 0 && index < TAP_DANCE_ACTIONS_DEFAULT_LENGTH) {
    switch (state) {
      case TD_SINGLE_TAP:
        return pgm_read_word(&via_tap_dance_entries_default[index].on_single_tap);
      case TD_SINGLE_HOLD:
        return pgm_read_word(&via_tap_dance_entries_default[index].on_single_hold);
      case TD_MULTI_TAP:
        return pgm_read_word(&via_tap_dance_entries_default[index].on_multi_tap);
      case TD_TAP_HOLD:
        return pgm_read_word(&via_tap_dance_entries_default[index].on_tap_hold);
      default:
        return KC_NO;
    }
  }
  return KC_NO;
}

// TODO VIA v3 UI
uint16_t dynamic_tap_dance_tapping_term(uint16_t index) {
  if (index >= 0 && index < TAP_DANCE_ACTIONS_DEFAULT_LENGTH) {
    return pgm_read_word(&via_tap_dance_entries_default[index].tapping_term);
  }
  return TAPPING_TERM;
}

#endif

#ifdef VIAL_TAP_DANCE_ENABLE
void vial_tap_dance_reset_kb(uint8_t index, vial_tap_dance_entry_t *entry) {
  if (index < TAP_DANCE_ACTIONS_DEFAULT_LENGTH) {
    pgm_memcpy(entry, &vial_tap_dance_actions_default[index], sizeof(vial_tap_dance_entry_t));
  }
  vial_tap_dance_reset_user(index, entry);
}
#endif

#ifdef VIAL_COMBO_ENABLE
void vial_combo_reset_kb(uint8_t index, vial_combo_entry_t *entry) {
  if (index < VIAL_COMBO_ACTIONS_DEFAULT_LENGTH) {
    pgm_memcpy(entry, &vial_combo_actions_default[index], sizeof(vial_combo_entry_t));
  }
  vial_combo_reset_user(index, entry);
}
#endif

#ifdef VIAL_KEY_OVERRIDE_ENABLE
void vial_key_override_reset_kb(uint8_t index, vial_key_override_entry_t *entry) {
  if (index < VIAL_KEY_OVERRIDE_ACTIONS_DEFAULT_LENGTH) {
    pgm_memcpy(entry, &vial_key_override_actions_default[index], sizeof(vial_key_override_entry_t));
  }
  vial_key_override_reset_user(index, entry);
}
#endif

//   local functions
//------------------------------------------

static bool process_apple_ff_fkey(uint16_t fkey_index, keyrecord_t *record) {
  uint16_t flag = 1 << fkey_index;
  if (record->event.pressed) {
    volatile_state.apple_ff_flags |= flag;
    register_code(g_common_kb_config.mac ? KC_F1 + fkey_index : apple_like_fkeys[fkey_index]);
    return false;
  } else {
    volatile_state.apple_ff_flags &= ~flag;
    unregister_code(g_common_kb_config.mac ? KC_F1 + fkey_index : apple_like_fkeys[fkey_index]);
    return false;
  }
  return true;
}

static void set_mac(bool value) {
  if (value != g_common_kb_config.mac) {
    g_common_kb_config.mac = value;
    eeconfig_update_kb(g_common_kb_config.raw);
    // reboot for changing USB device descriptor
    soft_reset_keyboard();
  }
}

static void set_usj(bool value) {
  if (value != g_common_kb_config.usj) {
    g_common_kb_config.usj = value;
    eeconfig_update_kb(g_common_kb_config.raw);
  }
}

#ifdef VIAL_ENABLE
static void pgm_memcpy(void *dest, const void *src, size_t len) {
  for (size_t i = 0; i < len; i++) {
    *(uint8_t *)dest++ = pgm_read_byte((uint8_t *)src++);
  }
}
#endif
