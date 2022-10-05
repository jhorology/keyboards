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

#include "apple_fn.h"
#include "jis_util.h"
#ifdef RADIAL_CONTROLLER_ENABLE
#  include "radial_controller.h"
#endif
#ifndef VIAL_ENABLE
#  include "tap_dance.h"
#endif
#include "custom_config.h"

/*
 * pre-defined tap dance
 *
 * [0] single tap
 * [1] single hold
 * [2] multi tap
 * [3] tap hold
 * [4] tapping term
 */
#ifndef VIAL_ENABLE
const tap_dance_entry_t PROGMEM via_tap_dance_entries_default[] = {
#endif
#ifdef VIAL_TAP_DANCE_ENABLE
    const vial_tap_dance_entry_t PROGMEM vial_tap_dance_actions_default[] = {
#endif
#if defined(VIAL_TAP_DANCE_ENABLE) || !defined(VIAL_ENABLE)
        // for HHKB Right Alt, Alt + layer switch
        [TD_RALT_MO3] = {KC_RALT, KC_RALT, KC_RALT, MO(3), TAPPING_TERM},

        // for HHKB Left Alt, Alt + Apple fn + IME switch
        [TD_LALT_APFF_EISU_KANA] = {KC_LNG2, KC_LALT, KC_LNG1, APPLE_FF, TAPPING_TERM},

        // Apple Fn key + IME switch
        [TD_APFF_EISU_KANA] = {KC_LNG2, APPLE_FF, KC_LNG1, APPLE_FF, TAPPING_TERM}};
#endif

/*
 * pre-defined combo
 */
#ifdef VIAL_COMBO_ENABLE
const vial_combo_entry_t PROGMEM vial_combo_actions_default[] = {};
#endif

#ifndef VIAL_ENABLE
combo_t key_combos[COMBO_COUNT] = {};
#endif

#ifdef VIAL_KEY_OVERRIDE_ENABLE
/*
 * pre-defined key overrride
 */
const vial_combo_entry_t PROGMEM vial_key_override_actions_default[] = {};
#endif

//  qmk/vial custom hook functsions
//------------------------------------------

void via_init_kb(void) {
  if (!via_eeprom_is_valid()) {
    eeconfig_init_kb();
  }
}

void eeconfig_init_kb(void) {
  custom_config_reset();
#ifndef VIAL_ENABLE
  dynamic_tap_dance_reset(via_tap_dance_entries_default, TAP_DANCE_PRE_DEFINED_LENGTH);
#endif
  eeconfig_init_user();
}

#ifndef VIAL_ENABLE
// requires TAPPING_TERM_PER_KEY
uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
  uint16_t tapping_term;
  if ((tapping_term = tap_dance_get_tapping_term(keycode, record)) != 0) {
    return tapping_term;
  }
  return TAPPING_TERM;
}
#endif

void keyboard_pre_init_kb(void) {
  custom_config_init();
  keyboard_pre_init_user();
}

void keyboard_post_init_kb(void) {
  default_layer_set(custom_config_is_mac() ? 1 : 2);

#ifndef VIAL_ENABLE
  tap_dance_actions_init();
#endif

  keyboard_post_init_user();
}

bool process_record_kb(uint16_t keycode, keyrecord_t *record) {
  return process_record_user(keycode, record) &&
#ifndef VIAL_ENABLE
         process_tap_dance_store_event(keycode, record) &&
#endif
         process_apple_fn(keycode, record) &&
#ifdef RADIAL_CONTROLLER_ENABLE
         process_radial_controller(keycode, record) &&
#endif
         process_ansi_layout_on_jis(keycode, record);
}

#ifndef VIAL_ENABLE
void raw_hid_receive(uint8_t *data, uint8_t length) {
  // TODO access control
  via_raw_hid_receive(data, length);
}
#endif

#ifdef RGB_MATRIX_ENABLE

void rgb_matrix_indicators_kb(void) {
#  ifdef RGB_MATRIX_CAPS_LOCK_LED
  static bool capslock_prev;
  bool capslock = host_keyboard_led_state().caps_lock;
  if (capslock || capslock_prev) {
    rgb_matrix_set_color(RGB_MATRIX_CAPS_LOCK_LED, capslock ? rgb_matrix_get_val() : 0, 0, 0);
  }
  capslock_prev = capslock;
#  endif

  rgb_matrix_indicators_user();
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

#ifdef VIAL_TAP_DANCE_ENABLE
void vial_tap_dance_reset_kb(uint8_t index, vial_tap_dance_entry_t *entry) {
  if (index < TAP_DANCE_PRE_DEFINED_LENGTH) {
    pgm_memcpy(entry, &vial_tap_dance_actions_default[index], sizeof(vial_tap_dance_entry_t));
  }
  vial_tap_dance_reset_user(index, entry);
}
#endif

#ifdef VIAL_COMBO_ENABLE
void vial_combo_reset_kb(uint8_t index, vial_combo_entry_t *entry) {
  if (index < COMBO_PRE_DEFINED_LENGTH) {
    pgm_memcpy(entry, &vial_combo_actions_default[index], sizeof(vial_combo_entry_t));
  }
  vial_combo_reset_user(index, entry);
}
#endif

#ifdef VIAL_KEY_OVERRIDE_ENABLE
void vial_key_override_reset_kb(uint8_t index, vial_key_override_entry_t *entry) {
  if (index < KEY_OVERRIDE_PRE_DEFINED_LENGTH) {
    pgm_memcpy(entry, &vial_key_override_actions_default[index], sizeof(vial_key_override_entry_t));
  }
  vial_key_override_reset_user(index, entry);
}
#endif
