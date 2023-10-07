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
#include "via_custom_menus.h"

#ifdef CONSOLE_ENABLE
#  include <print.h>
#endif
#include <eeprom.h>

#include "apple_fn.h"
#include "custom_config.h"
#include "tap_dance.h"

#define DEFER_EEPROM_UPDATE_ITEM_SIZE 4
#define DEFER_EEPROM_UPDATE_DELAY_MILLIS 400

typedef enum { BYTE, WORD, DWORD, BLOCK } defer_eeprom_update_value_type_t;
typedef struct {
  uint16_t id;        // 0: empty, channel_id << 8 + value_id
  void *eeprom_adrs;  // eeprom address
  defer_eeprom_update_value_type_t value_type;
  union {
    uint32_t value;  // data value
    struct {
      void *adrs;
      size_t size;
    } data;
  } src;
  deferred_token token;  // defer_exec token
} defer_eeprom_update_item_t;

__attribute__((weak)) bool via_custom_value_command_user(uint8_t *data, uint8_t length) { return true; }

static defer_eeprom_update_item_t defer_eeprom_update_items[DEFER_EEPROM_UPDATE_ITEM_SIZE];

static void defer_eeprom_update(uint16_t id, defer_eeprom_update_value_type_t value_type, void *eeprom_adrs,
                                uint32_t value, void *block_adrs, size_t block_size);
static uint32_t defer_eeprom_update_callback(uint32_t trigger_time, defer_eeprom_update_item_t *item);

// VIA custom hook function
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
#ifdef RADIAL_CONTROLLER_ENABLE
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
#endif
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
    case id_custom_non_mac_fn_channel:
      switch (data[0]) {
        case id_custom_set_value:
          via_custom_non_mac_fn_set_value(data[2], &(data[3]));
          return;
        case id_custom_get_value:
          via_custom_non_mac_fn_get_value(data[2], &(data[3]));
          return;
        case id_custom_save:
          return;
      }
    default:
      if (!via_custom_value_command_user(data, length)) {
        return;
      }
  }
  // Return the unhandled state
  data[0] = id_unhandled;
}

// QMK Magic menu

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
    case id_custom_magic_swap_bs_bsls:
      value_data[0] = keymap_config.swap_backslash_backspace;
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
    case id_custom_magic_swap_bs_bsls:
      keymap_config.swap_backslash_backspace = value_data[0];
      break;
  }
  eeconfig_update_keymap(keymap_config.raw);
  clear_keyboard();  // clear to prevent stuck keys
}

// Radial Controller menu

#ifdef RADIAL_CONTROLLER_ENABLE
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
    case id_custom_rc_fine_tune_mod_ctrl ... id_custom_rc_fine_tune_mod_apple_fn:
      value_data[0] = (rc_config.fine_tune_mods & (1 << (value_id - id_custom_rc_fine_tune_mod_ctrl))) ? 1 : 0;
      break;
  }
#  ifdef CONSOLE_ENABLE
  uprintf("via_custom_rc_get_value:value_id:%d value:%02X %02X\n", value_id, value_data[0], value_data[1]);
#  endif
}

void via_custom_rc_set_value(uint8_t value_id, uint8_t *value_data) {
  uint8_t mod_mask;
#  ifdef CONSOLE_ENABLE
  uprintf("via_custom_rc_set_value:value_id:%d value:%02X %02X\n", value_id, value_data[0], value_data[1]);
#  endif
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
    case id_custom_rc_fine_tune_mod_ctrl ... id_custom_rc_fine_tune_mod_apple_fn:
      mod_mask = 1 << (value_id - id_custom_rc_fine_tune_mod_ctrl);
      if (value_data[0]) {
        rc_config.fine_tune_mods |= mod_mask;
      } else {
        rc_config.fine_tune_mods &= ~mod_mask;
      }
      break;
  }
  defer_eeprom_update_dword(id_custom_rc_channel, 0, (void *)RADIAL_CONTROLLER_EEPROM_ADDR, rc_config.raw);
}

void via_custom_rc_save() {
#  ifdef CONSOLE_ENABLE
  uprintf("via_custom_rc_save_value\n");
#  endif
}
#endif  // RADIAL_CONTROLLER_ENABLE

// Tap Dance menu

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
  // 16 bit BigEndian
  value_data[0] = value >> 8;
  value_data[1] = value & 0xff;
#ifdef CONSOLE_ENABLE
  uprintf("via_custom_td_get_value:td_index:%d value_id:%d value:%02X %02X\n", td_index, value_id, value_data[0],
          value_data[1]);
#endif
}

void via_custom_td_set_value(uint8_t td_index, uint8_t value_id, uint8_t *value_data) {
  uint16_t *adrs = (uint16_t *)(DYNAMIC_TAP_DANCE_EEPROM_ADDR + 10 * td_index + (value_id - 1) * 2);
#ifdef CONSOLE_ENABLE
  uprintf("via_custom_td_set_value:td_index:%d value_id:%d value:%02X %02X\n", td_index, value_id, value_data[0],
          value_data[1]);
#endif
  if (td_index < TAP_DANCE_ENTRIES) {
    switch (value_id) {
      case id_custom_td_single_tap ... id_custom_td_tap_hold:
        // 18bit BigEndian
        eeprom_update_word(adrs, ((uint16_t)value_data[0] << 8) + value_data[1]);
        break;
      case id_custom_td_tapping_term:
        // 16bit BigEndian
        defer_eeprom_update_word(id_custom_td_channel_start + td_index, value_id, adrs,
                                 ((uint16_t)value_data[0] << 8) + value_data[1]);
        break;
    }
  }
}

void via_custom_td_save(uint8_t td_index) {
#ifdef CONSOLE_ENABLE
  uprintf("via_custom_td_save:td_index:%d\n", td_index);
#endif  // ONSOLE_ENABLE
}

// non-mac fn functions

void via_custom_non_mac_fn_get_value(uint8_t value_id, uint8_t *value_data) {
  switch (value_id) {
    case id_custom_non_mac_auto_detect:
      value_data[0] = custom_config_auto_detect_is_enable();
      break;
    case id_custom_non_mac_fn_fkey:
      value_data[0] = custom_config_non_mac_fn_fkey_is_enable();
      break;
    case id_custom_non_mac_fn_alpha:
      value_data[0] = custom_config_non_mac_fn_alpha_is_enable();
      break;
    case id_custom_non_mac_fn_cursor:
      value_data[0] = custom_config_non_mac_fn_cursor_is_enable();
      break;
    case id_custom_non_mac_fn_f1 ... id_custom_non_mac_fn_right: {
      uint16_t keycode = dynamic_non_mac_fn_keycode(FN_F1 + (value_id - id_custom_non_mac_fn_f1));
      // 16bit BigEndian
      value_data[0] = keycode >> 8;
      value_data[1] = keycode & 0xff;
      break;
    }
  }
#ifdef CONSOLE_ENABLE
  uprintf("via_custom_non_mac_fn_get_value:value_id:%d value:%02X %02X\n", value_id, value_data[0], value_data[1]);
#endif
}

void via_custom_non_mac_fn_set_value(uint8_t value_id, uint8_t *value_data) {
#ifdef CONSOLE_ENABLE
  uprintf("via_custom_non_mac_fn_set_value:value_id:%d value:%02X %02X\n", value_id, value_data[0], value_data[1]);
#endif
  switch (value_id) {
    case id_custom_non_mac_auto_detect:
      custom_config_auto_detect_set_enable(value_data[0]);
      break;
    case id_custom_non_mac_fn_fkey:
      custom_config_non_mac_fn_set_fkey(value_data[0]);
      break;
    case id_custom_non_mac_fn_alpha:
      custom_config_non_mac_fn_set_alpha(value_data[0]);
      break;
    case id_custom_non_mac_fn_cursor:
      custom_config_non_mac_fn_set_cursor(value_data[0]);
      break;
    case id_custom_non_mac_fn_f1 ... id_custom_non_mac_fn_right:
      // 16bit BigEndian
      eeprom_update_word((uint16_t *)(DYNAMIC_NON_MAC_FN_EEPROM_ADDR + (value_id - id_custom_non_mac_fn_f1) * 2),
                         ((uint16_t)value_data[0] << 8) + value_data[1]);
      break;
  }
}

// utility routine

static void defer_eeprom_update(uint16_t id, defer_eeprom_update_value_type_t value_type, void *eeprom_adrs,
                                uint32_t value, void *block_adrs, size_t block_size) {
  defer_eeprom_update_item_t *new_item = NULL;
  for (uint8_t i = 0; i < DEFER_EEPROM_UPDATE_ITEM_SIZE; i++) {
    defer_eeprom_update_item_t *item = &defer_eeprom_update_items[i];
    if (id == item->id) {
      if (value_type == BLOCK) {
        new_item->src.data.adrs = block_adrs;
        new_item->src.data.size = block_size;
      } else {
        new_item->src.value = value;
      }
      extend_deferred_exec(item->token, DEFER_EEPROM_UPDATE_DELAY_MILLIS);
      return;
    } else if (new_item == NULL && item->id == 0) {
      new_item = item;
    }
  }
  if (new_item != NULL) {
    new_item->value_type = value_type;
    new_item->eeprom_adrs = eeprom_adrs;
    if (value_type == BLOCK) {
      new_item->src.data.adrs = block_adrs;
      new_item->src.data.size = block_size;
    } else {
      new_item->src.value = value;
    }
    new_item->token = defer_exec(DEFER_EEPROM_UPDATE_DELAY_MILLIS,
                                 (uint32_t(*)(uint32_t, void *))defer_eeprom_update_callback, new_item);
    if (new_item->token) {
      new_item->id = id;
    }
  } else {
    // DEFER_EEPROM_UPDATE_ITEM_SIZE is normally enough for GUI editing.
    // however, it maybe overflow when load settings.
    switch (value_type) {
      case BYTE:
        eeprom_update_byte(eeprom_adrs, value);
        break;
      case WORD:
        eeprom_update_word(eeprom_adrs, value);
        break;
      case DWORD:
        eeprom_update_dword(eeprom_adrs, value);
        break;
      case BLOCK:
        eeprom_update_block(block_adrs, eeprom_adrs, block_size);
        break;
    }
  }
}

static uint32_t defer_eeprom_update_callback(uint32_t trigger_time, defer_eeprom_update_item_t *item) {
  switch (item->value_type) {
    case BYTE:
      eeprom_update_byte(item->eeprom_adrs, item->src.value);
      break;
    case WORD:
      eeprom_update_word(item->eeprom_adrs, item->src.value);
      break;
    case DWORD:
      eeprom_update_dword(item->eeprom_adrs, item->src.value);
      break;
    case BLOCK:
      eeprom_update_block(item->src.data.adrs, item->eeprom_adrs, item->src.data.size);
      break;
  }
#ifdef CONSOLE_ENABLE
  uprintf("defer_eeprom_update_callback:id:%04X value:%d\n", item->id, item->value);
#endif  // ONSOLE_ENABLE
  // release item
  item->id = 0;
  return 0;
}

// export functions

void defer_eeprom_update_byte(uint8_t channel_id, uint8_t value_id, void *eeprom_adrs, uint8_t value) {
  defer_eeprom_update((channel_id << 8) + value_id, BYTE, eeprom_adrs, value, 0, 0);
}

void defer_eeprom_update_word(uint8_t channel_id, uint8_t value_id, void *eeprom_adrs, uint16_t value) {
  defer_eeprom_update((channel_id << 8) + value_id, WORD, eeprom_adrs, value, 0, 0);
}

void defer_eeprom_update_dword(uint8_t channel_id, uint8_t value_id, void *eeprom_adrs, uint32_t value) {
  defer_eeprom_update((channel_id << 8) + value_id, DWORD, eeprom_adrs, value, 0, 0);
}

void defer_eeprom_update_block(uint8_t channel_id, uint8_t value_id, void *block_adrs, void *eeprom_adrs,
                               uint32_t block_size) {
  defer_eeprom_update((channel_id << 8) + value_id, BLOCK, eeprom_adrs, 0, block_adrs, block_size);
}
