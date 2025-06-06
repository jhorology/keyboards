/* Copyright 2022 @ Keychron (https://www.keychron.com)
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

#include "q60_ansi.h"

#ifdef DIP_SWITCH_ENABLE

bool dip_switch_update_kb(uint8_t index, bool active) {
  if (!dip_switch_update_user(index, active)) {
    return false;
  }
  if (index == 0) {
    default_layer_set(1UL << (active ? 1 : 0));
  }
  return true;
}

#endif  // DIP_SWITCH_ENABLE

#ifdef RGB_MATRIX_ENABLE

// clang-format off

const snled27351_led_t PROGMEM g_snled27351_leds[SNLED27351_LED_COUNT] = {
/* Refer to SNLED27351 manual for these locations
 *  driver
 *  |  R location
 *  |  |       G location
 *  |  |       |       B location
 *  |  |       |       | */
    {0, CB6_CA1,    CB4_CA1,    CB5_CA1},
    {0, CB6_CA2,    CB4_CA2,    CB5_CA2},
    {0, CB6_CA3,    CB4_CA3,    CB5_CA3},
    {0, CB6_CA4,    CB4_CA4,    CB5_CA4},
    {0, CB6_CA5,    CB4_CA5,    CB5_CA5},
    {0, CB6_CA6,    CB4_CA6,    CB5_CA6},
    {0, CB6_CA7,    CB4_CA7,    CB5_CA7},
    {0, CB6_CA8,    CB4_CA8,    CB5_CA8},
    {0, CB6_CA9,    CB4_CA9,    CB5_CA9},
    {0, CB6_CA10,   CB4_CA10,   CB5_CA10},
    {0, CB6_CA11,   CB4_CA11,   CB5_CA11},
    {0, CB6_CA12,   CB4_CA12,   CB5_CA12},
    {0, CB6_CA13,   CB4_CA13,   CB5_CA13},
    {0, CB6_CA14,   CB4_CA14,   CB5_CA14},
    {0, CB6_CA15,   CB4_CA15,   CB5_CA15},

    {0, CB9_CA1,    CB7_CA1,    CB8_CA1},
    {0, CB9_CA2,    CB7_CA2,    CB8_CA2},
    {0, CB9_CA3,    CB7_CA3,    CB8_CA3},
    {0, CB9_CA4,    CB7_CA4,    CB8_CA4},
    {0, CB9_CA5,    CB7_CA5,    CB8_CA5},
    {0, CB9_CA6,    CB7_CA6,    CB8_CA6},
    {0, CB9_CA7,    CB7_CA7,    CB8_CA7},
    {0, CB9_CA8,    CB7_CA8,    CB8_CA8},
    {0, CB9_CA9,    CB7_CA9,    CB8_CA9},
    {0, CB9_CA10,   CB7_CA10,   CB8_CA10},
    {0, CB9_CA11,   CB7_CA11,   CB8_CA11},
    {0, CB9_CA12,   CB7_CA12,   CB8_CA12},
    {0, CB9_CA13,   CB7_CA13,   CB8_CA13},
    {0, CB9_CA14,   CB7_CA14,   CB8_CA14},

    {0, CB12_CA1,   CB10_CA1,   CB11_CA1},
    {0, CB12_CA2,   CB10_CA2,   CB11_CA2},
    {0, CB12_CA3,   CB10_CA3,   CB11_CA3},
    {0, CB12_CA4,   CB10_CA4,   CB11_CA4},
    {0, CB12_CA5,   CB10_CA5,   CB11_CA5},
    {0, CB12_CA6,   CB10_CA6,   CB11_CA6},
    {0, CB12_CA7,   CB10_CA7,   CB11_CA7},
    {0, CB12_CA8,   CB10_CA8,   CB11_CA8},
    {0, CB12_CA9,   CB10_CA9,   CB11_CA9},
    {0, CB12_CA10,  CB10_CA10,  CB11_CA10},
    {0, CB12_CA11,  CB10_CA11,  CB11_CA11},
    {0, CB12_CA12,  CB10_CA12,  CB11_CA12},
    {0, CB12_CA14,  CB10_CA14,  CB11_CA14},

    {0, CB3_CA1,    CB1_CA1,    CB2_CA1},
    {0, CB3_CA3,    CB1_CA3,    CB2_CA3},
    {0, CB3_CA4,    CB1_CA4,    CB2_CA4},
    {0, CB3_CA5,    CB1_CA5,    CB2_CA5},
    {0, CB3_CA6,    CB1_CA6,    CB2_CA6},
    {0, CB3_CA7,    CB1_CA7,    CB2_CA7},
    {0, CB3_CA8,    CB1_CA8,    CB2_CA8},
    {0, CB3_CA9,    CB1_CA9,    CB2_CA9},
    {0, CB3_CA10,   CB1_CA10,   CB2_CA10},
    {0, CB3_CA11,   CB1_CA11,   CB2_CA11},
    {0, CB3_CA12,   CB1_CA12,   CB2_CA12},
    {0, CB3_CA13,   CB1_CA13,   CB2_CA13},
    {0, CB3_CA14,   CB1_CA14,   CB2_CA14},

    {0, CB3_CA15,   CB1_CA15,   CB2_CA15},
    {0, CB3_CA16,   CB1_CA16,   CB2_CA16},
    {0, CB12_CA16,  CB10_CA16,  CB11_CA16},
    {0, CB9_CA15,   CB7_CA15,   CB8_CA15},
    {0, CB9_CA16,   CB7_CA16,   CB8_CA16},
};

#define __ NO_LED

led_config_t g_led_config = {
     {
         // Key Matrix to LED Index
        {  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13 },
        { 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28 },
        { 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, __, 41 },
        { 42, __, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54 },
        { 55, 56, __, __, __, __, 57, __, __, __, 58, 59, 14, __ },
    },
    {
        // LED Index to Physical Position
        {0,0},   {16,0},  {32,0},  {48,0},  {64,0},  {80,0},  {96,0},   {112,0},  {128,0},  {144,0},  {160,0},  {176,0},  {192,0},  {208,0},  {224,0},
        {4,16},  {24,16}, {40,16}, {56,16}, {72,16}, {88,16}, {104,16}, {120,16}, {136,16}, {152,16}, {168,16}, {184,16}, {200,16},           {220,16},
        {6,32},  {28,32}, {44,32}, {60,32}, {76,32}, {92,32}, {108,32}, {124,32}, {140,32}, {156,32}, {172,32}, {188,32},                     {214,32},
        {10,48},          {36,48}, {52,48}, {68,48}, {84,48}, {100,48}, {116,48}, {132,48}, {148,48}, {164,48}, {180,48},           {202,48}, {224,48},
                          {24,64}, {44,64},                             {112,64},                               {180,64}, {200,64},
    },
    {
        // RGB LED Index to Flag
        1, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 1, 1,
        1, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,    1,
        8, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,       1,
        1,    4, 4, 4, 4, 4, 4, 4, 4, 4, 4,    1, 1,
              1, 1,          4,          1, 1,
    }
};

#endif // RGB_MATRIX_ENABLE
