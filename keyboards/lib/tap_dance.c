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

#include "tap_dance.h"

#define IS_CUSTOM_KEYCODE(kc) (kc >= SAFE_RANGE || (kc >= USER00 && kc <= USER15))
static void on_tap_dance_finished(qk_tap_dance_state_t *state, tap_dance_data_t *data);
static void on_tap_dance_reset(qk_tap_dance_state_t *state, tap_dance_data_t *data);

tap_dance_data_t tap_dance_datas[VIA_TAP_DANCE_ENTRIES];
qk_tap_dance_action_t tap_dance_actions[VIA_TAP_DANCE_ENTRIES];

// TODO support VIA v3 UI
void via_tap_dance_action_init(uint16_t size) {
  for (int i = 0; i < size; i++) {
    if (i < VIA_TAP_DANCE_ENTRIES) {
      tap_dance_datas[i].index = i;
      tap_dance_datas[i].state = TD_NONE;
      tap_dance_actions[i].fn.on_each_tap = NULL;
      tap_dance_actions[i].fn.on_dance_finished = (void (*)(qk_tap_dance_state_t *, void *))on_tap_dance_finished;
      tap_dance_actions[i].fn.on_reset = (void (*)(qk_tap_dance_state_t *, void *))on_tap_dance_reset;
      tap_dance_actions[i].user_data = &tap_dance_datas[i];
    };
  }
}

void tap_dance_store_keyrecord(uint16_t index, keyrecord_t *record) {
  if (index >= 0 && index < VIA_TAP_DANCE_ENTRIES) {
    memcpy(&tap_dance_datas[index].record, record, sizeof(keyrecord_t));
  }
}

static tap_dance_state_t cur_dance(qk_tap_dance_state_t *state) {
  if (state->count == 1) {
    return state->pressed ? TD_SINGLE_HOLD : TD_SINGLE_TAP;
  } else if (state->count >= 2) {
    return state->pressed ? TD_TAP_HOLD : TD_MULTI_TAP;
  } else {
    return TD_UNKNOWN;
  }
}

static void on_tap_dance_finished(qk_tap_dance_state_t *state, tap_dance_data_t *data) {
  data->state = cur_dance(state);
  uint16_t keycode = get_via_tap_dance_keycode(data->index, data->state);
  if (keycode) {
    register_code16(keycode);
    if (IS_CUSTOM_KEYCODE(keycode)) {
      data->record.event.pressed = true;
      process_record_kb(keycode, &data->record);
    } else {
      register_code16(keycode);
    }
  }
}

static void on_tap_dance_reset(qk_tap_dance_state_t *state, tap_dance_data_t *data) {
  uint16_t keycode = get_via_tap_dance_keycode(data->index, data->state);
  if (keycode) {
    if (IS_CUSTOM_KEYCODE(keycode)) {
      data->record.event.pressed = false;
      process_record_kb(keycode, &data->record);
    } else {
      unregister_code16(keycode);
    }
  }
  data->state = TD_NONE;
}
