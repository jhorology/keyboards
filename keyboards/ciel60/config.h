#pragma once

#include "config_common.h"

#define MATRIX_ROWS 5
#define MATRIX_COLS 15

#define MATRIX_ROW_PINS \
  { C5, C4, B0, C7, B7 }
#define MATRIX_COL_PINS \
  { C6, B6, B5, B4, B3, B2, B1, D6, D5, D4, D3, D2, D1, D0, C2 }
#define DIODE_DIRECTION COL2ROW

/* Vial-specific definitions. */
#ifdef VIAL_ENABLE
#  define VIAL_KEYBOARD_UID \
    { 0xC5, 0xA0, 0x98, 0xD4, 0x2F, 0xE1, 0x1A, 0x7B }
#  define VIAL_UNLOCK_COMBO_ROWS \
    { 0, 2 }
#  define VIAL_UNLOCK_COMBO_COLS \
    { 0, 13 }
#endif

/* ViA layout options */
#define LAYOUT_OPTION_SPLIT_BS 0x4
#define LAYOUT_OPTION_SPLIT_RIGHT_SHIFT 0x2
#define LAYOUT_OPTION_BOTTOM_ROW_ANSI 0
#define LAYOUT_OPTION_BOTTOM_ROW_TSANGAN 0x1
