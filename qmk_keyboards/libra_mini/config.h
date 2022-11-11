/* SPDX-License-Identifier: GPL-2.0-or-later */

#pragma once

/* Matrix config */

#define MATRIX_ROWS 4
#define MATRIX_COLS 12

#define MATRIX_ROW_PINS \
  { E6, B4, B5, B2 }
#define MATRIX_COL_PINS \
  { B3, B1, F7, F6, F5, F4, C6, D4, D0, D1, D2, D3 }

#define DIODE_DIRECTION ROW2COL

#define DYNAMIC_KEYMAP_LAYER_COUNT 5

/* Joystick config */

#define ANALOG_JOYSTICK_Y_AXIS_PIN B6
#define ANALOG_JOYSTICK_X_AXIS_PIN D7

#define POINTING_DEVICE_INVERT_X
#define POINTING_DEVICE_INVERT_Y

/* Cursor speed */
#define ANALOG_JOYSTICK_SPEED_MAX 5
#define ANALOG_JOYSTICK_SPEED_REGULATOR 10

/* Mouse inertia (keeps sliding after a flick) */
// #define POINTING_DEVICE_GESTURES_CURSOR_GLIDE_ENABLE

/* ViA layout options */
#define LAYOUT_OPTION_SPLIT_BS 0x2
#define LAYOUT_OPTION_SPLIT_JOYSTICK 0x1

#if __has_include("secure_config.h")
#  include "secure_config.h"
#else
#  define VIA_FIRMWARE_VERSION 0
#  define SECURE_UNLOCK_SEQUENCE \
    {                            \
      {0, 0}, { 2, 11 }          \
    }
#endif
