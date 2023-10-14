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
#pragma once

#include <quantum.h>
#include <via.h>

#define VIA_CUSTTOM_CHANNEL_ID_START 5
enum via_custom_channel_id {
  id_custom_magic_channel = VIA_CUSTTOM_CHANNEL_ID_START,
  id_custom_rc_channel,          // Radial Controller
  id_custom_non_mac_fn_channel,  // none mac fn functions
  id_custom_td_channel_start,    // Tap Dance start
  id_custom_td_channel_end = id_custom_td_channel_start + TAP_DANCE_ENTRIES - 1,  // Tap Dance end
  id_custom_channel_user_range
};

enum via_custom_magic_value_id {
  id_custom_magic_swap_control_capslock = 1,
  id_custom_magic_swap_escape_capslock,
  id_custom_magic_capslock_to_control,
  id_custom_magic_swap_lctl_lgui,
  id_custom_magic_swap_rctl_rgui,
  id_custom_magic_swap_lalt_lgui,
  id_custom_magic_swap_ralt_rgui,
  id_custom_magic_no_gui,
  id_custom_magic_swap_grave_esc,
  id_custom_magic_host_nkro,
  id_custom_magic_swap_bs_bsls
};

enum via_custom_rc_value_id {
  id_custom_rc_encoder_clicks = 1,
  id_custom_rc_key_angular_speed,
  id_custom_rc_fine_tune_ratio,
  id_custom_rc_fine_tune_mod_ctrl,
  id_custom_rc_fine_tune_mod_shift,
  id_custom_rc_fine_tune_mod_alt,
  id_custom_rc_fine_tune_mod_gui,
  id_custom_rc_fine_tune_mod_apple_fn
};

enum via_custom_td_value_id {
  id_custom_td_single_tap = 1,
  id_custom_td_single_hold,
  id_custom_td_multi_tap,
  id_custom_td_tap_hold,
  id_custom_td_tapping_term
};

enum via_custom_non_mac_fn_value_id {
  id_custom_non_mac_auto_detect = 1,
  id_custom_non_mac_fn_fkey,
  id_custom_non_mac_fn_alpha,
  id_custom_non_mac_fn_cursor,
  id_custom_non_mac_fn_f1,
  id_custom_non_mac_fn_f2,
  id_custom_non_mac_fn_f3,
  id_custom_non_mac_fn_f4,
  id_custom_non_mac_fn_f5,
  id_custom_non_mac_fn_f6,
  id_custom_non_mac_fn_f7,
  id_custom_non_mac_fn_f8,
  id_custom_non_mac_fn_f9,
  id_custom_non_mac_fn_f10,
  id_custom_non_mac_fn_f11,
  id_custom_non_mac_fn_f12,
  id_custom_non_mac_fn_spc,    // Hey Siri
  id_custom_non_mac_fn_q,      // Qick Notes
  id_custom_non_mac_fn_e,      // Emoji & Symbols
  id_custom_non_mac_fn_a,      // Focus Dock
  id_custom_non_mac_fn_d,      // Start Dictation
  id_custom_non_mac_fn_f,      // Toggle full screen mode
  id_custom_non_mac_fn_h,      // Show Desktop
  id_custom_non_mac_fn_c,      // Show Control Cnecter
  id_custom_non_mac_fn_n,      // Show Notification
  id_custom_non_mac_fn_m,      // Focus Menubar
  id_custom_non_mac_fn_bspc,   // Delete
  id_custom_non_mac_fn_up,     // up arrow
  id_custom_non_mac_fn_down,   // down arrow
  id_custom_non_mac_fn_left,   // left arrow
  id_custom_non_mac_fn_right,  // right arrow
};

typedef struct {
  uint8_t command_id;
  uint8_t channel_id;
  uint8_t value_id;
  uint8_t *data;
} via_custom_command_t;

#define VIA_CUSTOM_COMMAND(data) \
  { .command_id = data[0], .channel_id = data[1], .value_id = data[2], .data = &data[3] }

uint8_t via_read_dropdown_value(via_custom_command_t *command);
void via_write_dropdown_value(via_custom_command_t *command, uint8_t value);

bool via_read_toggle_value(via_custom_command_t *command);
void via_write_toggle_value(via_custom_command_t *command, bool value);

uint8_t via_read_range_byte_value(via_custom_command_t *command);
void via_write_range_byte_value(via_custom_command_t *command, uint8_t value);

uint16_t via_read_range_word_value(via_custom_command_t *command);
void via_write_range_word_value(via_custom_command_t *command, uint16_t value);

uint16_t via_read_keycode_value(via_custom_command_t *command);
void via_write_keycode_value(via_custom_command_t *command, uint16_t keycode);

bool via_custom_value_command_user(via_custom_command_t *command);

void defer_eeprom_update_byte(uint8_t channel_id, uint8_t value_id, void *adrs, uint8_t value);
void defer_eeprom_update_word(uint8_t channel_id, uint8_t value_id, void *adrs, uint16_t value);
void defer_eeprom_update_dword(uint8_t channel_id, uint8_t value_id, void *adrs, uint32_t value);
void defer_eeprom_update_block(uint8_t channel_id, uint8_t value_id, void *block_adrs,
                               void *eeprom_adrs, uint32_t block_size);
