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
#  define VENDOR_ID 0x05AC
// #  define PRODUCT_ID 0x021d  // ANSI
// #  define PRODUCT_ID 0x021e  // ISO
#  define PRODUCT_ID 0x021f  // JIS
#else
#  define VENDOR_ID 0x445A
#  define PRODUCT_ID 0x1321
#endif
#define DEVICE_VER 0x0003
#define MANUFACTURER KBDfans
#define PRODUCT D60 HHKB

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
#define UNUSED_PINS

/* COL2ROW, ROW2COL*/
#define DIODE_DIRECTION COL2ROW

#define RGB_DISABLE_AFTER_TIMEOUT 0     // number of ticks to wait until disabling effects
#define RGB_DISABLE_WHEN_USB_SUSPENDED  // turn off effects when suspended
// reduce firmware size
#define RGB_MATRIX_FRAMEBUFFER_EFFECTS
#define RGB_MATRIX_KEYPRESSES
// #define RGB_MATRIX_KEYRELEASES

// reduce firmware size
#ifndef VIAL_ENABLE
#  define ENABLE_RGB_MATRIX_ALPHAS_MODS             // Static dual hue, speed is hue for secondary hue
#  define ENABLE_RGB_MATRIX_GRADIENT_UP_DOWN        // Static gradient top to bottom, speed controls how much gradient
                                                    // changes
#  define ENABLE_RGB_MATRIX_GRADIENT_LEFT_RIGHT     // Static gradient left to right, speed controls how much gradient
                                                    // changes
#  define ENABLE_RGB_MATRIX_BREATHING               // Single hue brightness cycling animation
#  define ENABLE_RGB_MATRIX_BAND_SAT                // Single hue band fading saturation scrolling left to right
#  define ENABLE_RGB_MATRIX_BAND_VAL                // Single hue band fading brightness scrolling left to right
#  define ENABLE_RGB_MATRIX_BAND_PINWHEEL_SAT       // Single hue 3 blade spinning pinwheel fades saturation
#  define ENABLE_RGB_MATRIX_BAND_PINWHEEL_VAL       // Single hue 3 blade spinning pinwheel fades brightness
#  define ENABLE_RGB_MATRIX_BAND_SPIRAL_SAT         // Single hue spinning spiral fades saturation
#  define ENABLE_RGB_MATRIX_BAND_SPIRAL_VAL         // Single hue spinning spiral fades brightness
#  define ENABLE_RGB_MATRIX_CYCLE_ALL               // Full keyboard solid hue cycling through full gradient
#  define ENABLE_RGB_MATRIX_CYCLE_LEFT_RIGHT        // Full gradient scrolling left to right
#  define ENABLE_RGB_MATRIX_CYCLE_UP_DOWN           // Full gradient scrolling top to bottom
#  define ENABLE_RGB_MATRIX_CYCLE_OUT_IN            // Full gradient scrolling out to in
#  define ENABLE_RGB_MATRIX_CYCLE_OUT_IN_DUAL       // Full dual gradients scrolling out to in
#  define ENABLE_RGB_MATRIX_RAINBOW_MOVING_CHEVRON  // Full gradent Chevron shapped scrolling left to right
#  define ENABLE_RGB_MATRIX_CYCLE_PINWHEEL          // Full gradient spinning pinwheel around center of keyboard
#  define ENABLE_RGB_MATRIX_CYCLE_SPIRAL            // Full gradient spinning spiral around center of keyboard
#  define ENABLE_RGB_MATRIX_DUAL_BEACON             // Full gradient spinning around center of keyboard
#  define ENABLE_RGB_MATRIX_RAINBOW_BEACON          // Full tighter gradient spinning around center of keyboard
#  define ENABLE_RGB_MATRIX_RAINBOW_PINWHEELS       // Full dual gradients spinning two halfs of keyboard
#  define ENABLE_RGB_MATRIX_RAINDROPS               // Randomly changes a single key's hue
#  define ENABLE_RGB_MATRIX_JELLYBEAN_RAINDROPS     // Randomly changes a single key's hue and saturation
#  define ENABLE_RGB_MATRIX_HUE_BREATHING           // Hue shifts up a slight ammount at the same time, then shifts back
#  define ENABLE_RGB_MATRIX_HUE_PENDULUM   // Hue shifts up a slight ammount in a wave to the right, then back to the
                                           // left
#  define ENABLE_RGB_MATRIX_HUE_WAVE       // Hue shifts up a slight ammount and then back down in a wave to the right
#  define ENABLE_RGB_MATRIX_PIXEL_FRACTAL  // Single hue fractal filled keys pulsing horizontally out to edges
#  define ENABLE_RGB_MATRIX_PIXEL_FLOW     // Pulsing RGB flow along LED wiring with random hues
#  define ENABLE_RGB_MATRIX_PIXEL_RAIN     // Randomly light keys with random hues#define ENABLE_RGB_MATRIX_ALPHAS_MODS
#endif

#ifdef RGB_MATRIX_FRAMEBUFFER_EFFECTS
#  define ENABLE_RGB_MATRIX_TYPING_HEATMAP  // How hot is your WPM!
                                            // reduce firmware size
#  ifndef VIAL_ENABLE
#    define ENABLE_RGB_MATRIX_DIGITAL_RAIN  // That famous computer simulation
#  endif
#endif

#if defined(RGB_MATRIX_KEYPRESSES) || defined(RGB_MATRIX_KEYRELEASES)
// reduce firmware size
#  ifndef VIAL_ENABLE
#    define ENABLE_RGB_MATRIX_SOLID_REACTIVE_SIMPLE  // Pulses keys hit to hue & value then fades value out
#    define ENABLE_RGB_MATRIX_SOLID_REACTIVE         // Static single hue, pulses keys hit to shifted hue then fades to
                                                     // current hue
#    define ENABLE_RGB_MATRIX_SOLID_REACTIVE_WIDE    // Hue & value pulse near a single key hit then fades value out
#    define ENABLE_RGB_MATRIX_SOLID_REACTIVE_MULTIWIDE  // Hue & value pulse near multiple key hits then fades value
                                                        // out
#    define ENABLE_RGB_MATRIX_SOLID_REACTIVE_CROSS      // Hue & value pulse the same column and row of a single key hit
                                                        // then fades value out
#    define ENABLE_RGB_MATRIX_SOLID_REACTIVE_MULTICROSS  // Hue & value pulse the same column and row of multiple key
                                                         // hits then fades value out
#    define ENABLE_RGB_MATRIX_SOLID_REACTIVE_NEXUS  // Hue & value pulse away on the same column and row of a single
                                                    // key hit then fades value out
#    define ENABLE_RGB_MATRIX_SOLID_REACTIVE_MULTINEXUS  // Hue & value pulse away on the same column and row of
                                                         // multiple key hits then fades value out
#    define ENABLE_RGB_MATRIX_SPLASH  // Full gradient & value pulse away from a single key hit then fades value out
#    define ENABLE_RGB_MATRIX_MULTISPLASH   // Full gradient & value pulse away from multiple key hits then fades value
                                            // out
#    define ENABLE_RGB_MATRIX_SOLID_SPLASH  // Hue & value pulse away from a single key hit then fades value out
#    define ENABLE_RGB_MATRIX_SOLID_MULTISPLASH  // Hue & value pulse away from multiple key hits then fades value out
#  endif
#endif

#define DRIVER_ADDR_1 0b1010000
#define DRIVER_COUNT 1
#define DRIVER_LED_TOTAL 62

/* Vial-specific definitions. */
#ifdef VIAL_ENABLE
#  define VIAL_KEYBOARD_UID \
    { 0xC3, 0xB7, 0xF6, 0xE8, 0x92, 0xC7, 0x5F, 0x5F }
#  define VIAL_UNLOCK_COMBO_ROWS \
    { 0, 2 }
#  define VIAL_UNLOCK_COMBO_COLS \
    { 0, 13 }
#endif

#ifdef VIA_ENABLE
#  define LAYOUT_OPTION_HHKB 0x0
#  define LAYOUT_OPTION_TSANGAN 0x1
#endif
