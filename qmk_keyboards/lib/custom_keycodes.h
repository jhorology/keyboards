/* Copyright 2022 Msafumi
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

#include "quantum.h"
#include "via.h"

//  constants
//------------------------------------------

enum custom_user_keycodes {
  RHID_TOGG = USER00,  // Toggle allow or deny accaess to RAW HID
  RHID_ON,             // allow accaess to RAW HID
  RHID_OFF,            // deny accaess to RAW HID
#ifndef DIP_SWITCH_ENABLE
  MAC_TOGG,  // Toggle enabling fake apple mode with switching base layer 0(apple mode) or 1.
  MAC_ON,    // Enable true apple mode with switching base layer 0.
  MAC_OFF,   // Disable true apple mode with switching base layer 1.
#endif
  USJ_TOGG,   // Tooggle enabling key overrides for ANSI layout on JIS
  USJ_ON,     // Enable key overrides for ANSI layout on JIS.
  USJ_OFF,    // Disable key overrides for ANSI layout on JIS.
  APPLE_FN,   // Apple fn/globe key
  APPLE_FF,   // Apple fn/globe with remapping F1-12
  EISU_KANA,  // Toggle send 英数(KC_LNG2) and かな(KC_LNG1)
  BB_TOGG,    // Toogle Backslash and Backspace swap
  BB_SWAP,    // Sswap Backslash and Backspace
  BB_NORM,    // Unswap Backslash and Backspace
#ifdef RADIAL_CONTROLLER_ENABLE
  RC_BTN,   // State of the button located on radial controller
  RC_CCW,   // counter clock wise rotation of the radial controller
  RC_CW,    // clock wise rotation of the radial controller
  RC_FINE,  // Dial rotation speed becomes slow
#endif
  CUSTOM_KEYCODES_SAFE_RANGE
};
