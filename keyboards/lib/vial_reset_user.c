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

#include "vial_reset_user.h"

#include "combo_actions.h"
#include "key_override_actions.h"
#include "tap_dance_actions.h"

void pgm_memcpy(uint8_t *dest, uint8_t *src, size_t len);

#ifdef VIAL_TAP_DANCE_ENABLE
void vial_tap_dance_reset_user(uint8_t index, vial_tap_dance_entry_t *entry) {
  if (index < VIAL_TAP_DANCE_ACTIONS_DEFAULT_LENGTH) {
    pgm_memcpy((uint8_t *)entry, (uint8_t *)&vial_tap_dance_actions_default[index], sizeof(vial_tap_dance_entry_t));
  }
}
#endif

#ifdef VIAL_COMBO_ENABLE
void vial_combo_reset_user(uint8_t index, vial_combo_entry_t *entry) {
  if (index < VIAL_COMBO_ACTIONS_DEFAULT_LENGTH) {
    pgm_memcpy((uint8_t *)entry, (uint8_t *)&vial_combo_actions_default[index], sizeof(vial_combo_entry_t));
  }
}
#endif

#ifdef VIAL_KEY_OVERRIDE_ENABLE
void vial_key_override_reset_user(uint8_t index, vial_key_override_entry_t *entry) {
  if (index < VIAL_KEY_OVERRIDE_ACTIONS_DEFAULT_LENGTH) {
    pgm_memcpy((uint8_t *)entry, (uint8_t *)&vial_key_override_actions_default[index],
               sizeof(vial_key_override_entry_t));
  }
}
#endif

void pgm_memcpy(uint8_t *dest, uint8_t *src, size_t len) {
  for (size_t i = 0; i < len; i++) {
    *dest++ = pgm_read_byte(&(*src++));
  }
}
