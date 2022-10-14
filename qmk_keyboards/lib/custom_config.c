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
#include QMK_KEYBOARD_H
#include "custom_config.h"

#include <print.h>

#include "eeprom.h"
#include "lib/custom_keycodes.h"

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
#  ifndef RADIAL_CONTROLLER_FINE_TUNE_MOD_CTRL_DEFAULT
#    define RADIAL_CONTROLLER_FINE_TUNE_MOD_CTRL_DEFAULT false
#  endif
#  ifndef RADIAL_CONTROLLER_FINE_TUNE_MOD_SHIFT_DEFAULT
#    define RADIAL_CONTROLLER_FINE_TUNE_MOD_SHIFT_DEFAULT true
#  endif
#  ifndef RADIAL_CONTROLLER_FINE_TUNE_MOD_ALT_DEFAULT
#    define RADIAL_CONTROLLER_FINE_TUNE_MOD_ALT_DEFAULT false
#  endif
#  ifndef RADIAL_CONTROLLER_FINE_TUNE_MOD_GUI_DEFAULT
#    define RADIAL_CONTROLLER_FINE_TUNE_MOD_GUI_DEFAULT false
#  endif
#  define RADIAL_CONTROLLER_KEY_ANGULAR_SPEED_OFFSET 15
#endif

typedef union {
  uint32_t raw;
  struct {
    bool raw_hid : 1;  // allow access to raw hid
    bool mac : 1;      // mac mode.
    bool usj : 1;      // ANSI layou on JIS.
  };
} kb_config_t;
static kb_config_t kb_config;

#ifdef RADIAL_CONTROLLER_ENABLE
typedef union {
  uint32_t raw;
  struct {
    uint8_t encoder_clicks;       // encoder clicks per rotation
    uint8_t key_angular_speed;    // degree per second, 15 - 270 (offset 15)
    uint8_t fine_tune_ratio : 2;  // power-of-2 divider 0: none, 1: 1/2, 2:1/4, 3:1/8
    bool fine_tune_mod_ctrl : 1;
    bool fine_tune_mod_shift : 1;
    bool fine_tune_mod_alt : 1;
    bool fine_tune_mod_gui : 1;
  };
} rc_config_t;
static rc_config_t rc_config;
#endif

static void _custom_config_raw_hid_set_enable(bool enable);
static void _custom_config_mac_set_enable(bool enable);
static void _custom_config_usj_set_enable(bool enable);

void custom_config_reset() {
  kb_config.raw = 0;
  kb_config.mac = true;
  eeconfig_update_kb(kb_config.raw);
#ifdef RADIAL_CONTROLLER_ENABLE
  rc_config.raw = 0;
  rc_config.encoder_clicks = RADIAL_CONTROLLER_ENCODER_CLICKS_DEFAULT;
  rc_config.key_angular_speed =
      RADIAL_CONTROLLER_KEY_ANGULAR_SPEED_DEFAULT - RADIAL_CONTROLLER_KEY_ANGULAR_SPEED_OFFSET;
  rc_config.fine_tune_ratio = RADIAL_CONTROLLER_FINE_TUNE_RATIO_DEFAULT;
  rc_config.fine_tune_mod_ctrl = RADIAL_CONTROLLER_FINE_TUNE_MOD_CTRL_DEFAULT;
  rc_config.fine_tune_mod_shift = RADIAL_CONTROLLER_FINE_TUNE_MOD_SHIFT_DEFAULT;
  rc_config.fine_tune_mod_alt = RADIAL_CONTROLLER_FINE_TUNE_MOD_ALT_DEFAULT;
  rc_config.fine_tune_mod_gui = RADIAL_CONTROLLER_FINE_TUNE_MOD_GUI_DEFAULT;
  eeprom_update_dword((uint32_t *)RADIAL_CONTROLLER_EEPROM_ADDR, rc_config.raw);
#endif
}

void custom_config_init() {
  kb_config.raw = eeconfig_read_kb();
#ifdef RADIAL_CONTROLLER_ENABLE
  rc_config.raw = eeprom_read_dword((uint32_t *)RADIAL_CONTROLLER_EEPROM_ADDR);
#endif
#ifdef CUSTOM_CONFIG_MAC_MODE_PIN
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
    switch (keycode) {
      case RHID_TOGG:
        custom_config_raw_hid_toggle_enable();
        return false;
      case RHID_ON:
        custom_config_raw_hid_set_enable(true);
        return false;
      case RHID_OFF:
        custom_config_raw_hid_set_enable(false);
        return false;
      case MAC_TOGG:
        custom_config_mac_toggle_enable();
        return false;
      case MAC_ON:
        custom_config_mac_set_enable(true);
        return false;
      case MAC_OFF:
        custom_config_mac_set_enable(false);
        return false;
      case USJ_TOGG:
        custom_config_usj_toggle_enable();
        return false;
      case USJ_ON:
        custom_config_usj_set_enable(true);
        return false;
      case USJ_OFF:
        custom_config_usj_set_enable(false);
        return false;
    }
  }
  return true;
}

#if VIA_VERSION == 3
void via_custom_value_command_kb(uint8_t *data, uint8_t length) {
  // data = [ command_id, channel_id, value_id, value_data ]
  switch (data[1]) {
#  ifdef RADIAL_CONTROLLER_ENABLE
    case id_custom_rc_channel:
      switch (data[0]) {
        case id_custom_set_value:
          via_custom_rc_set_value(data[2], &(data[3]));
          return;
        case id_custom_get_value:
          via_custom_rc_get_value(data[2], &(data[3]));
          return;
        case id_custom_save:
          via_custom_rc_save();
          return;
      }
#  endif
    case id_custom_td_channel_start ... id_custom_td_channel_end:
      switch (data[0]) {
        case id_custom_set_value:
          via_custom_td_set_value(data[1] - id_custom_td_channel_start, data[2], &(data[3]));
          return;
        case id_custom_get_value:
          via_custom_td_get_value(data[1] - id_custom_td_channel_start, data[2], &(data[3]));
          return;
        case id_custom_save:
          via_custom_td_save(data[1] - id_custom_td_channel_start);
          return;
      }
  }
  // Return the unhandled state
  data[0] = id_unhandled;
}
#endif  // VIA_VERSION == 3

bool custom_config_raw_hid_is_enable() { return kb_config.raw_hid; }

void custom_config_raw_hid_toggle_enable() { custom_config_raw_hid_set_enable(!kb_config.raw_hid); }

static void _custom_config_raw_hid_set_enable(bool enable) { kb_config.raw_hid = enable; }

void custom_config_raw_hid_set_enable(bool enable) {
  if (enable != kb_config.raw_hid) {
    _custom_config_raw_hid_set_enable(enable);
#ifndef CUSTOM_CONFIG_FORCE_RHID
    eeconfig_update_kb(kb_config.raw);
#endif
  }
}

bool custom_config_mac_is_enable() { return kb_config.mac; }

void custom_config_mac_toggle_enable() { custom_config_mac_set_enable(!kb_config.mac); }

static void _custom_config_mac_set_enable(bool mac) { kb_config.mac = mac; }

void custom_config_mac_set_enable(bool mac) {
  if (mac != kb_config.mac) {
    _custom_config_mac_set_enable(mac);
    eeconfig_update_kb(kb_config.raw);
    // reboot for changing USB device descriptor
    soft_reset_keyboard();
  }
}

bool custom_config_usj_is_enable() { return kb_config.usj; }

void custom_config_usj_toggle_enable() { custom_config_usj_set_enable(!kb_config.usj); }

static void _custom_config_usj_set_enable(bool usj) {
  kb_config.usj = usj;
#ifdef CUSTOM_CONFIG_USJ_MODE_PIN
  writePin(CUSTOM_CONFIG_USJ_MODE_PIN, usj);
#endif
}

void custom_config_usj_set_enable(bool usj) {
  if (usj != kb_config.usj) {
    _custom_config_usj_set_enable(usj);
#ifndef CUSTOM_CONFIG_FORCE_USJ
    eeconfig_update_kb(kb_config.raw);
#endif
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

bool custom_config_rc_is_fine_tune_mod() {
  uint16_t mods = get_mods();
  // fine-tune off
  if (!rc_config.fine_tune_ratio) return false;
  // need modifier at least one.
  if (!(rc_config.fine_tune_mod_ctrl || rc_config.fine_tune_mod_ctrl || rc_config.fine_tune_mod_ctrl ||
        rc_config.fine_tune_mod_ctrl))
    return false;
  if (rc_config.fine_tune_mod_ctrl && !(mods & MOD_MASK_CTRL)) return false;
  if (rc_config.fine_tune_mod_shift && !(mods & MOD_MASK_SHIFT)) return false;
  if (rc_config.fine_tune_mod_alt && !(mods & MOD_MASK_ALT)) return false;
  if (rc_config.fine_tune_mod_gui && !(mods & MOD_MASK_GUI)) return false;
  return true;
}

#  if VIA_VERSION == 3
void via_custom_rc_get_value(uint8_t value_id, uint8_t *value_data) {
  switch (value_id) {
    case id_custom_rc_encoder_clicks:
      value_data[0] = rc_config.encoder_clicks;
      break;
    case id_custom_rc_key_angular_speed:
      value_data[0] = rc_config.key_angular_speed;
      break;
    case id_custom_rc_fine_tune_ratio:
      value_data[0] = rc_config.fine_tune_ratio;
      break;
    case id_custom_rc_fine_tune_mod_ctrl:
      value_data[0] = rc_config.fine_tune_mod_ctrl ? 1 : 0;
      break;
    case id_custom_rc_fine_tune_mod_shift:
      value_data[0] = rc_config.fine_tune_mod_shift ? 1 : 0;
      break;
    case id_custom_rc_fine_tune_mod_alt:
      value_data[0] = rc_config.fine_tune_mod_alt ? 1 : 0;
      break;
    case id_custom_rc_fine_tune_mod_gui:
      value_data[0] = rc_config.fine_tune_mod_gui ? 1 : 0;
      break;
  }
#    ifdef CONSOLE_ENABLE
  uprintf("via_custom_rc_get_value:value_id:%d value:%02X %02X\n", value_id, value_data[0], value_data[1]);
#    endif
}

void via_custom_rc_set_value(uint8_t value_id, uint8_t *value_data) {
#    ifdef CONSOLE_ENABLE
  uprintf("via_custom_rc_set_value:value_id:%d value:%02X %02X\n", value_id, value_data[0], value_data[1]);
#    endif
  switch (value_id) {
    case id_custom_rc_encoder_clicks:
      rc_config.encoder_clicks = value_data[0];
      break;
    case id_custom_rc_key_angular_speed:
      rc_config.key_angular_speed = value_data[0];
      break;
    case id_custom_rc_fine_tune_ratio:
      rc_config.fine_tune_ratio = value_data[0];
      break;
    case id_custom_rc_fine_tune_mod_ctrl:
      rc_config.fine_tune_mod_ctrl = value_data[0];
      break;
    case id_custom_rc_fine_tune_mod_shift:
      rc_config.fine_tune_mod_shift = value_data[0];
      break;
    case id_custom_rc_fine_tune_mod_alt:
      rc_config.fine_tune_mod_alt = value_data[0];
      break;
    case id_custom_rc_fine_tune_mod_gui:
      rc_config.fine_tune_mod_gui = value_data[0];
      break;
  }
}

void via_custom_rc_save() {
#    ifdef CONSOLE_ENABLE
  uprintf("via_custom_rc_save_value\n");
#    endif
  eeprom_update_dword((uint32_t *)RADIAL_CONTROLLER_EEPROM_ADDR, rc_config.raw);
}

#  endif  // VIA_VERSION == 3
#endif    // RADIAL_CONTROLLER_ENABLE

// dynamic tap dance

void dynamic_tap_dance_reset(const tap_dance_entry_t *entry, uint8_t len) {
  tap_dance_entry_t empty = {0};
  tap_dance_entry_t data;
  for (int i = 0; i < TAP_DANCE_ENTRIES; i++) {
    pgm_memcpy(&data, &entry[i], 10);
    eeprom_update_block((uint8_t *)(i < len ? &data : &empty), (uint8_t *)(DYNAMIC_TAP_DANCE_EEPROM_ADDR + 10 * i), 10);
  }
}

uint16_t dynamic_tap_dance_keycode(uint16_t index, tap_dance_state_t state) {
  uint16_t keycode = KC_NO;
  if (index < TAP_DANCE_ENTRIES) {
    switch (state) {
      case TD_SINGLE_TAP ... TD_TAP_HOLD:
        keycode = eeprom_read_word((uint16_t *)(DYNAMIC_TAP_DANCE_EEPROM_ADDR + 10 * index + (state - 1) * 2));
      default:
        break;
    }
  }
#ifdef CONSOLE_ENABLE
  uprintf("dynamic_tap_dance_keycode:td_index:%d state:%d keycode:%04X\n", index, state, keycode);
#endif
  return keycode;
}

uint16_t dynamic_tap_dance_tapping_term(uint16_t index) {
  uint16_t tapping_term = index < TAP_DANCE_ENTRIES
                              ? eeprom_read_word((uint16_t *)(DYNAMIC_TAP_DANCE_EEPROM_ADDR + 8 + 10 * index))
                              : TAPPING_TERM;
#ifdef CONSOLE_ENABLE
  uprintf("dynamic_tap_dance_tapping_term:td_index:%d tapping_term:%d\n", index, tapping_term);
#endif
  return tapping_term;
}

#if VIA_VERSION == 3

// TODO keycode control dosen't recognize 16bit keycode, maybe bug.
// TODO slider control dosen't support value greater than 255
void via_custom_td_get_value(uint8_t td_index, uint8_t value_id, uint8_t *value_data) {
  uint16_t keycode;
  switch (value_id) {
    case id_custom_td_single_tap ... id_custom_td_tap_hold:
      keycode = dynamic_tap_dance_keycode(td_index, value_id);
      // should be BE
      // value_data[0] = keycode >> 8;
      // value_data[1] = keycode & 0xff;
      // temporary fix: 8bit code only
      value_data[0] = keycode & 0xff;
      break;
    case id_custom_td_tapping_term:
      // temporary fix: self ranged value (0...1024)ms -> (0...255)
      value_data[0] = dynamic_tap_dance_tapping_term(td_index) >> 2;
      break;
  }
#  ifdef CONSOLE_ENABLE
  uprintf("via_custom_td_get_value:td_index:%d value_id:%d value:%02X %02X\n", td_index, value_id, value_data[0],
          value_data[1]);
#  endif
}

// TODO keycode control dosen't support 16bit keycode, maybe bug.
// TODO slider control dosen't support value greater than 255
void via_custom_td_set_value(uint8_t td_index, uint8_t value_id, uint8_t *value_data) {
  uint16_t *adrs = (uint16_t *)(DYNAMIC_TAP_DANCE_EEPROM_ADDR + 10 * td_index);
#  ifdef CONSOLE_ENABLE
  uprintf("via_custom_td_set_value:td_index:%d value_id:%d value:%02X %02X\n", td_index, value_id, value_data[0],
          value_data[1]);
#  endif
  if (td_index < TAP_DANCE_ENTRIES) {
    switch (value_id) {
      case id_custom_td_single_tap ... id_custom_td_tap_hold:
        // temporary comment out: break 16 bit keycode.
        // eeprom_update_word(adrs + value_id - 1, ((uint16_t)value_data[0] << 8) + value_data[1]);
        break;
      case id_custom_td_tapping_term:
        // TODO should reduce the number of writing times
        // temporary fix: self ranged value (0...255) -> (0...1024)ms
        eeprom_update_word(adrs + 4, (uint16_t)value_data[0] << 2);
        break;
    }
  }
}

// TODO should reduce the number of writing times of slider control
void via_custom_td_save(uint8_t td_index) {
#  ifdef CONSOLE_ENABLE
  uprintf("via_custom_td_save:td_index:%d\n", td_index);
#  endif
}

#endif  // VIA_VERSION == 3

void pgm_memcpy(void *dest, const void *src, size_t len) {
  for (size_t i = 0; i < len; i++) {
    *(uint8_t *)dest++ = pgm_read_byte((uint8_t *)src++);
  }
}
