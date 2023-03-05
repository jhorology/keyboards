/* Copyright 2022 jhorology
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

#include "lib/my_keyboard_common.h"

// custom keycodes

// Toggle keylight on/off, toggle underglow on/off when shift is held.
#define RGB_TOGX CUSTOM_KEYCODES_SAFE_RANGE

#ifdef RGB_MATRIX_ENABLE
typedef union {
  uint32_t raw;
  struct {
    bool keylight_enable : 1;
    bool underglow_enable : 1;
  };
} user_config_t;
extern user_config_t g_user_config;

void set_keylight_enable(bool enable);
void set_underglow_enable(bool enable);
#endif
