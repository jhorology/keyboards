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

#include <nvm/nvm_via.h>
#include <quantum.h>
#include <via.h>

#include "apple_fn.h"
#include "tap_dance.h"

// apple fn override modes for non-macos
typedef union {
  uint32_t raw;
  struct {
    bool raw_hid : 1;               // allow access to raw hid
    bool usj : 1;                   // ANSI layou on JIS.
    bool mac : 1;                   // mac mode.
    bool auto_detect : 1;           // auto detect mac/non-mac mode.
    uint8_t non_mac_fn_fkey : 1;    // none mac fn key enable for F1-12
    uint8_t non_mac_fn_alpha : 1;   // none mac fn key enable for alpha key
    uint8_t non_mac_fn_cursor : 1;  // none mac fn key enable for cursor key
  };
} kb_config_t;
extern kb_config_t kb_config;

#ifdef RADIAL_CONTROLLER_ENABLE
typedef union {
  uint32_t raw;
  struct {
    uint8_t encoder_clicks;       // encoder clicks per rotation
    uint8_t key_angular_speed;    // degree per second, 15 - 270 (offset 15)
    uint8_t fine_tune_ratio : 2;  // power-of-2 divider 0: none, 1: 1/2, 2:1/4, 3:1/8
    uint8_t fine_tune_mods : 5;   // bit0: ctrl, bit1: shift, bit2: alt, bit3: gui, bit4: fn🌐
  };
} rc_config_t;
extern rc_config_t rc_config;
#endif

bool process_record_custom_config(uint16_t keycode, keyrecord_t *record);

void custom_config_reset(void);
void custom_config_init(void);

bool custom_config_raw_hid_is_enable(void);
void custom_config_raw_hid_toggle_enable(void);
void custom_config_raw_hid_set_enable(bool);

void dynamic_none_mac_apple_fn_fkeys_reset(uint16_t *fkeys);
void dynamic_none_mac_apple_fn_fkeys_keycode(uint8_t fkeys_index);

bool custom_config_usj_is_enable(void);
void custom_config_usj_toggle_enable(void);
void custom_config_usj_set_enable(bool);

bool custom_config_mac_is_enable(void);
void custom_config_mac_toggle_enable(void);
void custom_config_mac_set_enable(bool);

bool custom_config_auto_detect_is_enable(void);
void custom_config_auto_detect_toggle_enable(void);
void custom_config_auto_detect_set_enable(bool);

void custom_config_mac_set_enable_without_reset(bool);

uint8_t custom_config_non_mac_fn_fkey_is_enable(void);
void custom_config_non_mac_fn_set_fkey(bool);
uint8_t custom_config_non_mac_fn_alpha_is_enable(void);
void custom_config_non_mac_fn_set_alpha(bool);
uint8_t custom_config_non_mac_fn_cursor_is_enable(void);
void custom_config_non_mac_fn_set_cursor(bool);

#ifdef RADIAL_CONTROLLER_ENABLE
uint8_t custom_config_rc_get_encoder_clicks(void);
uint16_t custom_config_rc_get_key_angular_speed(void);
uint8_t custom_config_rc_get_fine_tune_ratio(void);
uint8_t custom_config_rc_is_fine_tune_mods(void);
bool custom_config_rc_is_fine_tune_mods_now(void);
#endif

void dynamic_tap_dance_reset(void);
uint16_t dynamic_tap_dance_keycode(uint8_t index, tap_dance_event_t event);
uint16_t dynamic_tap_dance_tapping_term(uint8_t index);

void dynamic_non_mac_fn_reset(const uint16_t *keycodes, uint8_t len);
uint16_t dynamic_non_mac_fn_keycode(non_mac_fn_key_t fn_key);

static inline uint8_t nvm_via_read_byte(uint32_t offset) {
  uint8_t value = 0;
  nvm_via_read_custom_config((void *)&value, offset, 1);
  return value;
}

static inline uint16_t nvm_via_read_word(uint32_t offset) {
  uint16_t value = 0;
  nvm_via_read_custom_config((void *)&value, offset, 2);
  return value;
}

static inline uint16_t nvm_via_read_dword(uint32_t offset) {
  uint32_t value = 0;
  nvm_via_read_custom_config((void *)&value, offset, 4);
  return value;
}

static inline void nvm_via_update_byte(uint32_t offset, uint8_t value) {
  nvm_via_update_custom_config((void *)&value, offset, 1);
}

static inline void nvm_via_update_word(uint32_t offset, uint16_t value) {
  nvm_via_update_custom_config((void *)&value, offset, 2);
}

static inline void nvm_via_update_dword(uint32_t offset, uint32_t value) {
  nvm_via_update_custom_config((void *)&value, offset, 4);
}
