/* Copyright 2022 masafumi
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

#include <eeprom.h>

#include "custom_keycodes.h"
#include "tap_dance.h"

#ifndef CUSTOM_CONFIG_RHID_DEFAULT
#  define CUSTOM_CONFIG_RHID_DEFAULT false
#endif
#ifndef CUSTOM_CONFIG_MAC_DEFAULT
#  define CUSTOM_CONFIG_MAC_DEFAULT true
#endif
#ifndef CUSTOM_CONFIG_AUTO_DETECT_DEFAULT
#  define CUSTOM_CONFIG_AUTO_DETECT_DEFAULT true
#endif
#ifndef CUSTOM_CONFIG_USJ_DEFAULT
#  define CUSTOM_CONFIG_USJ_DEFAULT false
#endif
#ifndef CUSTOM_CONFIG_NON_MAC_FN_FKEY_DEFAULT
#  define CUSTOM_CONFIG_NON_MAC_FN_FKEY_DEFAULT true
#endif
#ifndef CUSTOM_CONFIG_NON_MAC_FN_ALPHA_DEFAULT
#  define CUSTOM_CONFIG_NON_MAC_FN_ALPHA_DEFAULT true
#endif
#ifndef CUSTOM_CONFIG_NON_MAC_FN_CURSOR_DEFAULT
#  define CUSTOM_CONFIG_NON_MAC_FN_CURSOR_DEFAULT true
#endif

#ifdef RADIAL_CONTROLLER_ENABLE
#  ifndef RADIAL_CONTROLLER_ENCODER_CLICKS_DEFAULT
#    define RADIAL_CONTROLLER_ENCODER_CLICKS_DEFAULT 36
#  endif
#  ifndef RADIAL_CONTROLLER_KEY_ANGULAR_SPEED_DEFAULT
#    define RADIAL_CONTROLLER_KEY_ANGULAR_SPEED_DEFAULT 90
#  endif
#  ifndef RADIAL_CONTROLLER_FINE_TUNE_RATIO_DEFAULT
#    define RADIAL_CONTROLLER_FINE_TUNE_RATIO_DEFAULT 2
#  endif
// additional conbination of modfier for RC fine-tune.
// ctrl, bit1: shift, bit2: alt, bit3: gui, bit4: fn🌐
#  ifndef RADIAL_CONTROLLER_FINE_TUNE_MODS_DEFAULT
#    define RADIAL_CONTROLLER_FINE_TUNE_MODS_DEFAULT 0x10
#  endif
#  define RADIAL_CONTROLLER_KEY_ANGULAR_SPEED_OFFSET 15
#endif

extern const tap_dance_entry_t PROGMEM tap_dance_predefined_entries[];

kb_config_t kb_config;
#ifdef RADIAL_CONTROLLER_ENABLE
rc_config_t rc_config;
#endif

static void _custom_config_raw_hid_set_enable(bool enable);
static void _custom_config_mac_set_enable(bool enable);
static void _custom_config_usj_set_enable(bool enable);

void custom_config_reset() {
  kb_config.raw = 0;
  kb_config.raw_hid = CUSTOM_CONFIG_RHID_DEFAULT;
  kb_config.mac = CUSTOM_CONFIG_MAC_DEFAULT;
  kb_config.auto_detect = CUSTOM_CONFIG_AUTO_DETECT_DEFAULT;
  kb_config.usj = CUSTOM_CONFIG_USJ_DEFAULT;
  kb_config.non_mac_fn_fkey = CUSTOM_CONFIG_NON_MAC_FN_FKEY_DEFAULT;
  kb_config.non_mac_fn_alpha = CUSTOM_CONFIG_NON_MAC_FN_ALPHA_DEFAULT;
  kb_config.non_mac_fn_cursor = CUSTOM_CONFIG_NON_MAC_FN_CURSOR_DEFAULT;

  eeconfig_update_kb(kb_config.raw);
#ifdef RADIAL_CONTROLLER_ENABLE
  rc_config.raw = 0;
  rc_config.encoder_clicks = RADIAL_CONTROLLER_ENCODER_CLICKS_DEFAULT;
  rc_config.key_angular_speed =
    RADIAL_CONTROLLER_KEY_ANGULAR_SPEED_DEFAULT - RADIAL_CONTROLLER_KEY_ANGULAR_SPEED_OFFSET;
  rc_config.fine_tune_ratio = RADIAL_CONTROLLER_FINE_TUNE_RATIO_DEFAULT;
  rc_config.fine_tune_mods = RADIAL_CONTROLLER_FINE_TUNE_MODS_DEFAULT;
  nvm_via_update_dword(VIA_RADIAL_CONTROLLER_EEPROM_OFFSET, rc_config.raw);
#endif
}

void custom_config_init() {
  kb_config.raw = eeconfig_read_kb();
#ifdef RADIAL_CONTROLLER_ENABLE
  rc_config.raw = nvm_via_read_dword(VIA_RADIAL_CONTROLLER_EEPROM_OFFSET);
#endif
#ifdef CUSTOM_CONFIG_RHID_MODE_PIN
  setPinOutput(CUSTOM_CONFIG_RHID_MODE_PIN);
#endif
#ifdef CUSTOM_CONFIG_MAC_MODE_PIN
  // write pin at initialize time only. mac mode is never changed without restart.
  setPinOutput(CUSTOM_CONFIG_MAC_MODE_PIN);
  writePin(CUSTOM_CONFIG_MAC_MODE_PIN, kb_config.mac);
#endif
#ifdef CUSTOM_CONFIG_USJ_MODE_PIN
  setPinOutput(CUSTOM_CONFIG_USJ_MODE_PIN);
#endif
#ifdef CUSTOM_CONFIG_FORCE_RHID
  _custom_config_raw_hid_set_enable(CUSTOM_CONFIG_FORCE_RHID);
#endif
#ifdef CUSTOM_CONFIG_FORCE_USJ
  _custom_config_usj_set_enable(CUSTOM_CONFIG_FORCE_USJ);
#endif
}

bool process_record_custom_config(uint16_t keycode, keyrecord_t *record) {
  if (record->event.pressed) {
    bool normal_true = get_mods() & MOD_MASK_SHIFT ? false : true;
    bool normal_false = !normal_true;
    switch (keycode) {
      case RHID_ON:
        custom_config_raw_hid_set_enable(normal_true);
        return false;
      case RHID_OFF:
        custom_config_raw_hid_set_enable(normal_false);
        return false;
#ifndef DIP_SWITCH_ENABLE
      case MAC_ON:
        // *keyboard may restart
        custom_config_mac_set_enable(normal_true);
        return false;
      case MAC_OFF:
        // *keyboard may restart
        custom_config_mac_set_enable(normal_false);
        return false;
#endif
      case AUT_ON:
        // *keyboard may restart
        custom_config_auto_detect_set_enable(normal_true);
        return false;
      case AUT_OFF:
        custom_config_auto_detect_set_enable(normal_false);
        return false;
      case USJ_ON:
        custom_config_usj_set_enable(normal_true);
        return false;
      case USJ_OFF:
        custom_config_usj_set_enable(normal_false);
        return false;
    }
  }
  return true;
}

bool custom_config_raw_hid_is_enable() { return kb_config.raw_hid; }

void custom_config_raw_hid_toggle_enable() { custom_config_raw_hid_set_enable(!kb_config.raw_hid); }

static void _custom_config_raw_hid_set_enable(bool enable) {
  kb_config.raw_hid = enable;
#ifdef CUSTOM_CONFIG_RHID_MODE_PIN
  writePin(CUSTOM_CONFIG_RHID_MODE_PIN, enable);
#endif
}

void custom_config_raw_hid_set_enable(bool enable) {
  if (enable != kb_config.raw_hid) {
    _custom_config_raw_hid_set_enable(enable);
#ifndef CUSTOM_CONFIG_FORCE_RHID
    eeconfig_update_kb(kb_config.raw);
#endif
  }
}

bool custom_config_usj_is_enable() { return kb_config.usj; }

void custom_config_usj_toggle_enable() { custom_config_usj_set_enable(!kb_config.usj); }

static void _custom_config_usj_set_enable(bool enable) {
  kb_config.usj = enable;
#ifdef CUSTOM_CONFIG_USJ_MODE_PIN
  writePin(CUSTOM_CONFIG_USJ_MODE_PIN, enable);
#endif
}

void custom_config_usj_set_enable(bool enable) {
  if (enable != kb_config.usj) {
    _custom_config_usj_set_enable(enable);
#ifndef CUSTOM_CONFIG_FORCE_USJ
    eeconfig_update_kb(kb_config.raw);
#endif
  }
}

bool custom_config_mac_is_enable() { return kb_config.mac; }

void custom_config_mac_toggle_enable() { custom_config_mac_set_enable(!kb_config.mac); }

static void _custom_config_mac_set_enable(bool enable) { kb_config.mac = enable; }

void custom_config_mac_set_enable(bool enable) {
  if (enable != kb_config.mac) {
    _custom_config_mac_set_enable(enable);
    eeconfig_update_kb(kb_config.raw);
    // reboot for changing USB device descriptor
    soft_reset_keyboard();
  }
}
void custom_config_mac_set_enable_without_reset(bool enable) {
  if (enable != kb_config.mac) {
    _custom_config_mac_set_enable(enable);
    eeconfig_update_kb(kb_config.raw);
  }
}

bool custom_config_auto_detect_is_enable() { return kb_config.auto_detect; }

void custom_config_auto_detect_toggle_enable() {
  custom_config_auto_detect_set_enable(!kb_config.auto_detect);
}

void custom_config_auto_detect_set_enable(bool enable) {
  if (enable != kb_config.auto_detect) {
    kb_config.auto_detect = enable;
    eeconfig_update_kb(kb_config.raw);
    if (enable) {
      soft_reset_keyboard();
    }
  }
}

uint8_t custom_config_non_mac_fn_fkey_is_enable() { return kb_config.non_mac_fn_fkey; }

void custom_config_non_mac_fn_set_fkey(bool enable) {
  if (enable != kb_config.non_mac_fn_fkey) {
    kb_config.non_mac_fn_fkey = enable;
    eeconfig_update_kb(kb_config.raw);
  }
}

uint8_t custom_config_non_mac_fn_alpha_is_enable() { return kb_config.non_mac_fn_alpha; }

void custom_config_non_mac_fn_set_alpha(bool enable) {
  if (enable != kb_config.non_mac_fn_alpha) {
    kb_config.non_mac_fn_alpha = enable;
    eeconfig_update_kb(kb_config.raw);
  }
}
uint8_t custom_config_non_mac_fn_cursor_is_enable() { return kb_config.non_mac_fn_cursor; }

void custom_config_non_mac_fn_set_cursor(bool enable) {
  if (enable != kb_config.non_mac_fn_cursor) {
    kb_config.non_mac_fn_cursor = enable;
    eeconfig_update_kb(kb_config.raw);
  }
}

// radial controller

#ifdef RADIAL_CONTROLLER_ENABLE
uint8_t custom_config_rc_get_encoder_clicks() { return rc_config.encoder_clicks; }

uint16_t custom_config_rc_get_key_angular_speed() {
  return (uint16_t)rc_config.key_angular_speed + RADIAL_CONTROLLER_KEY_ANGULAR_SPEED_OFFSET;
}

// returns divider power-of-2
uint8_t custom_config_rc_get_fine_tune_ratio() { return (uint16_t)rc_config.fine_tune_ratio; }

uint8_t custom_config_rc_is_fine_tune_mods(void) { return (uint16_t)rc_config.fine_tune_mods; }

bool custom_config_rc_is_fine_tune_mods_now() {
  uint16_t mods = get_mods();
  if (rc_config.fine_tune_ratio && rc_config.fine_tune_mods) {
    uint8_t cur_mods = (mods & MOD_MASK_CTRL ? 1 : 0) + (mods & MOD_MASK_SHIFT ? 2 : 0) +
                       (mods & MOD_MASK_ALT ? 4 : 0) + (mods & MOD_MASK_GUI ? 8 : 0) +
                       (host_apple_is_pressed(1 << USAGE_INDEX_AVT_KEYBOARD_FN) ? 16 : 0);
    return (cur_mods & rc_config.fine_tune_mods) == rc_config.fine_tune_mods;
  }
  return false;
}
#endif  // RADIAL_CONTROLLER_ENABLE

// dynamic tap dance

void dynamic_tap_dance_reset() {
  tap_dance_entry_t initial_data = {0};
  tap_dance_entry_t data;
  initial_data.tapping_term = TAPPING_TERM;
  for (uint8_t i = 0; i < TAP_DANCE_ENTRIES; i++) {
    if (i < NUM_TAP_DANCE_PREDEFINED_ENTRIES) {
      memcpy_P(&data, &tap_dance_predefined_entries[i], sizeof(tap_dance_entry_t));
    } else {
      data = initial_data;
    }
    nvm_via_update_custom_config(
      (void *)&data, VIA_DYNAMIC_TAP_DANCE_EEPROM_OFFSET + sizeof(tap_dance_entry_t) * i,
      sizeof(tap_dance_entry_t));
  }
}

uint16_t dynamic_tap_dance_keycode(uint8_t index, tap_dance_event_t event) {
  uint16_t keycode = KC_NO;
  if (index < TAP_DANCE_ENTRIES) {
    switch (event) {
      case TD_SINGLE_TAP ... TD_TAP_HOLD:
        keycode =
          nvm_via_read_word(VIA_DYNAMIC_TAP_DANCE_EEPROM_OFFSET +
                            sizeof(tap_dance_entry_t) * index + (event - TD_SINGLE_TAP) * 2);
      default:
        break;
    }
  }
#ifdef CONSOLE_ENABLE
  uprintf("dynamic_tap_dance_keycode:td_index:%d event:%d keycode:%04X\n", index, event, keycode);
#endif
  return keycode;
}

uint16_t dynamic_tap_dance_tapping_term(uint8_t index) {
  uint16_t tapping_term =
    (index < TAP_DANCE_ENTRIES
       ? nvm_via_read_word(VIA_DYNAMIC_TAP_DANCE_EEPROM_OFFSET + 8 + 10 * index)
       : TAPPING_TERM) &
    0x03ff;  // highest 6bits are reserved for future use
#ifdef CONSOLE_ENABLE
  uprintf("dynamic_tap_dance_tapping_term:td_index:%d tapping_term:%d\n", index, tapping_term);
#endif
  return tapping_term;
}

// none mac fn functions

void dynamic_non_mac_fn_reset(const uint16_t *keycodes, uint8_t len) {
  for (uint8_t i = 0; i < NON_MAC_FN_KEY_ENTRIES; i++) {
    nvm_via_update_word(VIA_DYNAMIC_NON_MAC_FN_EEPROM_OFFSET + i * 2,
                        i < len ? pgm_read_word(&keycodes[i]) : KC_NO);
  }
}

uint16_t dynamic_non_mac_fn_keycode(non_mac_fn_key_t fn_key) {
  return nvm_via_read_word(VIA_DYNAMIC_NON_MAC_FN_EEPROM_OFFSET + fn_key * 2);
}
