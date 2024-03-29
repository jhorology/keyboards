/*
Copyright 2022 Qwertykeys

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

#define LED_CAPS_LOCK_PIN A0
#define LED_PIN_ON_STATE 0

/* VIA Layout Options */
// bit 4
#define LAYOUT_OPTION_SPLIT_BS 0x10
// bit 2-3
#define LAYOUT_OPTION_BOTTOM_ROW_TSANGAN 0x0
#define LAYOUT_OPTION_BOTTOM_ROW_HHKB 0x4
#define LAYOUT_OPTION_BOTTOM_ROW_WKL 0x8
// bit 0-1
#define LAYOUT_OPTION_SPACEBAR_7U 0x0
#define LAYOUT_OPTION_SPACEBAR_2_25U 0x1
#define LAYOUT_OPTION_SPACEBAR_2_75U 0x2

#if __has_include("secure_config.h")
#  include "secure_config.h"
#else
#  define VIA_FIRMWARE_VERSION 0
#  define SECURE_UNLOCK_SEQUENCE \
    {                            \
      {0, 0}, { 2, 12 }          \
    }
#endif
