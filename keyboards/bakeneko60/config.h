/*
Copyright 2020 Koichi Katano
Modified 2022 by jhorology

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

#define MATRIX_ROWS 5
#define MATRIX_COLS 15

#define MATRIX_ROW_PINS \
  { E6, B7, F7, F4, F5 }
#define MATRIX_COL_PINS \
  { F6, B0, F1, C7, C6, B6, B5, B4, D7, D6, D4, D5, D3, D2, D1 }
#define DIODE_DIRECTION COL2ROW

/* Vial-specific definitions. */
#ifdef VIAL_ENABLE
#  define VIAL_KEYBOARD_UID \
    { 0xB5, 0xA1, 0xF9, 0x13, 0x8C, 0x37, 0xFB, 0x6A }
#  define VIAL_UNLOCK_COMBO_ROWS \
    { 0, 2 }
#  define VIAL_UNLOCK_COMBO_COLS \
    { 0, 13 }
#endif

/* ViA layout options */
#ifdef VIA_ENABLE
#  define LAYOUT_OPTION_SPLIT_BS 0x4
#  define LAYOUT_OPTION_SPLIT_RIGHT_SHIFT 0x2
#  define LAYOUT_OPTION_BOTTOM_ROW_ANSI 0
#  define LAYOUT_OPTION_BOTTOM_ROW_TSANGAN 0x1
#endif
