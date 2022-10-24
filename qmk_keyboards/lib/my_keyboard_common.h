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

#include "lib/apple_fn.h"
#include "lib/custom_config.h"
#include "lib/custom_keycodes.h"
#include "lib/jis_util.h"
#ifdef RADIAL_CONTROLLER_ENABLE
#  include "lib/radial_controller.h"
#endif
#include "lib/tap_dance.h"
#include "lib/via_custom_menus.h"

extern const tap_dance_entry_t tap_dance_entries_default[TAP_DANCE_ENTRIES];
