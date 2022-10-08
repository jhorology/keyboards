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
#define RGBLIGHT_SLEEP

#define RGBLIGHT_EFFECT_ALTERNATING      // Enable alternating animation mode.
#define RGBLIGHT_EFFECT_BREATHING        // Enable breathing animation mode.
#define RGBLIGHT_EFFECT_CHRISTMAS        // Enable christmas animation mode.
#define RGBLIGHT_EFFECT_KNIGHT           // Enable knight animation mode.
#define RGBLIGHT_EFFECT_RAINBOW_MOOD     // Enable rainbow mood animation mode.
#define RGBLIGHT_EFFECT_RAINBOW_SWIRL    // Enable rainbow swirl animation mode.
#define RGBLIGHT_EFFECT_RGB_TEST         // Enable RGB test animation mode.
#define RGBLIGHT_EFFECT_SNAKE            // Enable snake animation mode.
#define RGBLIGHT_EFFECT_STATIC_GRADIENT  // Enable static gradient mode.
#define RGBLIGHT_EFFECT_TWINKLE          // Enable twinkle animation mode.

#define LED_CAPS_LOCK_PIN B1
#define CUSTOM_CONFIG_MAC_MODE_PIN B2
#define CUSTOM_CONFIG_USJ_MODE_PIN B3

#define DYNAMIC_KEYMAP_LAYER_COUNT 5

#if __has_include("secure_config.h")
#  include "secure_config.h"
#else
#  define VIA_FIRMWARE_VERSION 0
#  define SECURE_UNLOCK_SEQUENCE \
    {                            \
      {0, 0}, { 1, 12 }          \
    }
#endif
