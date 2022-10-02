/* Copyright 2022 zhaqian
 * Modified 2022 masafumi
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

#ifndef RADIAL_CONTROLLER_DIAL_MODE_DEFAULT
#  define RADIAL_CONTROLLER_DIAL_MODE_DEFAULT ENCODER
#endif
#ifndef RADIAL_CONTROLLER_DEGREE_PER_CLICK_DEFAULT
#  define RADIAL_CONTROLLER_DEGREE_PER_CLICK_DEFAULT 10
#endif
#ifndef RADIAL_CONTROLLER_DEGREE_PER_SEC_DEFAULT
#  define RADIAL_CONTROLLER_DEGREE_PER_SEC_DEFAULT 90
#endif
#ifndef RADIAL_CONTROLLER_REPORT_INTERVAL_MILLIS
#  define RADIAL_CONTROLLER_REPORT_INTERVAL_MILLIS 100
#endif

// radial controller dial mode
typedef enum { ENCODER, KEYSWITCH } rc_dial_mode_t;

bool process_radial_controller(uint16_t keycode, keyrecord_t *record);