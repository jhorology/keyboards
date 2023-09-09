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

#include <via.h>

//  constants
//------------------------------------------

enum custom_user_keycodes {
  RHID_ON = QK_KB_0,  // allow accaess to RAW HID
                      // +Shift: same as RHID_OFF
  RHID_OFF,           // deny accaess to RAW HID
                      // +Shift: same as RHID_ON
#ifndef DIP_SWITCH_ENABLE
  MAC_ON,   // Enable true apple mode with switching base layer 0.
            // +Shfit: same as MAC_OFF
  MAC_OFF,  // Disable true apple mode with switching base layer 1.
            // +Shift: same as MAC_ON
#endif
  AUT_ON,          // Enable auto detection of mac mode
                   // +Shift: same as MAC_AUT_OFF
  AUT_OFF,         // Disable auto detection of mac mode.
                   // +Shift: same as MAC_AUT_ON
  USJ_ON,          // Enable key overrides for ANSI layout on JIS.
                   // +Shift: same as USJ_OFF
  USJ_OFF,         // Disable key overrides for ANSI layout on JIS.
                   // +Shift: same as USJ_ON
  APPLE_FN,        // Apple fn/globe key
  APPLE_FF,        // Apple fn/globe with remapping F1-12
  APPLE_FUNCTION,  // for exprimental purpose
  EISU_KANA,       // Toggle send 英数(KC_LNG2) and かな(KC_LNG1)
  TERM_LCK,        // Terminal Lock
#ifdef RADIAL_CONTROLLER_ENABLE
  RC_BTN,   // State of the button located on radial controller
  RC_CCW,   // counter clock wise rotation of the radial controller
  RC_CW,    // clock wise rotation of the radial controller
  RC_FINE,  // Dial rotation speed becomes slow
#endif
#ifdef OS_FINGERPRINT_DEBUG_ENABLE
  TEST_OS,  // SEND_STRING deteced os variant
#endif
  CUSTOM_KEYCODES_SAFE_RANGE
};
