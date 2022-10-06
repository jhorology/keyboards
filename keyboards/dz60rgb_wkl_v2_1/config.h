#pragma once

/* key matrix size */
#define MATRIX_ROWS 5
#define MATRIX_COLS 14

/*
 * Keyboard Matrix Assignments
 *
 * Change this to how you wired your keyboard
 * COLS: AVR pins used for columns, left to right
 * ROWS: AVR pins used for rows, top to bottom
 * DIODE_DIRECTION: COL2ROW = COL = Anode (+), ROW = Cathode (-, marked on diode)
 *                  ROW2COL = ROW = Anode (+), COL = Cathode (-, marked on diode)
 *
 */
#define MATRIX_ROW_PINS \
  { F5, F4, F1, B3, B2 }
#define MATRIX_COL_PINS \
  { C7, F7, F6, F0, B0, B1, B4, D7, D6, D4, D5, D3, D2, B7 }

/* COL2ROW, ROW2COL*/
#define DIODE_DIRECTION COL2ROW

#define RGB_DISABLE_AFTER_TIMEOUT 0     // number of ticks to wait until disabling effects
#define RGB_DISABLE_WHEN_USB_SUSPENDED  // turn off effects when suspended

#define RGB_MATRIX_FRAMEBUFFER_EFFECTS
#define RGB_MATRIX_KEYPRESSES
#define ENABLE_RGB_MATRIX_SOLID_COLOR
#define ENABLE_RGB_MATRIX_BREATHING       // Single hue brightness cycling animation
#define ENABLE_RGB_MATRIX_TYPING_HEATMAP  // How hot is your WPM!

#define DRIVER_ADDR_1 0b1010000
#define DRIVER_COUNT 1
#define DRIVER_LED_TOTAL 62

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
