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
#include "my_keyboard_common.h"

#include "custom_config.h"
#include "os_detection.h"

/*
 * pre-defined apple fn functions for non-mac mode.
 */
const uint16_t PROGMEM non_mac_fn_keys_default[] = {
#ifdef MON_MAC_FN_KEYS_PREFER_LINUX
  // TODO Linux Gnome
  [FN_F1] = KC_BRID,   // decrease brightness
  [FN_F2] = KC_BRIU,   // increase brightness
  [FN_F3] = KC_NO,     // mission control -> TODO
  [FN_F4] = KC_NO,     // Spotlight -> TODO
  [FN_F5] = KC_NO,     // dictaion -> TODO
  [FN_F6] = KC_NO,     // Do not disturb -> TODO
  [FN_F7] = KC_MPRV,   // media prev
  [FN_F8] = KC_MPLY,   // media play/pause
  [FN_F9] = KC_MNXT,   // media next
  [FN_F10] = KC_MUTE,  // mute audio
  [FN_F11] = KC_VOLD,  // decrease audio volume
  [FN_F12] = KC_VOLU,  // increase audio volume
#else
  // Windows
  [FN_F1] = KC_BRID,    // decrease brightness
  [FN_F2] = KC_BRIU,    // increase brightness
  [FN_F3] = G(KC_TAB),  // mission control
  [FN_F4] = A(KC_SPC),  // Spotlight -> PowerToys Run
  [FN_F5] = G(KC_H),    // dictaion
  [FN_F6] = KC_NO,      // Do not disturb -> TODO
  [FN_F7] = KC_MPRV,    // media prev
  [FN_F8] = KC_MPLY,    // media play/pause
  [FN_F9] = KC_MNXT,    // media next
  [FN_F10] = KC_MUTE,   // mute audio
  [FN_F11] = KC_VOLD,   // decrease audio volume
  [FN_F12] = KC_VOLU,   // increase audio volume
  [FN_SPC] = KC_NO,     // Hey Siri -> TODO Cortana
  [FN_Q] = LAG(KC_N),   // Quick Notes
  [FN_E] = G(KC_DOT),   // Emoji & Symbols
  [FN_A] = G(KC_B),     // Focus Dock -> Focus Taskbar
  [FN_D] = G(KC_H),     // Dictation
  [FN_F] = KC_F11,      // Full screen mode
  [FN_H] = G(KC_COMM),  // Show Desktop
  [FN_C] = G(KC_A),     // Open Control Center -> Open Action Center
  [FN_N] = G(KC_N),     // Open Notification
  [FN_M] = KC_LALT,     // Focus menubar
#endif  // MON_MAC_FN_KEYS_PREFER_LINUX
};

combo_t key_combos[COMBO_COUNT] = {};

static bool proces_extra_keys(uint16_t keycode, keyrecord_t *record);

//  qmk custom hook functions
//------------------------------------------

void keyboard_pre_init_kb(void) {
  custom_config_init();
  keyboard_pre_init_user();
}

void via_init_kb(void) {
  if (!via_eeprom_is_valid()) {
    eeconfig_init_kb();
  }
}

void eeconfig_init_kb(void) {
  custom_config_reset();
  dynamic_tap_dance_reset();
  dynamic_non_mac_fn_reset(non_mac_fn_keys_default, sizeof(non_mac_fn_keys_default) >> 1);
  eeconfig_init_user();
}

// requires TAPPING_TERM_PER_KEY
uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
  uint16_t tapping_term;
  if ((tapping_term = tap_dance_get_tapping_term(keycode, record)) != 0) {
    return tapping_term;
  }
  return TAPPING_TERM;
}

void keyboard_post_init_kb(void) {
  tap_dance_actions_init();
  keyboard_post_init_user();
#if defined(MAC_BASE_LAYER) && defined(NON_MAC_BASE_LAYER)
  default_layer_set(custom_config_mac_is_enable() ? (1 << MAC_BASE_LAYER) : (1 << NON_MAC_BASE_LAYER));
#endif
}

void os_detection_update_kb(os_variant_t os) {
  if (custom_config_auto_detect_is_enable()) {
    custom_config_mac_set_enable(os == DARWIN);
  }
}

bool process_record_kb(uint16_t keycode, keyrecord_t *record) {
  return process_record_user(keycode, record) && proces_extra_keys(keycode, record) &&
         process_tap_dance_store_event(keycode, record) && process_record_custom_config(keycode, record) &&
         process_apple_fn(keycode, record) &&
#ifdef RADIAL_CONTROLLER_ENABLE
         process_radial_controller(keycode, record) &&
#endif
         process_jis_util(keycode, record);
}

__attribute__((weak)) bool raw_hid_receive_user(uint8_t *data, uint8_t length) { return true; }

void raw_hid_receive(uint8_t *data, uint8_t length) {
  if (custom_config_raw_hid_is_enable()) {
    if (raw_hid_receive_user(data, length)) {
      via_raw_hid_receive(data, length);
    }
  }
}

#ifdef RGB_MATRIX_ENABLE

bool rgb_matrix_indicators_kb(void) {
#  ifdef RGB_MATRIX_CAPS_LOCK_LED
  static bool capslock_prev;
  bool capslock = host_keyboard_led_state().caps_lock;
  if (capslock || capslock_prev) {
    rgb_matrix_set_color(RGB_MATRIX_CAPS_LOCK_LED, capslock ? rgb_matrix_get_val() : 0, 0, 0);
  }
  capslock_prev = capslock;
#  endif

  return rgb_matrix_indicators_user();
}

void suspend_power_down_kb(void) {
  rgb_matrix_set_suspend_state(true);
  suspend_power_down_user();
}

void suspend_wakeup_init_kb(void) {
  suspend_wakeup_init_user();
  rgb_matrix_set_suspend_state(false);
}
#endif

static bool proces_extra_keys(uint16_t keycode, keyrecord_t *record) {
  switch (keycode) {
    case TERM_LCK:
      host_consumer_send(record->event.pressed ? AL_LOCK : 0);
      return false;
#ifdef OS_DETECTION_DEBUG_ENABLE
    case TEST_OS:
      if (record->event.pressed) {
        send_os_fingerprint();
      }
      return false;
#endif
  }
  return true;
}
