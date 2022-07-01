/* Copyright 2021 trankimtung
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

/* USB Device descriptor parameter */
/*
  Enable Apple Fn

  see https://gist.github.com/fauxpark/010dcf5d6377c3a71ac98ce37414c6c4

  Product Ids
  0201  USB Keyboard [Alps or Logitech, M2452]
  0202  Keyboard [ALPS]
  0205  Extended Keyboard [Mitsumi]
  0206  Extended Keyboard [Mitsumi]
  020b  Pro Keyboard [Mitsumi, A1048/US layout]
  020c  Extended Keyboard [Mitsumi]
  020d  Pro Keyboard [Mitsumi, A1048/JIS layout]
  020e  Internal Keyboard/Trackpad (ANSI)
  020f  Internal Keyboard/Trackpad (ISO)
  0214  Internal Keyboard/Trackpad (ANSI)
  0215  Internal Keyboard/Trackpad (ISO)
  0216  Internal Keyboard/Trackpad (JIS)
  0217  Internal Keyboard/Trackpad (ANSI)                   --> * Apple Fn doesn't work
  0218  Internal Keyboard/Trackpad (ISO)
  0219  Internal Keyboard/Trackpad (JIS)
  021a  Internal Keyboard/Trackpad (ANSI)                   --> * Apple Fn doesn't work
  021b  Internal Keyboard/Trackpad (ISO)
  021c  Internal Keyboard/Trackpad (JIS)
  021d  Aluminum Mini Keyboard (ANSI)                       --> for 60% keyboards, bakeneko60 / Ciel60 / D60
  021e  Aluminum Mini Keyboard (ISO)
  021f  Aluminum Mini Keyboard (JIS)
  0220  Aluminum Keyboard (ANSI)                            --> for 40% or alice, Prime_E
  0221  Aluminum Keyboard (ISO)
  0222  Aluminum Keyboard (JIS)
  0223  Internal Keyboard/Trackpad (ANSI)                   ---> Reserved
  0224  Internal Keyboard/Trackpad (ISO)
  0225  Internal Keyboard/Trackpad (JIS)
  0229  Internal Keyboard/Trackpad (ANSI)                   ---> Reserved
  022a  Internal Keyboard/Trackpad (MacBook Pro) (ISO)
  022b  Internal Keyboard/Trackpad (MacBook Pro) (JIS)
  0230  Internal Keyboard/Trackpad (MacBook Pro 4,1) (ANSI) ---> * unstable, VIAL doesn't work
  0231  Internal Keyboard/Trackpad (MacBook Pro 4,1) (ISO)
  0232  Internal Keyboard/Trackpad (MacBook Pro 4,1) (JIS)
  0236  Internal Keyboard/Trackpad (ANSI)                   ---> * unstable, VIAL doesn't work
  0237  Internal Keyboard/Trackpad (ISO)
  0238  Internal Keyboard/Trackpad (JIS)
  023f  Internal Keyboard/Trackpad (ANSI)                   ---> * unstable, VIAL doesn't work
  0240  Internal Keyboard/Trackpad (ISO)
  0241  Internal Keyboard/Trackpad (JIS)
  0242  Internal Keyboard/Trackpad (ANSI)                   ---> * unstable, VIAL doesn't work
  0243  Internal Keyboard/Trackpad (ISO)
  0244  Internal Keyboard/Trackpad (JIS)
  0245  Internal Keyboard/Trackpad (ANSI)                   ---> * unstable, VAIL dosen't work
  0246  Internal Keyboard/Trackpad (ISO)
  0247  Internal Keyboard/Trackpad (JIS)
  024a  Internal Keyboard/Trackpad (MacBook Air) (ISO)
  024d  Internal Keyboard/Trackpad (MacBook Air) (ISO)
  024f  Aluminium Keyboard (ANSI)                           ---> for 65% keyboards, QK65 / Keychron K7
  0250  Aluminium Keyboard (ISO)
  0252  Internal Keyboard/Trackpad (ANSI)
  0253  Internal Keyboard/Trackpad (ISO)
  0254  Internal Keyboard/Trackpad (JIS)
  0259  Internal Keyboard/Trackpad
  025a  Internal Keyboard/Trackpad
  0263  Apple Internal Keyboard / Trackpad (MacBook Retina)
  0267  Magic Keyboard A1644                                ---> that I own
  0269  Magic Mouse 2 (Lightning connector)
  026c  Magic Keyboard with Numeric Keypad (ANSI)           ---> that I own
  0273  Internal Keyboard/Trackpad (ISO)
  029a  Magic Keyboard with Touch ID (ANSI)                 ---> that I own
*/
#ifdef APPLE_FN_ENABLE
  #define VENDOR_ID    0x05AC
  #define PRODUCT_ID   0x021D
#else
  #define VENDOR_ID    0x445A
  #define PRODUCT_ID   0x1321
#endif
#define DEVICE_VER     0x0003
#define MANUFACTURER   DZTECH
#define PRODUCT        DZ60RGB_WKL

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
#define MATRIX_ROW_PINS { F5, F4, F1, B3, B2 }
#define MATRIX_COL_PINS { C7, F7, F6, F0, B0, B1, B4, D7, D6, D4, D5, D3, D2, B7 }
#define UNUSED_PINS

/* COL2ROW, ROW2COL*/
#define DIODE_DIRECTION COL2ROW

/* Debounce reduces chatter (unintended double-presses) - set 0 if debouncing is not needed */
#define DEBOUNCE 3

#ifdef RGB_MATRIX_ENABLE
#    define RGB_DISABLE_AFTER_TIMEOUT 0 // number of ticks to wait until disabling effects
#    define RGB_DISABLE_WHEN_USB_SUSPENDED  // turn off effects when suspended
// #    define RGB_MATRIX_KEYPRESSES
// #    define RGB_MATRIX_FRAMEBUFFER_EFFECTS
// RGB Matrix Animation modes. Explicitly enabled
// For full list of effects, see:
// https://docs.qmk.fm/#/feature_rgb_matrix?id=rgb-matrix-effects
#    define ENABLE_RGB_MATRIX_ALPHAS_MODS
// #    define ENABLE_RGB_MATRIX_GRADIENT_UP_DOWN
// #    define ENABLE_RGB_MATRIX_GRADIENT_LEFT_RIGHT
// #    define ENABLE_RGB_MATRIX_BREATHING
// #    define ENABLE_RGB_MATRIX_BAND_SAT
// #    define ENABLE_RGB_MATRIX_BAND_VAL
// #    define ENABLE_RGB_MATRIX_BAND_PINWHEEL_SAT
// #    define ENABLE_RGB_MATRIX_BAND_PINWHEEL_VAL
// #    define ENABLE_RGB_MATRIX_BAND_SPIRAL_SAT
// #    define ENABLE_RGB_MATRIX_BAND_SPIRAL_VAL
// #    define ENABLE_RGB_MATRIX_CYCLE_ALL
// #    define ENABLE_RGB_MATRIX_CYCLE_LEFT_RIGHT
// #    define ENABLE_RGB_MATRIX_CYCLE_UP_DOWN
// #    define ENABLE_RGB_MATRIX_RAINBOW_MOVING_CHEVRON
// #    define ENABLE_RGB_MATRIX_CYCLE_OUT_IN
// #    define ENABLE_RGB_MATRIX_CYCLE_OUT_IN_DUAL
// #    define ENABLE_RGB_MATRIX_CYCLE_PINWHEEL
// #    define ENABLE_RGB_MATRIX_CYCLE_SPIRAL
// #    define ENABLE_RGB_MATRIX_DUAL_BEACON
// #    define ENABLE_RGB_MATRIX_RAINBOW_BEACON
// #    define ENABLE_RGB_MATRIX_RAINBOW_PINWHEELS
// #    define ENABLE_RGB_MATRIX_RAINDROPS
// #    define ENABLE_RGB_MATRIX_JELLYBEAN_RAINDROPS
// #    define ENABLE_RGB_MATRIX_HUE_BREATHING
// #    define ENABLE_RGB_MATRIX_HUE_PENDULUM
// #    define ENABLE_RGB_MATRIX_HUE_WAVE
// #    define ENABLE_RGB_MATRIX_PIXEL_RAIN
// #    define ENABLE_RGB_MATRIX_PIXEL_FLOW
// #    define ENABLE_RGB_MATRIX_PIXEL_FRACTAL
// enabled only if RGB_MATRIX_FRAMEBUFFER_EFFECTS is defined
// #    define ENABLE_RGB_MATRIX_TYPING_HEATMAP
// #    define ENABLE_RGB_MATRIX_DIGITAL_RAIN
// enabled only of RGB_MATRIX_KEYPRESSES or RGB_MATRIX_KEYRELEASES is defined
// #    define ENABLE_RGB_MATRIX_SOLID_REACTIVE_SIMPLE
// #    define ENABLE_RGB_MATRIX_SOLID_REACTIVE
// #    define ENABLE_RGB_MATRIX_SOLID_REACTIVE_WIDE
// #    define ENABLE_RGB_MATRIX_SOLID_REACTIVE_MULTIWIDE
// #    define ENABLE_RGB_MATRIX_SOLID_REACTIVE_CROSS
// #    define ENABLE_RGB_MATRIX_SOLID_REACTIVE_MULTICROSS
// #    define ENABLE_RGB_MATRIX_SOLID_REACTIVE_NEXUS
// #    define ENABLE_RGB_MATRIX_SOLID_REACTIVE_MULTINEXUS
// #    define ENABLE_RGB_MATRIX_SPLASH
// #    define ENABLE_RGB_MATRIX_MULTISPLASH
#    define ENABLE_RGB_MATRIX_SOLID_SPLASH
// #    define ENABLE_RGB_MATRIX_SOLID_MULTISPLASH

#    define DRIVER_ADDR_1      0b1010000
#    define DRIVER_COUNT       1
#    define DRIVER_LED_TOTAL   62
#endif
