#pragma once

#define MATRIX_ROWS 5
#define MATRIX_COLS 14

#define MATRIX_ROW_PINS \
  { F6, F5, F4, F1, F0 }
#define MATRIX_COL_PINS \
  { D0, D1, D2, D3, D5, D4, D6, D7, B4, B5, B6, C6, C7, F7 }
#define DIODE_DIRECTION COL2ROW

#define LED_CAPS_LOCK_PIN B7
#define LED_PIN_ON_STATE 0

/* ViA layout options */

// option 0: 5 bit
#define LAYOUT_OPTION_ISO_ENTER (0x1 << 5)

// option 1: 3-4 bit
#define LAYOUT_OPTION_BOTTOM_ROW_ANSI 0
#define LAYOUT_OPTION_BOTTOM_ROW_TSANGAN (0x1 << 3)
#define LAYOUT_OPTION_BOTTOM_ROW_HHKB (0x2 << 3)
#define LAYOUT_OPTION_BOTTOM_ROW_WKL (0x3 << 3)

// option 2: 2 bit
#define LAYOUT_OPTION_SPLIT_BS (0x1 << 2)

// option 3: 1 bit
#define LAYOUT_OPTION_SPLIT_LEFT_SHIFT (0x1 << 1)

// option 4: 0 bit
#define LAYOUT_OPTION_SPLIT_RIGHT_SHIFT 0x1

#if __has_include("secure_config.h")
#  include "secure_config.h"
#else
#  define VIA_FIRMWARE_VERSION 0
#  define SECURE_UNLOCK_SEQUENCE \
    {                            \
      {0, 0}, { 2, 13 }          \
    }
#endif
