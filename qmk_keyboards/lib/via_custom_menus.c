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

__attribute__((weak)) bool via_custom_value_command_user(via_custom_command_t *command) { return true; }

static defer_eeprom_update_item_t defer_eeprom_update_items[DEFER_EEPROM_UPDATE_ITEM_SIZE];

static void via_custom_magic_get_value(via_custom_command_t *command);
static void via_custom_magic_set_value(via_custom_command_t *command);
#ifdef RADIAL_CONTROLLER_ENABLE
static void via_custom_rc_get_value(via_custom_command_t *command);
static void via_custom_rc_set_value(via_custom_command_t *command);
#endif
static void via_custom_td_get_value(via_custom_command_t *command);
static void via_custom_td_set_value(via_custom_command_t *command);

static void via_custom_non_mac_fn_get_value(via_custom_command_t *command);
static void via_custom_non_mac_fn_set_value(via_custom_command_t *command);

static void defer_eeprom_update(uint16_t id, defer_eeprom_update_value_type_t value_type, void *eeprom_adrs,
                                uint32_t value, void *block_adrs, size_t block_size);
static uint32_t defer_eeprom_update_callback(uint32_t trigger_time, defer_eeprom_update_item_t *item);

static inline uint8_t via_readUInt8(via_custom_command_t *command) { return command->data[0]; }
static inline void via_writeUInt8(via_custom_command_t *command, uint8_t value) { command->data[0] = value; }
static inline uint16_t via_readUInt16BE(via_custom_command_t *command) {
  return ((uint16_t)(command->data[0]) << 8) + command->data[1];
}
static inline void via_writeUInt16BE(via_custom_command_t *command, uint16_t value) {
  command->data[0] = value >> 8;
  command->data[1] = value & 0xff;
}

// VIA custom hook function
void via_custom_value_command_kb(uint8_t *data, uint8_t length) {
  via_custom_command_t command = VIA_CUSTOM_COMMAND(data);
#ifdef CONSOLE_ENABLE
  uprintf("via_custom_value_command_kb:command_id:%02X channel_id:%02X value_id:%02X data:%02X, %02X\n",
          command.command_id, command.channel_id, command.value_id, command.data[0], command.data[1]);
#endif  // ONSOLE_ENABLE
  // data = [ command_id, channel_id, value_id, value_data ]
  switch (command.channel_id) {
    case id_custom_magic_channel:
      switch (command.command_id) {
        case id_custom_set_value:
          via_custom_magic_set_value(&command);
          return;
        case id_custom_get_value:
          via_custom_magic_get_value(&command);
          return;
        case id_custom_save:
          return;
      }
#ifdef RADIAL_CONTROLLER_ENABLE
    case id_custom_rc_channel:
      switch (command.command_id) {
        case id_custom_set_value:
          via_custom_rc_set_value(&command);
          return;
        case id_custom_get_value:
          via_custom_rc_get_value(&command);
          return;
        case id_custom_save:
          return;
      }
#endif
    case id_custom_td_channel_start ... id_custom_td_channel_end:
      switch (data[0]) {
        case id_custom_set_value:
          via_custom_td_set_value(&command);
          return;
        case id_custom_get_value:
          via_custom_td_get_value(&command);
          return;
        case id_custom_save:
          return;
      }
    case id_custom_non_mac_fn_channel:
      switch (data[0]) {
        case id_custom_set_value:
          via_custom_non_mac_fn_set_value(&command);
          return;
        case id_custom_get_value:
          via_custom_non_mac_fn_get_value(&command);
          return;
        case id_custom_save:
          return;
      }
    default:
      if (!via_custom_value_command_user(&command)) {
        return;
      }
  }
  // Return the unhandled state
  data[0] = id_unhandled;
}

// QMK Magic menu

static void via_custom_magic_get_value(via_custom_command_t *command) {
  keymap_config.raw = eeconfig_read_keymap();
  switch (command->value_id) {
    case id_custom_magic_swap_control_capslock:
      via_write_toggle_value(command, keymap_config.swap_control_capslock);
      break;
    case id_custom_magic_swap_escape_capslock:
      via_write_toggle_value(command, keymap_config.swap_escape_capslock);
      break;
    case id_custom_magic_capslock_to_control:
      via_write_toggle_value(command, keymap_config.capslock_to_control);
      break;
    case id_custom_magic_swap_lctl_lgui:
      via_write_toggle_value(command, keymap_config.swap_lctl_lgui);
      break;
    case id_custom_magic_swap_rctl_rgui:
      via_write_toggle_value(command, keymap_config.swap_rctl_rgui);
      break;
    case id_custom_magic_swap_lalt_lgui:
      via_write_toggle_value(command, keymap_config.swap_lalt_lgui);
      break;
    case id_custom_magic_swap_ralt_rgui:
      via_write_toggle_value(command, keymap_config.swap_ralt_rgui);
      break;
    case id_custom_magic_no_gui:
      via_write_toggle_value(command, keymap_config.no_gui);
      break;
    case id_custom_magic_swap_grave_esc:
      via_write_toggle_value(command, keymap_config.swap_grave_esc);
      break;
    case id_custom_magic_host_nkro:
      via_write_toggle_value(command, keymap_config.nkro);
      break;
    case id_custom_magic_swap_bs_bsls:
      via_write_toggle_value(command, keymap_config.swap_backslash_backspace);
      break;
  }
}

static void via_custom_magic_set_value(via_custom_command_t *command) {
  keymap_config.raw = eeconfig_read_keymap();
  bool value = via_read_toggle_value(command);
  switch (command->value_id) {
    case id_custom_magic_swap_control_capslock:
      keymap_config.swap_control_capslock = value;
      break;
    case id_custom_magic_swap_escape_capslock:
      keymap_config.swap_escape_capslock = value;
      break;
    case id_custom_magic_capslock_to_control:
      keymap_config.capslock_to_control = value;
      break;
    case id_custom_magic_swap_lctl_lgui:
      keymap_config.swap_lctl_lgui = value;
      break;
    case id_custom_magic_swap_rctl_rgui:
      keymap_config.swap_rctl_rgui = value;
      break;
    case id_custom_magic_swap_lalt_lgui:
      keymap_config.swap_lalt_lgui = value;
      break;
    case id_custom_magic_swap_ralt_rgui:
      keymap_config.swap_ralt_rgui = value;
      break;
    case id_custom_magic_no_gui:
      keymap_config.no_gui = value;
      break;
    case id_custom_magic_swap_grave_esc:
      keymap_config.swap_grave_esc = value;
      break;
    case id_custom_magic_host_nkro:
      keymap_config.nkro = value;
      break;
    case id_custom_magic_swap_bs_bsls:
      keymap_config.swap_backslash_backspace = value;
      break;
  }
  eeconfig_update_keymap(keymap_config.raw);
  clear_keyboard();  // clear to prevent stuck keys
}

// Radial Controller menu

#ifdef RADIAL_CONTROLLER_ENABLE
static void via_custom_rc_get_value(via_custom_command_t *command) {
  switch (command->value_id) {
    case id_custom_rc_encoder_clicks:
      via_write_dropdown_value(command, rc_config.encoder_clicks);
      break;
    case id_custom_rc_key_angular_speed:
      via_write_range_byte_value(command, rc_config.key_angular_speed);
      break;
    case id_custom_rc_fine_tune_ratio:
      via_write_dropdown_value(command, rc_config.fine_tune_ratio);
      break;
    case id_custom_rc_fine_tune_mod_ctrl ... id_custom_rc_fine_tune_mod_apple_fn:
      via_write_toggle_value(
        command, (rc_config.fine_tune_mods & (1 << (command->value_id - id_custom_rc_fine_tune_mod_ctrl))) ? 1 : 0);
      break;
  }
}

static void via_custom_rc_set_value(via_custom_command_t *command) {
  switch (command->value_id) {
    case id_custom_rc_encoder_clicks:
      rc_config.encoder_clicks = via_read_dropdown_value(command);
      break;
    case id_custom_rc_key_angular_speed:
      rc_config.key_angular_speed = via_read_range_byte_value(command);
      break;
    case id_custom_rc_fine_tune_ratio:
      rc_config.fine_tune_ratio = via_read_dropdown_value(command);
      break;
    case id_custom_rc_fine_tune_mod_ctrl ... id_custom_rc_fine_tune_mod_apple_fn: {
      uint8_t mod_mask = 1 << (command->value_id - id_custom_rc_fine_tune_mod_ctrl);
      if (via_read_toggle_value(command)) {
        rc_config.fine_tune_mods |= mod_mask;
      } else {
        rc_config.fine_tune_mods &= ~mod_mask;
      }
      break;
    }
  }
  defer_eeprom_update_dword(id_custom_rc_channel, 0, (void *)RADIAL_CONTROLLER_EEPROM_ADDR, rc_config.raw);
}

#endif  // RADIAL_CONTROLLER_ENABLE

// Tap Dance menu

static void via_custom_td_get_value(via_custom_command_t *command) {
  uint8_t td_index = command->channel_id - id_custom_td_channel_start;
  switch (command->value_id) {
    case id_custom_td_single_tap ... id_custom_td_tap_hold:
      via_write_keycode_value(command, dynamic_tap_dance_keycode(td_index, command->value_id));
      break;
    case id_custom_td_tapping_term:
      via_write_range_word_value(command, dynamic_tap_dance_tapping_term(td_index));
      break;
  }
}

static void via_custom_td_set_value(via_custom_command_t *command) {
  uint8_t td_index = command->channel_id - id_custom_td_channel_start;
  uint16_t *adrs = (uint16_t *)(DYNAMIC_TAP_DANCE_EEPROM_ADDR + 10 * td_index + (command->value_id - 1) * 2);
  if (td_index < TAP_DANCE_ENTRIES) {
    switch (command->value_id) {
      case id_custom_td_single_tap ... id_custom_td_tap_hold:
        eeprom_update_word(adrs, via_read_keycode_value(command));
        break;
      case id_custom_td_tapping_term:
        defer_eeprom_update_word(command->channel_id, command->value_id, adrs, via_read_range_word_value(command));
        break;
    }
  }
}

// non-mac fn functions

static void via_custom_non_mac_fn_get_value(via_custom_command_t *command) {
  switch (command->value_id) {
    case id_custom_non_mac_auto_detect:
      via_write_toggle_value(command, custom_config_auto_detect_is_enable());
      break;
    case id_custom_non_mac_fn_fkey:
      via_write_toggle_value(command, custom_config_non_mac_fn_fkey_is_enable());
      break;
    case id_custom_non_mac_fn_alpha:
      via_write_toggle_value(command, custom_config_non_mac_fn_alpha_is_enable());
      break;
    case id_custom_non_mac_fn_cursor:
      via_write_toggle_value(command, custom_config_non_mac_fn_cursor_is_enable());
      break;
    case id_custom_non_mac_fn_f1 ... id_custom_non_mac_fn_right: {
      via_write_keycode_value(command,
                              dynamic_non_mac_fn_keycode(FN_F1 + (command->value_id - id_custom_non_mac_fn_f1)));
      break;
    }
  }
}

static void via_custom_non_mac_fn_set_value(via_custom_command_t *command) {
  switch (command->value_id) {
    case id_custom_non_mac_auto_detect:
      custom_config_auto_detect_set_enable(via_read_toggle_value(command));
      break;
    case id_custom_non_mac_fn_fkey:
      custom_config_non_mac_fn_set_fkey(via_read_toggle_value(command));
      break;
    case id_custom_non_mac_fn_alpha:
      custom_config_non_mac_fn_set_alpha(via_read_toggle_value(command));
      break;
    case id_custom_non_mac_fn_cursor:
      custom_config_non_mac_fn_set_cursor(via_read_toggle_value(command));
      break;
    case id_custom_non_mac_fn_f1 ... id_custom_non_mac_fn_right:
      eeprom_update_word(
        (uint16_t *)(DYNAMIC_NON_MAC_FN_EEPROM_ADDR + (command->value_id - id_custom_non_mac_fn_f1) * 2),
        via_read_keycode_value(command));
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
        item->src.data.adrs = block_adrs;
        item->src.data.size = block_size;
      } else {
        item->src.value = value;
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
  // release item
  item->id = 0;
#ifdef CONSOLE_ENABLE
  if (item->value_type == BLOCK) {
    uprintf("defer_eeprom_update_callback:id:%04X block_adrs:0x%04X-%04X block_size:%d\n", item->id,
            (uint16_t)((uint32_t)item->src.data.adrs >> 16), (uint16_t)((uint32_t)item->src.data.adrs & 0xffff),
            item->src.data.size);
  } else {
    uprintf("defer_eeprom_update_callback:id:%04X value:%ld\n", item->id, item->src.value);
  }
#endif  // CONSOLE_ENABLE
  return 0;
}

// export functions

uint8_t via_read_dropdown_value(via_custom_command_t *command) { return via_readUInt8(command); }
void via_write_dropdown_value(via_custom_command_t *command, uint8_t value) { via_writeUInt8(command, value); }
bool via_read_toggle_value(via_custom_command_t *command) { return via_readUInt8(command); }
void via_write_toggle_value(via_custom_command_t *command, bool value) { via_writeUInt8(command, value); }
uint8_t via_read_range_byte_value(via_custom_command_t *command) { return via_readUInt8(command); }
void via_write_range_byte_value(via_custom_command_t *command, uint8_t value) { via_writeUInt8(command, value); }
uint16_t via_read_range_word_value(via_custom_command_t *command) { return via_readUInt16BE(command); }
void via_write_range_word_value(via_custom_command_t *command, uint16_t value) { via_writeUInt16BE(command, value); }
uint16_t via_read_keycode_value(via_custom_command_t *command) { return via_readUInt16BE(command); }
void via_write_keycode_value(via_custom_command_t *command, uint16_t keycode) { via_writeUInt16BE(command, keycode); }

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
