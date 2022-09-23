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

static void on_tap_dance_finished(qk_tap_dance_state_t *state, tap_dance_data_t *data);
static void on_tap_dance_reset(qk_tap_dance_state_t *state, tap_dance_data_t *data);

tap_dance_data_t tap_dance_datas[TAP_DANCE_ENTRIES];
qk_tap_dance_action_t tap_dance_actions[TAP_DANCE_ENTRIES];

// quantum hook function
// requires TAPPING_TERM_PER_KEY
uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
  uint16_t index = TD_INDEX(keycode);
  if (index >= 0 && index < TAP_DANCE_ENTRIES) {
    return dynamic_tap_dance_tapping_term(index);
  }
  return TAPPING_TERM;
}

void tap_dance_actions_init() {
  keyrecord_t record = {0};
  for (int i = 0; i < TAP_DANCE_ENTRIES; i++) {
    tap_dance_datas[i].index = i;
    tap_dance_datas[i].state = TD_NONE;
    tap_dance_datas[i].record = record;
    tap_dance_actions[i].fn.on_each_tap = NULL;
    tap_dance_actions[i].fn.on_dance_finished = (void (*)(qk_tap_dance_state_t *, void *))on_tap_dance_finished;
    tap_dance_actions[i].fn.on_reset = (void (*)(qk_tap_dance_state_t *, void *))on_tap_dance_reset;
    tap_dance_actions[i].user_data = &tap_dance_datas[i];
  }
}

void tap_dance_store_keyevent(uint16_t index, keyevent_t *event) {
  if (index >= 0 && index < TAP_DANCE_ENTRIES) {
    tap_dance_datas[index].record.event = *event;
  }
}
static void on_tap_dance_finished(qk_tap_dance_state_t *state, tap_dance_data_t *data) {
  if (state->count == 1) {
    data->state = state->pressed ? TD_SINGLE_HOLD : TD_SINGLE_TAP;
  } else if (state->count >= 2) {
    data->state = state->pressed ? TD_TAP_HOLD : TD_MULTI_TAP;
  } else {
    data->state = TD_UNKNOWN;
  }
  uint16_t keycode = dynamic_tap_dance_keycode(data->index, data->state);
  if (keycode) {
    data->record.keycode = keycode;
    data->record.event.pressed = true;
    data->record.event.time = timer_read();
    process_record(&data->record);
  }
}

static void on_tap_dance_reset(qk_tap_dance_state_t *state, tap_dance_data_t *data) {
  if (data->record.keycode) {
    data->record.event.pressed = false;
    data->record.event.time = timer_read();
    process_record(&data->record);
  }
  data->state = TD_NONE;
  data->record.keycode = KC_NO;
}
