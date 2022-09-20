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
#pragma once

#include "quantum.h"
#ifndef VIA_TAP_DANCE_ENTRIES
#  define VIA_TAP_DANCE_ENTRIES 8
#endif

typedef struct {
  uint16_t on_single_tap;
  uint16_t on_single_hold;
  uint16_t on_double_tap;
  uint16_t on_double_hold;
  uint16_t tapping_term;
} tap_dance_entry_t;

typedef enum {
  TD_NONE,
  TD_SINGLE_TAP,
  TD_SINGLE_HOLD,
  TD_DOUBLE_TAP,
  TD_DOUBLE_HOLD,
  TD_UNKNOWN,
} tap_dance_state_t;

typedef struct {
  uint16_t index;
  tap_dance_state_t state;
  keyrecord_t record;
} tap_dance_data_t;

void via_tap_dance_action_init(uint16_t size);
void tap_dance_store_keyrecord(uint16_t index, keyrecord_t *record);
uint16_t get_via_tap_dance_keycode(uint16_t index, tap_dance_state_t state);
