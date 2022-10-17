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
// additional conbination of modfier for RC fine-tune.
// ctrl, bit1: shift, bit2: alt, bit3: gui
#  ifndef RADIAL_CONTROLLER_FINE_TUNE_MODS_DEFAULT
#    define RADIAL_CONTROLLER_FINE_TUNE_MODS_DEFAULT 0x00
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
    uint8_t fine_tune_mods : 4;   // bit0: ctrl, bit1: shift, bit2: alt, bit3: gui
    uint8_t fine_tune_ratio : 2;  // power-of-2 divider 0: none, 1: 1/2, 2:1/4, 3:1/8
  };
} rc_config_t;
static rc_config_t rc_config;
#endif

static void _custom_config_raw_hid_set_enable(bool enable);
static void _custom_config_mac_set_enable(bool enable);
static void _custom_config_usj_set_enable(bool enable);

#define DEFER_EEPROM_UPDATE_ITEM_SIZE 4
#define DEFER_EEPROM_UPDATE_DELAY_MILLIS 400
typedef enum { BYTE, WORD, DWORD } defer_eeprom_update_value_type_t;
typedef struct {
  uint16_t id;  // 0: empty, channel_id << 8 + value_id
  void *adrs;   // eeprom address
  defer_eeprom_update_value_type_t value_type;
  uint32_t value;        // data value
  deferred_token token;  // defer_exec token
} defer_eeprom_update_item_t;
static defer_eeprom_update_item_t defer_eeprom_update_items[DEFER_EEPROM_UPDATE_ITEM_SIZE];
static void defer_eeprom_update(uint16_t id, defer_eeprom_update_value_type_t value_type, void *adrs, uint32_t value);
static uint32_t defer_eeprom_update_callback(uint32_t trigger_time, defer_eeprom_update_item_t *item);

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
  rc_config.fine_tune_mods = RADIAL_CONTROLLER_FINE_TUNE_MODS_DEFAULT;
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
    case id_custom_magic_channel:
      switch (data[0]) {
        case id_custom_set_value:
          via_custom_magic_set_value(data[2], &(data[3]));
          return;
        case id_custom_get_value:
          via_custom_magic_get_value(data[2], &(data[3]));
          return;
        case id_custom_save:
          return;
      }
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

void via_custom_magic_get_value(uint8_t value_id, uint8_t *value_data) {
  keymap_config.raw = eeconfig_read_keymap();
  switch (value_id) {
    case id_custom_magic_swap_control_capslock:
      value_data[0] = keymap_config.swap_control_capslock;
      break;
    case id_custom_magic_swap_escape_capslock:
      value_data[0] = keymap_config.swap_escape_capslock;
      break;
    case id_custom_magic_capslock_to_control:
      value_data[0] = keymap_config.capslock_to_control;
      break;
    case id_custom_magic_swap_lctl_lgui:
      value_data[0] = keymap_config.swap_lctl_lgui;
      break;
    case id_custom_magic_swap_rctl_rgui:
      value_data[0] = keymap_config.swap_rctl_rgui;
      break;
    case id_custom_magic_swap_lalt_lgui:
      value_data[0] = keymap_config.swap_lalt_lgui;
      break;
    case id_custom_magic_swap_ralt_rgui:
      value_data[0] = keymap_config.swap_ralt_rgui;
      break;
    case id_custom_magic_no_gui:
      value_data[0] = keymap_config.no_gui;
      break;
    case id_custom_magic_swap_grave_esc:
      value_data[0] = keymap_config.swap_grave_esc;
      break;
    case id_custom_magic_host_nkro:
      value_data[0] = keymap_config.nkro;
      break;
  }
}

void via_custom_magic_set_value(uint8_t value_id, uint8_t *value_data) {
  keymap_config.raw = eeconfig_read_keymap();
  switch (value_id) {
    case id_custom_magic_swap_control_capslock:
      keymap_config.swap_control_capslock = value_data[0];
      break;
    case id_custom_magic_swap_escape_capslock:
      keymap_config.swap_escape_capslock = value_data[0];
      break;
    case id_custom_magic_capslock_to_control:
      keymap_config.capslock_to_control = value_data[0];
      break;
    case id_custom_magic_swap_lctl_lgui:
      keymap_config.swap_lctl_lgui = value_data[0];
      break;
    case id_custom_magic_swap_rctl_rgui:
      keymap_config.swap_rctl_rgui = value_data[0];
      break;
    case id_custom_magic_swap_lalt_lgui:
      keymap_config.swap_lalt_lgui = value_data[0];
      break;
    case id_custom_magic_swap_ralt_rgui:
      keymap_config.swap_ralt_rgui = value_data[0];
      break;
    case id_custom_magic_no_gui:
      keymap_config.no_gui = value_data[0];
      break;
    case id_custom_magic_swap_grave_esc:
      keymap_config.swap_grave_esc = value_data[0];
      break;
    case id_custom_magic_host_nkro:
      keymap_config.nkro = value_data[0];
      break;
  }
  eeconfig_update_keymap(keymap_config.raw);
  clear_keyboard();  // clear to prevent stuck keys
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
  if (rc_config.fine_tune_ratio && rc_config.fine_tune_mods) {
    uint8_t cur_mods = (mods & MOD_MASK_CTRL ? 1 : 0) + (mods & MOD_MASK_SHIFT ? 2 : 0) +
                       (mods & MOD_MASK_ALT ? 4 : 0) + (mods & MOD_MASK_GUI ? 8 : 0);
    return (cur_mods & rc_config.fine_tune_mods) == rc_config.fine_tune_mods;
  }
  return false;
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
    case id_custom_rc_fine_tune_mod_ctrl ... id_custom_rc_fine_tune_mod_gui:
      value_data[0] = (rc_config.fine_tune_mods & (1 << (value_id - id_custom_rc_fine_tune_mod_ctrl))) ? 1 : 0;
      break;
  }
#    ifdef CONSOLE_ENABLE
  uprintf("via_custom_rc_get_value:value_id:%d value:%02X %02X\n", value_id, value_data[0], value_data[1]);
#    endif
}

void via_custom_rc_set_value(uint8_t value_id, uint8_t *value_data) {
  uint8_t mod_mask;
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
    case id_custom_rc_fine_tune_mod_ctrl ... id_custom_rc_fine_tune_mod_gui:
      mod_mask = 1 << (value_id - id_custom_rc_fine_tune_mod_ctrl);
      if (value_data[0]) {
        rc_config.fine_tune_mods |= mod_mask;
      } else {
        rc_config.fine_tune_mods &= ~mod_mask;
      }
      break;
  }
  defer_eeprom_update(id_custom_rc_channel << 8, DWORD, (void *)RADIAL_CONTROLLER_EEPROM_ADDR, rc_config.raw);
}

void via_custom_rc_save() {
#    ifdef CONSOLE_ENABLE
  uprintf("via_custom_rc_save_value\n");
#    endif
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

void via_custom_td_get_value(uint8_t td_index, uint8_t value_id, uint8_t *value_data) {
  uint16_t value;
  switch (value_id) {
    case id_custom_td_single_tap ... id_custom_td_tap_hold:
      value = dynamic_tap_dance_keycode(td_index, value_id);
      break;
    case id_custom_td_tapping_term:
      value = dynamic_tap_dance_tapping_term(td_index);
      break;
  }
  // LE
  value_data[0] = value & 0xff;
  value_data[1] = value >> 8;
  // BE
  // value_data[0] = value >> 8;
  // value_data[1] = value & 0xff;
#  ifdef CONSOLE_ENABLE
  uprintf("via_custom_td_get_value:td_index:%d value_id:%d value:%02X %02X\n", td_index, value_id, value_data[0],
          value_data[1]);
#  endif
}

void via_custom_td_set_value(uint8_t td_index, uint8_t value_id, uint8_t *value_data) {
  uint16_t *adrs = (uint16_t *)(DYNAMIC_TAP_DANCE_EEPROM_ADDR + 10 * td_index + (value_id - 1) * 2);
#  ifdef CONSOLE_ENABLE
  uprintf("via_custom_td_set_value:td_index:%d value_id:%d value:%02X %02X\n", td_index, value_id, value_data[0],
          value_data[1]);
#  endif
  if (td_index < TAP_DANCE_ENTRIES) {
    switch (value_id) {
      case id_custom_td_single_tap ... id_custom_td_tap_hold:
        // LE
        eeprom_update_word(adrs, ((uint16_t)value_data[1] << 8) + value_data[0]);
        break;
      case id_custom_td_tapping_term:
        // LE
        defer_eeprom_update(((id_custom_td_channel_start + td_index) << 8) + value_id, WORD, adrs,
                            ((uint16_t)value_data[1] << 8) + value_data[0]);
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

static void defer_eeprom_update(uint16_t id, defer_eeprom_update_value_type_t value_type, void *adrs, uint32_t value) {
  defer_eeprom_update_item_t *new_item = NULL;
  for (size_t i = 0; i < DEFER_EEPROM_UPDATE_ITEM_SIZE; i++) {
    defer_eeprom_update_item_t *item = &defer_eeprom_update_items[i];
    if (id == item->id) {
      item->value = value;
      extend_deferred_exec(item->token, DEFER_EEPROM_UPDATE_DELAY_MILLIS);
      return;
    } else if (new_item == NULL && item->id == 0) {
      new_item = item;
    }
  }
  if (new_item != NULL) {
    new_item->id = id;
    new_item->value_type = value_type;
    new_item->adrs = adrs;
    new_item->value = value;
    new_item->token = defer_exec(DEFER_EEPROM_UPDATE_DELAY_MILLIS,
                                 (uint32_t(*)(uint32_t, void *))defer_eeprom_update_callback, new_item);
  }
}

static uint32_t defer_eeprom_update_callback(uint32_t trigger_time, defer_eeprom_update_item_t *item) {
  switch (item->value_type) {
    case BYTE:
      eeprom_update_byte(item->adrs, item->value);
      break;
    case WORD:
      eeprom_update_word(item->adrs, item->value);
      break;
    case DWORD:
      eeprom_update_dword(item->adrs, item->value);
      break;
  }
#ifdef CONSOLE_ENABLE
  uprintf("defer_eeprom_update_callback:id:%04X value:%d\n", item->id, item->value);
#endif
  // release item
  item->id = 0;
  return 0;
}
