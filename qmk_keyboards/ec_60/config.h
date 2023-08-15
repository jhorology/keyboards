/* Copyright 2023 Cipulot
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

#define MATRIX_ROWS 5
#define MATRIX_COLS 15

/* Custom matrix pins and port select array */
#define MATRIX_ROW_PINS \
  { B15, A8, B0, A7, B1 }
#define MATRIX_COL_CHANNELS \
  { 0, 3, 1, 2, 5, 7, 6, 4 }
#define MUX_SEL_PINS \
  { B6, B5, B4 }

/* Hardware peripherals pins */
#define APLEX_EN_PIN_0 B7
#define APLEX_EN_PIN_1 B3
#define DISCHARGE_PIN A6
#define ANALOG_PORT A3

/* Mechanical locking support. Use KC_LCAP, KC_LNUM or KC_LSCR instead in keymap */
#define LOCKING_SUPPORT_ENABLE

/* Locking resynchronize hack */
#define LOCKING_RESYNC_ENABLE

#define DEFAULT_ACTUATION_LEVEL 500
#define DEFAULT_RELEASE_LEVEL 400

#define DISCHARGE_TIME 10

/* use EEPROM for actuation settnigs */
#define EECONFIG_USER_DATA_SIZE 4

/* ViA layout options */
/*  7 bit */
#define LAYOUT_OPTION_SPLIT_BS (1 << 7)

/*  6 bit */
#define LAYOUT_OPTION_SPLIT_LSHIFT (1 << 6)

/*  4-5 bit */
#define LAYOUT_OPTION_SPLIT_RSHIFT_UNIFIED (0 << 4)
#define LAYOUT_OPTION_SPLIT_RSHIFT_1U_1_75U (1 << 4)
#define LAYOUT_OPTION_SPLIT_RSHIFT_1_75U_1U (2 << 4)

/*  3 bit */
#define LAYOUT_OPTION_ENTER_JIS_ISO (0 << 3)
#define LAYOUT_OPTION_ENTER_ANSI (1 << 3)

/*  0-2 bit */
#define LAYOUT_OPTION_BOTTOM_ROW_JIS 0
#define LAYOUT_OPTION_BOTTOM_ROW_TRUE_HHKB 1
#define LAYOUT_OPTION_BOTTOM_ROW_TRUE_HHKB_3U_3U 2
#define LAYOUT_OPTION_BOTTOM_ROW_RF_6U 3
#define LAYOUT_OPTION_BOTTOM_ROW_7U 4
#define LAYOUT_OPTION_BOTTOM_ROW_7U_HHKB 5

#if __has_include("secure_config.h")
#  include "secure_config.h"
#else
#  define VIA_FIRMWARE_VERSION 0
#  define SECURE_UNLOCK_SEQUENCE \
    {                            \
      {0, 0}, { 2, 13 }          \
    }
#endif

#define CUSTOM_CONFIG_RHID_DEFAULT true
