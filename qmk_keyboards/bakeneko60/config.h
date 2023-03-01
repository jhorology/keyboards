#pragma once

#define MATRIX_ROWS 5
#define MATRIX_COLS 15

#define MATRIX_ROW_PINS \
  { E6, B7, F7, F4, F5 }
#define MATRIX_COL_PINS \
  { F6, B0, F1, C7, C6, B6, B5, B4, D7, D6, D4, D5, D3, D2, D1 }
#define DIODE_DIRECTION COL2ROW

/* ViA layout options */
#define LAYOUT_OPTION_SPLIT_BS 0x4
#define LAYOUT_OPTION_SPLIT_RIGHT_SHIFT 0x2
#define LAYOUT_OPTION_BOTTOM_ROW_ANSI 0
#define LAYOUT_OPTION_BOTTOM_ROW_TSANGAN 0x1

#if __has_include("secure_config.h")
#  include "secure_config.h"
#else
#  define VIA_FIRMWARE_VERSION 0
#  define SECURE_UNLOCK_SEQUENCE \
    {                            \
      {0, 0}, { 2, 13 }          \
    }
#endif
