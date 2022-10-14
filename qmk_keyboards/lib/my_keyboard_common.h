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

#include "lib/custom_keycodes.h"
#include "quantum.h"

// tap dance actions
enum tap_dance_action_index {
  // for HHKB Right Alt, Alt + layer switch
  TD_RALT_MO3,

  // for HHKB Left Alt, Alt + Apple fn + IME switch
  TD_LALT_APFF_EISU_KANA,

  // Apple Fn key + IME switch
  TD_APFF_EISU_KANA,  // Apple Fn/Globe + FK override, on double tap: toggle send 英数/かな

  // for debugging
  TD_TEST,

  TAP_DANCE_PRE_DEFINED_LENGTH
};