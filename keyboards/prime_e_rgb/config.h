#pragma once

#include "config_common.h"

/* key matrix size */
#define MATRIX_ROWS 4
#define MATRIX_COLS 13

/* Keyboard Matrix Assignments */
#define MATRIX_ROW_PINS \
  { E6, C7, B5, B4 }
#define MATRIX_COL_PINS \
  { F0, F1, F4, F5, F6, F7, D6, D4, D5, D3, D2, D1, D0 }

/* COL2ROW, ROW2COL */
#define DIODE_DIRECTION COL2ROW

#define RGB_DI_PIN B7
#define RGBLED_NUM 8

#define LED_CAPS_LOCK_PIN B1
#define CUSTOM_CONFIG_MAC_MODE_PIN B2
#define CUSTOM_CONFIG_USJ_MODE_PIN B3

/* Vial-specific definitions. */
#ifdef VIAL_ENABLE
#  define VIAL_KEYBOARD_UID \
    { 0xF3, 0xB8, 0xA3, 0x29, 0x37, 0x91, 0xFD, 0xA2 }
#  define VIAL_UNLOCK_COMBO_ROWS \
    { 0, 1 }
#  define VIAL_UNLOCK_COMBO_COLS \
    { 0, 12 }
#endif

#define DYNAMIC_KEYMAP_LAYER_COUNT 5
