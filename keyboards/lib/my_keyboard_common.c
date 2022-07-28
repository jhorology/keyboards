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

void set_mac(bool value);
void set_usj(bool value);

#ifdef APPLE_FN_ENABLE
bool process_apple_ff_fkey(uint16_t fkey_index, keyrecord_t *record);
#endif

#ifdef VIAL_ENABLE
void pgm_memcpy(uint8_t *dest, uint8_t *src, size_t len);
#endif

//   global variavles
//------------------------------------------

common_kb_config_t g_common_kb_config;

#ifdef VIAL_ENABLE

#  ifdef VIAL_TAP_DANCE_ENABLE
/*
 * pre-defined vial tap dabce
 */
// clang-format off
const vial_tap_dance_entry_t PROGMEM vial_tap_dance_actions_default[] = {
  //                     tap,      hold,     double_tap,   tap_hold, tapping_term
  // for HHKB, Right Alt, on  tap hold: MO(3)
  [TD_RALT_MO3]        = { KC_RALT, KC_RALT,  KC_RALT,      MO(3),    TAPPING_TERM },
  // for mac,  Apple Fn/Globe + FK override, on tap: 英数, on double tap かな
  [TD_APFF_EISU_KANA] = { KC_LNG2, APPLE_FF, KC_LNG1,      APPLE_FF, TAPPING_TERM },
  // for mac/HHKB,Left Command, on tap: 英数, on double tap かな
  [TD_LCMD_EISU_KANA] = { KC_LNG2, KC_LGUI,  KC_LNG1,      KC_LGUI,  TAPPING_TERM },
  // for mac/HHKB, Left Option, on tap hold: Apple fn/globe + FK overrde
  [TD_LOPT_APFF]      = { KC_LALT, KC_LALT,  KC_LALT,      APPLE_FF, TAPPING_TERM },
  // for win/HHKB, Left Win, on double tap: Win + space
  [TD_LWIN_LANG]      = { KC_LGUI, KC_LGUI,  LGUI(KC_SPC), KC_LGUI,  TAPPING_TERM },
  // for win/HHKB, Left Alt, on tap: 英数, on double tap かな
  [TD_LALT_EISU_KANA] = { KC_LNG2, KC_LALT,  KC_LNG1,      KC_LALT, TAPPING_TERM }
};
// clang-format on
#  endif
#  ifdef VIAL_COMBO_ENABLE
/*
 * pre-defined vial combo
 */
const vial_combo_entry_t PROGMEM vial_combo_actions_default[] = {};
#  endif
#  ifdef VIAL_KEY_OVERRIDE_ENABLE
/*
 * pre-defined key overrride
 */
const vial_combo_entry_t PROGMEM vial_key_override_actions_default[] = {};
#  endif
#else

/*
 * pre-defined tap dabce
 * TODO not same instruction as VIAL
 */
qk_tap_dance_action_t tap_dance_actions[] = {
    // Tap once for standard key, twice to toggle layers
    [TD_RALT_MO3] = ACTION_TAP_DANCE_DOUBLE(KC_RALT, MO(3)),
    [TD_APFF_EISU_KANA] = ACTION_TAP_DANCE_DOUBLE(APPLE_FF, EJ_TOGG),
    [TD_LCMD_EISU_KANA] = ACTION_TAP_DANCE_DOUBLE(KC_LGUI, EJ_TOGG),
    [TD_LOPT_APFF] = ACTION_TAP_DANCE_DOUBLE(KC_LALT, APPLE_FF),
    [TD_LWIN_LANG] = ACTION_TAP_DANCE_DOUBLE(KC_LGUI, LGUI(KC_SPC)),
    [TD_LALT_EISU_KANA] = ACTION_TAP_DANCE_DOUBLE(KC_LALT, EJ_TOGG)};
#endif

//   local variables
//------------------------------------------

typedef union {
  uint16_t raw;
  struct {
#ifdef APPLE_FN_ENABLE
    bool apple_ff : 1;
    bool capslock_old : 1;
    uint16_t apple_ff_flags : 12;
#endif
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
  keyboard_post_init_user();
}

bool process_record_kb(uint16_t keycode, keyrecord_t *record) {
  if (!process_record_user(keycode, record)) return false;
  if (g_common_kb_config.usj) {
    if (!process_ansi_layout_on_apple_jis(keycode, record)) return false;
  }
  bool result = true;
  switch (keycode) {
    case MAC_TOGG:
      set_mac(!g_common_kb_config.mac);
      return false;
    case MAC_ON:
      set_mac(true);
      return false;
    case MAC_OFF:
      set_mac(false);
      return false;
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
        volatile_state.apple_ff = true;
        register_code(KC_APFN);
      } else {
        volatile_state.apple_ff = false;
        unregister_code(KC_APFN);
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
#endif
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
      set_usj(!g_common_kb_config.usj);
      return false;
    case USJ_ON:
      set_usj(true);
      return false;
    case USJ_OFF:
      set_usj(false);
      return false;
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

#ifdef VIAL_ENABLE
#  ifdef VIAL_TAP_DANCE_ENABLE
void vial_tap_dance_reset_kb(uint8_t index, vial_tap_dance_entry_t *entry) {
  if (index < TAP_DANCE_ACTIONS_DEFAULT_LENGTH) {
    pgm_memcpy((uint8_t *)entry, (uint8_t *)&vial_tap_dance_actions_default[index], sizeof(vial_tap_dance_entry_t));
  }
  vial_tap_dance_reset_user(index, entry);
}
#  endif

#  ifdef VIAL_COMBO_ENABLE
void vial_combo_reset_kb(uint8_t index, vial_combo_entry_t *entry) {
  if (index < VIAL_COMBO_ACTIONS_DEFAULT_LENGTH) {
    pgm_memcpy((uint8_t *)entry, (uint8_t *)&vial_combo_actions_default[index], sizeof(vial_combo_entry_t));
  }
  vial_combo_reset_user(index, entry);
}
#  endif

#  ifdef VIAL_KEY_OVERRIDE_ENABLE
void vial_key_override_reset_kb(uint8_t index, vial_key_override_entry_t *entry) {
  if (index < VIAL_KEY_OVERRIDE_ACTIONS_DEFAULT_LENGTH) {
    pgm_memcpy((uint8_t *)entry, (uint8_t *)&vial_key_override_actions_default[index],
               sizeof(vial_key_override_entry_t));
  }
  vial_key_override_reset_user(index, entry);
}
#  endif
#endif

//   local functions
//------------------------------------------

#ifdef APPLE_FN_ENABLE
bool process_apple_ff_fkey(uint16_t fkey_index, keyrecord_t *record) {
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
#endif

void set_mac(bool value) {
  if (value != g_common_kb_config.mac) {
    g_common_kb_config.mac = value;
    eeconfig_update_kb(g_common_kb_config.raw);
    // reboot for changing USB device descriptor
    soft_reset_keyboard();
  }
}

void set_usj(bool value) {
  if (value != g_common_kb_config.usj) {
    g_common_kb_config.usj = value;
    eeconfig_update_kb(g_common_kb_config.raw);
  }
}

#ifdef VIAL_ENABLE
void pgm_memcpy(uint8_t *dest, uint8_t *src, size_t len) {
  for (size_t i = 0; i < len; i++) {
    *dest++ = pgm_read_byte(&(*src++));
  }
}
#endif
