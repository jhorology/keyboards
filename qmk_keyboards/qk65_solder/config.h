#pragma once

#include "config_common.h"

/* key matrix size */
#define MATRIX_ROWS 5
#define MATRIX_COLS 15

/* Keyboard Matrix Assignments */
#define MATRIX_ROW_PINS \
  { B0, B7, D0, D1, D2 }
#define MATRIX_COL_PINS \
  { D5, D4, D6, D7, B4, B5, B6, C6, C7, F7, F6, F5, F4, F1, F0 }
/* COL2ROW, ROW2COL */
#define DIODE_DIRECTION COL2ROW

/* ViA layout options */
#define LAYOUT_OPTION_SPLIT_BS 0x8
#define LAYOUT_OPTION_ISO_ENTER 0x4
#define LAYOUT_OPTION_SPLIT_LEFT_SHIFT 0x2
#define LAYOUT_OPTION_BOTTOM_ROW_6dot25U 0x0
#define LAYOUT_OPTION_BOTTOM_ROW_7U 0x1

#if __has_include("secure_config.h")
#  include "secure_config.h"
#else
#  define VIA_FIRMWARE_VERSION 0
#  define SECURE_UNLOCK_SEQUENCE \
    {                            \
      {0, 0}, { 3, 13 }          \
    }
#endif
