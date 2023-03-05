#pragma once

#define RGB_DI_PIN E2
#define RGBLED_NUM 20
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
