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

#include <quantum.h>

typedef struct {
  uint16_t on_single_tap;
  uint16_t on_single_hold;
  uint16_t on_multi_tap;
  uint16_t on_tap_hold;
  uint16_t tapping_term : 10;
  uint8_t reserved : 6;  // reserved for future use
} __attribute__((packed)) tap_dance_entry_t;

typedef enum {
  TD_NONE,
  TD_SINGLE_TAP,
  TD_SINGLE_HOLD,
  TD_MULTI_TAP,
  TD_TAP_HOLD,
  TD_UNKNOWN,
} tap_dance_state_t;

typedef struct {
  uint16_t index;
  tap_dance_state_t state;
  keyrecord_t record;
} tap_dance_data_t;

uint16_t tap_dance_get_tapping_term(uint16_t keycode, keyrecord_t *record);
void tap_dance_actions_init(void);
bool process_tap_dance_store_event(uint16_t keycode, keyrecord_t *record);
