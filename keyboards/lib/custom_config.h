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

#include "quantum.h"
#ifndef VIAL_ENABLE
#  include "tap_dance.h"
#endif
#include "radial_controller.h"

#define RADIAL_CONTROLLER_EEPROM_ADDR VIA_EEPROM_CUSTOM_CONFIG_ADDR
#ifndef VIAL_ENABLE
#  define DYNAMIC_TAP_DANCE_EEPROM_ADDR (RADIAL_CONTROLLER_EEPROM_ADDR + 4)
#endif

void custom_config_reset(void);
void custom_config_init(void);

bool custom_config_is_mac(void);
void custom_config_toggle_mac(void);
void custom_config_set_mac(bool);

bool custom_config_is_usj(void);
void custom_config_toggle_usj(void);
void custom_config_set_usj(bool);

uint16_t custom_config_get_rc_deg_per_click(void);
void custom_config_set_rc_deg_per_click(uint16_t);

uint16_t custom_config_get_rc_deg_per_sec(void);
void custom_config_set_rc_deg_per_sec(uint16_t);

#ifndef VIAL_ENABLE
void dynamic_tap_dance_reset(const tap_dance_entry_t *entry, uint8_t len);
uint16_t dynamic_tap_dance_keycode(uint16_t index, tap_dance_state_t state);
uint16_t dynamic_tap_dance_tapping_term(uint16_t index);
#endif

void pgm_memcpy(void *dest, const void *src, size_t len);
