#pragma once

#include "config_common.h"

/* key matrix size */
#define MATRIX_ROWS 5
#define MATRIX_COLS 15

/* Key matrix pins */
#define MATRIX_ROW_PINS \
  { B4, B3, A15, A14, A13 }
#define MATRIX_COL_PINS                                                                                             \
  {                                                                                                                 \
    NO_PIN, NO_PIN, NO_PIN, NO_PIN, NO_PIN, NO_PIN, NO_PIN, NO_PIN, NO_PIN, NO_PIN, NO_PIN, NO_PIN, NO_PIN, NO_PIN, \
      NO_PIN                                                                                                        \
  }

/* COL2ROW or ROW2COL */
#define DIODE_DIRECTION ROW2COL

/* Turn off effects when suspended */
#define RGB_DISABLE_WHEN_USB_SUSPENDED

/* Set 0 if debouncing isn't needed */
#define DEBOUNCE 5

/* DIP switch for Mac/win OS switch */
#define DIP_SWITCH_PINS \
  { A8 }

/* Caps lock LED Pin */
#define LED_CAPS_LOCK_PIN A7
#define LED_PIN_ON_STATE 1

/* Hardware configuration */
#define USB_BT_MODE_SELECT_PIN A10

#define CKBT51_RESET_PIN A9
#define CKBT51_INT_INPUT_PIN A5
#define BLUETOOTH_INT_INPUT_PIN A6

#define USB_POWER_SENSE_PIN B1
#define USB_POWER_CONNECTED_LEVEL 0

#define BAT_LOW_LED_PIN A4
#define BAT_LOW_LED_PIN_ON_STATE 1

#define HOST_DEVICES_COUNT 3

#define HOST_LED_PIN_LIST \
  { H3, H3, H3 }
#define HOST_LED_PIN_ON_STATE 1

#define LED_DRIVER_SHUTDOWN_PIN C14

#define HOST_LED_MATRIX_LIST \
  { 16, 17, 18 }

#define BAT_LEVEL_LED_LIST \
  { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 }

/* Backlit disable timeout when keyboard is disconnected(unit: second) */
#define DISCONNECTED_BACKLIGHT_DISABLE_TIMEOUT 40

/* Backlit disable timeout when keyboard is connected(unit: second) */
#define CONNECTED_BACKLIGHT_DISABLE_TIMEOUT 600

/* Keep USB connection in blueooth mode */
#define KEEP_USB_CONNECTION_IN_BLUETOOTH_MODE

/* Enable bluetooth NKRO */
#define BLUETOOTH_NKRO_ENABLE

/* Raw hid command for factory test and bluetooth DFU */
#define RAW_HID_CMD 0xAA ... 0xAB

/* EEPROM Driver Configuration */
#define WEAR_LEVELING_LOGICAL_SIZE 2048
#define WEAR_LEVELING_BACKING_SIZE (WEAR_LEVELING_LOGICAL_SIZE * 2)

/* RGB Matrix driver configuration */
#define DRIVER_COUNT 2

#define DRIVER_ADDR_1 0b1110111
#define DRIVER_ADDR_2 0b1110100
#define DRIVER_1_LED_TOTAL 30
#define DRIVER_2_LED_TOTAL 38
#define RGB_MATRIX_LED_COUNT (DRIVER_1_LED_TOTAL + DRIVER_2_LED_TOTAL)

/* Set to infinit, which is use in USB mode by default */
#define RGB_MATRIX_TIMEOUT RGB_MATRIX_TIME_INFINITE
/* Allow shutdown of led driver to save power */
#define RGB_MATRIX_DRIVER_SHUTDOWN_ENABLE
/* Turn off backlight on low brightness to save power */
#define RGB_MATRIX_BRIGHTNESS_TURN_OFF_VAL 32

#define VIA_QMK_RGBLIGHT_ENABLE

/* RGB Matrix Animation modes. Explicitly enabled
 * For full list of effects, see:
 * https://docs.qmk.fm/#/feature_rgb_matrix?id=rgb-matrix-effects
 */

#define RGB_MATRIX_KEYPRESSES
#define RGB_MATRIX_FRAMEBUFFER_EFFECTS

// #    define ENABLE_RGB_MATRIX_ALPHAS_MODS
// #    define ENABLE_RGB_MATRIX_GRADIENT_UP_DOWN
// #    define ENABLE_RGB_MATRIX_GRADIENT_LEFT_RIGHT
#define ENABLE_RGB_MATRIX_BREATHING
// #    define ENABLE_RGB_MATRIX_BAND_SAT
// #    define ENABLE_RGB_MATRIX_BAND_VAL
// #    define ENABLE_RGB_MATRIX_BAND_PINWHEEL_SAT
// #    define ENABLE_RGB_MATRIX_BAND_PINWHEEL_VAL
// #    define ENABLE_RGB_MATRIX_BAND_SPIRAL_SAT
#define ENABLE_RGB_MATRIX_BAND_SPIRAL_VAL
#define ENABLE_RGB_MATRIX_CYCLE_ALL
#define ENABLE_RGB_MATRIX_CYCLE_LEFT_RIGHT
#define ENABLE_RGB_MATRIX_CYCLE_UP_DOWN
#define ENABLE_RGB_MATRIX_RAINBOW_MOVING_CHEVRON
#define ENABLE_RGB_MATRIX_CYCLE_OUT_IN
#define ENABLE_RGB_MATRIX_CYCLE_OUT_IN_DUAL
#define ENABLE_RGB_MATRIX_CYCLE_PINWHEEL
#define ENABLE_RGB_MATRIX_CYCLE_SPIRAL
#define ENABLE_RGB_MATRIX_DUAL_BEACON
#define ENABLE_RGB_MATRIX_RAINBOW_BEACON
// #    define ENABLE_RGB_MATRIX_RAINBOW_PINWHEELS
// #    define ENABLE_RGB_MATRIX_RAINDROPS
#define ENABLE_RGB_MATRIX_JELLYBEAN_RAINDROPS
// #    define ENABLE_RGB_MATRIX_HUE_BREATHING
// #    define ENABLE_RGB_MATRIX_HUE_PENDULUM
// #    define ENABLE_RGB_MATRIX_HUE_WAVE
#define ENABLE_RGB_MATRIX_PIXEL_RAIN
// #    define ENABLE_RGB_MATRIX_PIXEL_FLOW
// #    define ENABLE_RGB_MATRIX_PIXEL_FRACTAL

/* enabled only if RGB_MATRIX_FRAMEBUFFER_EFFECTS is defined */
#define ENABLE_RGB_MATRIX_TYPING_HEATMAP
#define ENABLE_RGB_MATRIX_DIGITAL_RAIN

/*  enabled only of RGB_MATRIX_KEYPRESSES or RGB_MATRIX_KEYRELEASES is defined*/
#define ENABLE_RGB_MATRIX_SOLID_REACTIVE_SIMPLE
// #    define ENABLE_RGB_MATRIX_SOLID_REACTIVE
// #    define ENABLE_RGB_MATRIX_SOLID_REACTIVE_WIDE
#define ENABLE_RGB_MATRIX_SOLID_REACTIVE_MULTIWIDE
// #    define ENABLE_RGB_MATRIX_SOLID_REACTIVE_CROSS
// #    define ENABLE_RGB_MATRIX_SOLID_REACTIVE_MULTICROSS
// #    define ENABLE_RGB_MATRIX_SOLID_REACTIVE_NEXUS
#define ENABLE_RGB_MATRIX_SOLID_REACTIVE_MULTINEXUS
#define ENABLE_RGB_MATRIX_SPLASH
// #    define ENABLE_RGB_MATRIX_MULTISPLASH
#define ENABLE_RGB_MATRIX_SOLID_SPLASH
// #    define ENABLE_RGB_MATRIX_SOLID_MULTISPLASH

/* Set LED driver current */
#define CKLED2001_CURRENT_TUNE \
  { 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38 }

#define DIM_CAPS_LOCK
#define CAPS_LOCK_INDEX 30

#if __has_include("secure_config.h")
#  include "secure_config.h"
#else
#  define VIA_FIRMWARE_VERSION 0
#  define SECURE_UNLOCK_SEQUENCE \
    {                            \
      {0, 0}, { 2, 13 }          \
    }
#endif
