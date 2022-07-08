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

*/
#ifdef APPLE_FN_ENABLE
#define VENDOR_ID 0x05AC
#else
#define VENDOR_ID 0x5052
#endif
#define MANUFACTURER PrimeKB

/* key matrix size */
#define MATRIX_ROWS 4
#define MATRIX_COLS 13

/* Keyboard Matrix Assignments */
#define MATRIX_ROW_PINS \
  { E6, C7, B5, B4 }
#define MATRIX_COL_PINS \
  { F0, F1, F4, F5, F6, F7, D6, D4, D5, D3, D2, D1, D0 }

/* COL2ROW, ROW2COL */
#define DIODE_DIRECTION COL2ROW

/* Debounce reduces chatter (unintended double-presses) - set 0 if debouncing is not needed */
#define DEBOUNCE 5

/* Mechanical locking support. Use KC_LCAP, KC_LNUM or KC_LSCR instead in keymap */
#define LOCKING_SUPPORT_ENABLE
/* Locking resynchronize hack */
#define LOCKING_RESYNC_ENABLE

#define DYNAMIC_KEYMAP_LAYER_COUNT 4
