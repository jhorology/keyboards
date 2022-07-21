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

#include "vial.h"

#ifdef VIAL_TAP_DANCE_ENABLE
extern const vial_tap_dance_entry_t vial_tap_dance_actions_default[];
#endif

#ifdef VIAL_COMBO_ENABLE
extern const vial_combo_entry_t vial_combo_actions_default[];
#endif

#ifdef VIAL_KEY_OVERRIDE_ENABLE
extern const vial_key_override_entry_t vial_key_override_actions_default[];
#endif
