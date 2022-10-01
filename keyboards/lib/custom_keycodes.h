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
#ifdef VIAL_ENABLE
#  include "vial.h"
#endif

//  constants
//------------------------------------------

enum custom_user_keycodes {
  MAC_TOGG = USER00,  // Toggle enabling fake apple mode with switching base layer 0(apple mode) or 1.
  MAC_ON,             // Enable fake apple mode with switching base layer 0.
  MAC_OFF,            // Disable fake apple mode with switching base layer 1.
  APPLE_FN,           // Apple fn/globe key
  APPLE_FF,           // Apple fn/globe with remapping F1-12
  EJ_TOGG,            // Toggle send 英数 and かな
  USJ_TOGG,           // T oggle enabling conversion for ANSI layout on JIS
  USJ_ON,             // Enable conversion for ANSI layout on JIS.
  USJ_OFF,            // Disable conversion for ANSI layout on JIS.
  RADIAL_BUTTON,      // State of the button located on radial controller
  RADIAL_LEFT,        // Relative counter clock wise rotation of the radial controller
  RADIAL_RIGHT,       // Relative clock wise rotation of the radial controller
  CUSTOM_KEYCODES_SAFE_RANGE
};

#define DIAL_BUT RADIAL_BUTTON
#define DIAL_L RADIAL_LEFT
#define DIAL_R RADIAL_RIGHT
