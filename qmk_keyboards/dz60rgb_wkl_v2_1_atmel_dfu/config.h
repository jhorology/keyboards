#pragma once

// #define DEBOUNCE 10

#define IS31FL3733_I2C_ADDRESS_1 IS31FL3733_I2C_ADDRESS_GND_GND

/* ViA layout options */
#define LAYOUT_OPTION_HHKB 0x0
#define LAYOUT_OPTION_TSANGAN 0x1

/* definitions for my_keyboard_commoon lib */
#define RGB_MATRIX_CAPS_LOCK_LED 41

#if __has_include("secure_config.h")
#  include "secure_config.h"
#else
#  define VIA_FIRMWARE_VERSION 0
#  define SECURE_UNLOCK_SEQUENCE \
    {                            \
      {0, 0}, { 2, 13 }          \
    }
#endif
