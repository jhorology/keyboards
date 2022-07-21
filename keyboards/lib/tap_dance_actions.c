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
#include QMK_KEYBOARD_H
#include "tap_dance_actions.h"

#ifndef VIAL_ENABLE
qk_tap_dance_action_t tap_dance_actions[] = {
    // Tap once for standard key, twice to toggle layers
    [TD_ALTIME] = ACTION_TAP_DANCE_DOUBLE(KC_LALT, LALT(KC_GRV))};
#endif

#ifdef VIAL_TAP_DANCE_ENABLE
const vial_tap_dance_entry_t PROGMEM vial_tap_dance_actions_default[] = {
    // tap, hold,  double_tap, tap_hold, tapping_term
    [TD_ALTIME] = {KC_LALT, KC_LALT, LALT(KC_GRV), KC_LALT, TAPPING_TERM}};
#endif
