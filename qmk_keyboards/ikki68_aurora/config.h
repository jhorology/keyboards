#pragma once

#include "config_common.h"

/* key matrix size */
#define MATRIX_ROWS 5
#define MATRIX_COLS 16

#define MATRIX_ROW_PINS \
  { B7, B3, B2, B1, B0 }
#define MATRIX_COL_PINS \
  { D5, D3, D2, D1, D0, F0, F1, E6, B5, B4, D7, D6, D4, F4, F5, F6 }

#define DIODE_DIRECTION COL2ROW

/* Set 0 if debouncing isn't needed */
#define DEBOUNCE 5

/* Mechanical locking support. Use KC_LCAP, KC_LNUM or KC_LSCR instead in keymap */
#define LOCKING_SUPPORT_ENABLE

/* Locking resynchronize hack */
#define LOCKING_RESYNC_ENABLE

#define LED_CAPS_LOCK_PIN F7
#define LED_PIN_ON_STATE 0

#define RGB_DI_PIN E2
#define RGBLED_NUM 20
#define RGBLIGHT_SLEEP
#define RGBLIGHT_ANIMATIONS

/* ViA layout options */
#define LAYOUT_OPTION_SPLIT_BS 0x10
#define LAYOUT_OPTION_ISO_ENTER 0x8
#define LAYOUT_OPTION_SPLIT_LEFT_SHIFT 0x4
#define LAYOUT_OPTION_SPLIT_RIGHT_SHIFT 0x2
#define LAYOUT_OPTION_SPLIT_SPACE 0x1

#if __has_include("secure_config.h")
#  include "secure_config.h"
#else
#  define VIA_FIRMWARE_VERSION 0
#  define SECURE_UNLOCK_SEQUENCE \
    {                            \
      {0, 0}, { 2, 12 }          \
    }
#endif
