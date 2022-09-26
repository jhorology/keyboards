/*
Copyright 2019 Holten Campbell
Modified 2922 jhorology

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "config_common.h"

/* key matrix size */
#define MATRIX_ROWS 4
#define MATRIX_COLS 13

/* Keyboard Matrix Assignments */
#define MATRIX_ROW_PINS \
  { E6, C7, B5, B4 }
#define MATRIX_COL_PINS \
  { F0, F1, F4, F5, F6, F7, D6, D4, D5, D3, D2, D1, D0 }

/* COL2ROW, ROW2COL */
#define DIODE_DIRECTION COL2ROW

#define RGB_DI_PIN B7
#define RGBLED_NUM 8

/* Vial-specific definitions. */
#ifdef VIAL_ENABLE
#  define VIAL_KEYBOARD_UID \
    { 0xF3, 0xB8, 0xA3, 0x29, 0x37, 0x91, 0xFD, 0xA2 }
#  define VIAL_UNLOCK_COMBO_ROWS \
    { 0, 1 }
#  define VIAL_UNLOCK_COMBO_COLS \
    { 0, 12 }
#endif

/* reducing firmware size */
#ifdef VIAL_ENABLE
#  define DYNAMIC_KEYMAP_LAYER_COUNT 4
#else
#  define DYNAMIC_KEYMAP_LAYER_COUNT 8
#  define RGBLIGHT_ANIMATIONS
#endif
