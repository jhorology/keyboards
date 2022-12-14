#pragma once

#include "config_common.h"

#define MATRIX_ROWS 5
#define MATRIX_COLS 15

#define MATRIX_ROW_PINS \
  { C5, C4, B0, C7, B7 }
#define MATRIX_COL_PINS \
  { C6, B6, B5, B4, B3, B2, B1, D6, D5, D4, D3, D2, D1, D0, C2 }
#define DIODE_DIRECTION COL2ROW

/* ViA layout options */
#define LAYOUT_OPTION_SPLIT_BS 0x4
#define LAYOUT_OPTION_SPLIT_RIGHT_SHIFT 0x2
#define LAYOUT_OPTION_BOTTOM_ROW_ANSI 0
#define LAYOUT_OPTION_BOTTOM_ROW_TSANGAN 0x1

// TODO in only this board, when TAP_DANCE_ENTRIES is greater than 2 or 3, sometimes freez on tap dance keys.
#undef TAP_DANCE_ENTRIES
#define TAP_DANCE_ENTRIES 2

#if __has_include("secure_config.h")
#  include "secure_config.h"
#else
#  define VIA_FIRMWARE_VERSION 0
#  define SECURE_UNLOCK_SEQUENCE \
    {                            \
      {0, 0}, { 2, 13 }          \
    }
#endif
