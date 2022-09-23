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

// clang-format off
#define LAYOUT( \
  K000, K001, K002, K003, K004, K005, K006, K007, K008, K009, K010, K011, K012, K013, K014, \
  K100, K101, K102, K103, K104, K105, K106, K107, K108, K109, K110, K111, K112, K113, K114, \
  K200, K201, K202, K203, K204, K205, K206, K207, K208, K209, K210, K211,       K213, K214, \
  K300, K301, K302, K303, K304, K305, K306, K307, K308, K309, K310,       K312, K313, K314, \
  K400, K401, K402,            K405,                 K409, K410, K411, K412, K413, K414  \
) { \
  { K000,  K001,  K002,  K003,  K004,  K005,  K006,  K007,  K008,  K009,  K010,  K011,  K012,  K013,  K014 }, \
  { K100,  K101,  K102,  K103,  K104,  K105,  K106,  K107,  K108,  K109,  K110,  K111,  K112,  K113,  K114 }, \
  { K200,  K201,  K202,  K203,  K204,  K205,  K206,  K207,  K208,  K209,  K210,  K211,  KC_NO, K213,  K214 }, \
  { K300,  K301,  K302,  K303,  K304,  K305,  K306,  K307,  K308,  K309,  K310,  KC_NO, K312,  K313,  K314 }, \
  { K400,  K401,  K402,  KC_NO, KC_NO, K405,  KC_NO, KC_NO, KC_NO, K409,  K410,  K411,  K412,  K413,  K414 } \
}
// clang-format on

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
