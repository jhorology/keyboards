/* Copyright 2023 Cipulot
 * Modified 2023 masafumi
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

#define MATRIX_ROW_PINS \
  { B15, A8, B0, A7, B1 }

#define AMUX_COUNT 2
#define AMUX_MAX_COLS_COUNT 8

#define AMUX_EN_PINS \
  { B7, B3 }

#define AMUX_SEL_PINS \
  { B6, B5, B4 }

#define MATRIX_COL_CHANNEL(amux, ch) ((amux << 4) + ch)
// cla
#define MATRIX_COL_CHANNELS                                                                                   \
  {                                                                                                           \
    MATRIX_COL_CHANNEL(2, 0), MATRIX_COL_CHANNEL(2, 3), MATRIX_COL_CHANNEL(2, 1), MATRIX_COL_CHANNEL(2, 2),   \
      MATRIX_COL_CHANNEL(2, 5), MATRIX_COL_CHANNEL(2, 7), MATRIX_COL_CHANNEL(2, 6), MATRIX_COL_CHANNEL(2, 4), \
      MATRIX_COL_CHANNEL(1, 0), MATRIX_COL_CHANNEL(1, 3), MATRIX_COL_CHANNEL(1, 1), MATRIX_COL_CHANNEL(1, 2), \
      MATRIX_COL_CHANNEL(1, 5), MATRIX_COL_CHANNEL(1, 7), MATRIX_COL_CHANNEL(1, 6)                            \
  }

#define DISCHARGE_PIN A6
#define ANALOG_PORT A3
#define DISCHARGE_TIME 10
#define NOISE_FLOOR_SAMPLING_COUNT 30

#define EC_ACTUATION_THRESHOLD_DEFAULT 600
#define EC_RELEASE_THRESHOLD_DEFAULT 450
#define EC_ACTUATION_TRAVEL_DEFAULT 200
#define EC_RELEASE_TRAVEL_DEFAULT 200
#define EC_DEADZONE_DEFAULT 200
#define EC_BOTTOMING_READING_DEFAULT 1023
#define EC_BOOTMAGIC_LITE_THRESHOLD 0x180

// 2 - 16
#define EC_NUM_PRESETS 8
// 1 - 8
#define EC_NUM_PRESET_MAPS 4

// keymap * 4 + EC preset map * 4
#define DYNAMIC_KEYMAP_LAYER_COUNT 8

/* use calibrated bottming value as default */
#define ENABLE_CALIBRATED_BOTTOMING_READING

// #define VIA_EC_CUSTOM_CHANNEL_ID id_custom_channel_user_range
#define EC_VIA_CUSTOM_CHANNEL_ID_START 16

/* increase eeprom size */
#define WEAR_LEVELING_LOGICAL_SIZE 8192
#define WEAR_LEVELING_BACKING_SIZE (WEAR_LEVELING_LOGICAL_SIZE * 2)

// #define EECONFIG_KB_DATA_SIZE 160
// #define EECONFIG_USER_DATA_SIZE (10 + MATRIX_COLS * MATRIX_ROWS * 2)

/* ViA EEPROM */
#undef VIA_EEPROM_CUSTOM_CONFIG_SIZE
/*
  preset 4byte * EC_NUM_PRESETS
  preset_map 4bit * matrix
  bottoming_reading 2byte * matrix
*/
#define VIA_EEPROM_CUSTOM_CONFIG_SIZE \
  (VIA_EEPROM_CUSTOM_CONFIG_COMMON_SIZE + EC_NUM_PRESETS * 8 + MATRIX_COLS * MATRIX_ROWS * 2 + 2)

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

// for debugging
// #define CUSTOM_CONFIG_RHID_DEFAULT true
// #define EC_DEBUG 1
