/*
  Copyright 2019 Holten Campbell
  Modified 2922 jhorology

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
#define PRODUCT_ID 0x0220
#else
#define PRODUCT_ID 0x0052
#endif
#define DEVICE_VER 0x0001
#define PRODUCT Prime_E RGB

#define RGB_DI_PIN B7
#define RGBLED_NUM 8

/*
  polling rate
  see https://github.com/qmk/qmk_firmware/blob/master/docs/config_options.md
*/
#define USB_POLLING_INTERVAL_MS 1
#define QMK_KEYS_PER_SCAN 6

/* Vial-specific definitions. */
#ifdef VIAL_ENABLE
#define VIAL_KEYBOARD_UID \
  { 0xF3, 0xB8, 0xA3, 0x29, 0x37, 0x91, 0xFD, 0xA2 }
#define VIAL_UNLOCK_COMBO_ROWS \
  { 0, 1 }
#define VIAL_UNLOCK_COMBO_COLS \
  { 0, 12 }
#endif

/* reducing firmware size */
#ifndef VIAL_ENABLE
#define RGBLIGHT_ANIMATIONS
#endif
