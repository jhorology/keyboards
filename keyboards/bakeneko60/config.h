/*
Copyright 2020 Koichi Katano

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "config_common.h"

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
	0217  Internal Keyboard/Trackpad (ANSI)
	0218  Internal Keyboard/Trackpad (ISO)
	0219  Internal Keyboard/Trackpad (JIS)
	021a  Internal Keyboard/Trackpad (ANSI)
	021b  Internal Keyboard/Trackpad (ISO)
	021c  Internal Keyboard/Trackpad (JIS)
	021d  Aluminum Mini Keyboard (ANSI)
	021e  Aluminum Mini Keyboard (ISO)
	021f  Aluminum Mini Keyboard (JIS)
	0220  Aluminum Keyboard (ANSI)
	0221  Aluminum Keyboard (ISO)
	0222  Aluminum Keyboard (JIS)
	0223  Internal Keyboard/Trackpad (ANSI)
	0224  Internal Keyboard/Trackpad (ISO)
	0225  Internal Keyboard/Trackpad (JIS)
	0229  Internal Keyboard/Trackpad (ANSI)
	022a  Internal Keyboard/Trackpad (MacBook Pro) (ISO)
	022b  Internal Keyboard/Trackpad (MacBook Pro) (JIS)
	0230  Internal Keyboard/Trackpad (MacBook Pro 4,1) (ANSI)
	0231  Internal Keyboard/Trackpad (MacBook Pro 4,1) (ISO)
	0232  Internal Keyboard/Trackpad (MacBook Pro 4,1) (JIS)
	0236  Internal Keyboard/Trackpad (ANSI)
	0237  Internal Keyboard/Trackpad (ISO)
	0238  Internal Keyboard/Trackpad (JIS)
	023f  Internal Keyboard/Trackpad (ANSI)
	0240  Internal Keyboard/Trackpad (ISO)
	0241  Internal Keyboard/Trackpad (JIS)
	0242  Internal Keyboard/Trackpad (ANSI)
	0243  Internal Keyboard/Trackpad (ISO)
	0244  Internal Keyboard/Trackpad (JIS)
	0245  Internal Keyboard/Trackpad (ANSI)
	0246  Internal Keyboard/Trackpad (ISO)
	0247  Internal Keyboard/Trackpad (JIS)
	024a  Internal Keyboard/Trackpad (MacBook Air) (ISO)
	024d  Internal Keyboard/Trackpad (MacBook Air) (ISO)
	024f  Aluminium Keyboard (ANSI)
	0250  Aluminium Keyboard (ISO)
	0252  Internal Keyboard/Trackpad (ANSI)
	0253  Internal Keyboard/Trackpad (ISO)
	0254  Internal Keyboard/Trackpad (JIS)
	0259  Internal Keyboard/Trackpad
	025a  Internal Keyboard/Trackpad
	0263  Apple Internal Keyboard / Trackpad (MacBook Retina)
	0267  Magic Keyboard A1644
	0269  Magic Mouse 2 (Lightning connector)
	0273  Internal Keyboard/Trackpad (ISO)
*/
#ifdef APPLE_FN_ENABLE
  #define VENDOR_ID    0x05AC
  #define PRODUCT_ID   0x0267
#else
  #define VENDOR_ID    0x3A0E
  #define PRODUCT_ID   0xCBDC
#endif
#define DEVICE_VER   0x0001
#define MANUFACTURER kkatano
#define PRODUCT      Bakeneko 60

/* key matrix size */
#define MATRIX_ROWS 5
#define MATRIX_COLS 15

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
#define MATRIX_ROW_PINS { E6, B7, F7, F4, F5 }
#define MATRIX_COL_PINS { F6, B0, F1, C7, C6, B6, B5, B4, D7, D6, D4, D5, D3, D2, D1 }
#define UNUSED_PINS

/* COL2ROW, ROW2COL */
#define DIODE_DIRECTION COL2ROW

/* Debounce reduces chatter (unintended double-presses) - set 0 if debouncing is not needed */
#define DEBOUNCE 5

/* define if matrix has ghost (lacks anti-ghosting diodes) */
//#define MATRIX_HAS_GHOST

/* Mechanical locking support. Use KC_LCAP, KC_LNUM or KC_LSCR instead in keymap */
#define LOCKING_SUPPORT_ENABLE
/* Locking resynchronize hack */
#define LOCKING_RESYNC_ENABLE

/* If defined, GRAVE_ESC will always act as ESC when CTRL is held.
 * This is useful for the Windows task manager shortcut (ctrl+shift+esc).
 */
//#define GRAVE_ESC_CTRL_OVERRIDE

/*
 * Force NKRO
 *
 * Force NKRO (nKey Rollover) to be enabled by default, regardless of the saved
 * state in the bootmagic EEPROM settings. (Note that NKRO must be enabled in the
 * makefile for this to work.)
 *
 * If forced on, NKRO can be disabled via magic key (default = LShift+RShift+N)
 * until the next keyboard reset.
 *
 * NKRO may prevent your keystrokes from being detected in the BIOS, but it is
 * fully operational during normal computer usage.
 *
 * For a less heavy-handed approach, enable NKRO via magic key (LShift+RShift+N)
 * or via bootmagic (hold SPACE+N while plugging in the keyboard). Once set by
 * bootmagic, NKRO mode will always be enabled until it is toggled again during a
 * power-up.
 *
 */
//#define FORCE_NKRO

/*
 * Feature disable options
 *  These options are also useful to firmware size reduction.
 */

/* disable debug print */
//#define NO_DEBUG

/* disable print */
//#define NO_PRINT

/* disable action features */
//#define NO_ACTION_LAYER
//#define NO_ACTION_TAPPING
//#define NO_ACTION_ONESHOT

/* disable these deprecated features by default */
#define NO_ACTION_MACRO
#define NO_ACTION_FUNCTION

/* Bootmagic Lite key configuration */
//#define BOOTMAGIC_LITE_ROW 0
//#define BOOTMAGIC_LITE_COLUMN 0
