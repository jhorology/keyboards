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
  id_custom_rc_channel,                                                           // Radial Controller
  id_custom_non_mac_fn_channel,                                                   // none mac fn functions
  id_custom_td_channel_start,                                                     // Tap Dance start
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
  id_custom_non_mac_fn_mode,
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
  id_custom_non_mac_fn_spc,  // Hey Siri
  id_custom_non_mac_fn_q,    // Qick Notes
  id_custom_non_mac_fn_e,    // Emoji & Symbols
  id_custom_non_mac_fn_a,    // Focus Dock
  id_custom_non_mac_fn_d,    // Start Dictation
  id_custom_non_mac_fn_f,    // Toggle full screen mode
  id_custom_non_mac_fn_h,    // Show Desktop
  id_custom_non_mac_fn_c,    // Show Control Cnecter
  id_custom_non_mac_fn_n,    // Show Notification
  id_custom_non_mac_fn_m,    // Focus Menubar
  id_custom_non_mac_fn_bspc  // Delete
};

bool via_custom_value_command_user(uint8_t *data, uint8_t length);

void via_custom_magic_get_value(uint8_t value_id, uint8_t *value_data);
void via_custom_magic_set_value(uint8_t value_id, uint8_t *value_data);
#ifdef RADIAL_CONTROLLER_ENABLE
void via_custom_rc_get_value(uint8_t value_id, uint8_t *value_data);
void via_custom_rc_set_value(uint8_t value_id, uint8_t *value_data);
void via_custom_rc_save(void);
#endif
void via_custom_td_get_value(uint8_t td_index, uint8_t value_id, uint8_t *value_data);
void via_custom_td_set_value(uint8_t td_index, uint8_t value_id, uint8_t *value_data);
void via_custom_td_save(uint8_t td_index);

void via_custom_non_mac_fn_get_value(uint8_t value_id, uint8_t *value_data);
void via_custom_non_mac_fn_set_value(uint8_t value_id, uint8_t *value_data);
