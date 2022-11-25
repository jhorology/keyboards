/*
Copyright 2022 Qwertykeys

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
#define MATRIX_ROWS 5
#define MATRIX_COLS 14

/* Keyboard Matrix Assignments */
#define MATRIX_ROW_PINS \
  { A1, A2, A3, A4, A5 }
#define MATRIX_COL_PINS \
  { A7, B0, B1, B2, B10, B11, B12, B13, B14, B15, A8, B6, B7, A6 }
/* COL2ROW, ROW2COL */
#define DIODE_DIRECTION COL2ROW

#define LED_CAPS_LOCK_PIN A0
#define LED_PIN_ON_STATE 0

/* VIA Layout Options */
// bit 4
#define LAYOUT_OPTION_SPLIT_BS 0x10
// bit 2-3
#define LAYOUT_OPTION_BOTTOM_ROW_TSANGAN 0x0
#define LAYOUT_OPTION_BOTTOM_ROW_HHKB 0x4
#define LAYOUT_OPTION_BOTTOM_ROW_WKL 0x8
// bit 0-1
#define LAYOUT_OPTION_SPACEBAR_7U 0x0
#define LAYOUT_OPTION_SPACEBAR_2_25U 0x1
#define LAYOUT_OPTION_SPACEBAR_2_75U 0x2

#if __has_include("secure_config.h")
#  include "secure_config.h"
#else
#  define VIA_FIRMWARE_VERSION 0
#  define SECURE_UNLOCK_SEQUENCE \
    {                            \
      {0, 0}, { 2, 12 }          \
    }
#endif
