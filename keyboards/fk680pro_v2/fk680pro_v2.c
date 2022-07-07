/* Copyright 2022 jhorology
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

#include "fk680pro_v2.h"

#ifdef RGB_MATRIX_ENABLE
#    define xx NO_LED

user_config_t g_user_config;

// clang-format off
led_config_t g_led_config = {
  // Key Matrix to LED Index
  {
    {55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69},
    {40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54},
    {26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, xx, 38, 39},
    {12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, xx, 23, 24, 25},
    { 0,  1,  2, xx, xx,  4, xx, xx, xx,  6,  7,  8,  9, 10, 11}
  },
  // LED Index to Physical Position
  {
    // key matrix south to north
    { 9,61},{26,61},{44,61},     {75,55},   {96,61},     {115,55},             {147,61},{161,61},{175,61},{189,61},{203,61},{217,61},
    {16,48},{38,48},{52,48},{66,48},{80,48},{94,48},{108,48},{122,48},{136,48},{150,48},{164,48},         {184,48},{203,48},{217,48},
    {12,35},{32,35},{46,35},{60,35},{74,35},{88,35},{102,35},{116,35},{130,35},{144,35},{158,35},{172,35},         {194,35},{217,35},
    {10,23},{28,23},{42,23},{56,23},{70,23},{84,23},{ 98,23},{112,23},{126,23},{140,23},{154,23},{168,23},{182,23},{200,23},{217,23},
    { 7,10},{21,10},{35,10},{49,10},{63,10},{77,10},{ 91,10},{105,10},{119,10},{133,10},{147,10},{161,10},{175,10},{196,10},{217,10},

    // underglow
    // east side, north to south
    // LED01   LED02    LED03    LED04    LED05
    {223,12},{223,25},{223,37},{223,50},{223,61},

    // south side, east to west

    //LED06    LED07    LED08    LED09    LED10    LED11    LED12    LED13    LED14   LED15   LED16   LED17    LE18    LE19    LE20
    {210,63},{196,63},{182,63},{168,63},{154,63},{140,63},{126,63},{112,63},{102,63},{90,63},{76,63},{62,63},{49,63},{35,63},{18,63},

    // west side south to north
    // LED21 LED22 LED23 LED24  LED25
    {1,63},{1,52},{1,39},{1,27},{1,14},

    // north side, west to east
    //LED26  LED27   LED28   LED29   LED30   LED31   LED32    LED33    LED34    LED35    LED36    LED37    LED38    LED39    LED40
    {14,12},{28,12},{42,12},{56,12},{70,12},{84,12},{98,12},{112,12},{126,12},{140,12},{154,12},{168,12},{182,12},{202,12},{210,12}
  },
  /*
    LED Index to Flag

    HAS_FLAGS(bits, flags)     n/a  Evaluates to true if bits has all flags set
    HAS_ANY_FLAGS(bits, flags) n/a  Evaluates to true if bits has any flags set
    LED_FLAG_NONE              0x00 If this LED has no flags
    LED_FLAG_ALL               0xFF If this LED has all flags
    LED_FLAG_MODIFIER          0x01 If the LED is on a modifier key
    LED_FLAG_UNDERGLOW         0x02 If the LED is for underglow
    LED_FLAG_KEYLIGHT          0x04 If the LED is for key backlight
    LED_FLAG_INDICATOR         0x08 If the LED is for keyboard state indication
  */
  {
    // key matrix
    // key matrix south to north
    4,4,4, 4, 4, 4,   4,4,4,4,4,4,
    4,4,4,4,4,4,4,4,4,4,4,  4,4,4,
    4,4,4,4,4,4,4,4,4,4,4,4,  4,4,
    4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
    4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,

    // underglow
    // east side, north to south
    2,2,2,2,2,

    // south side, east to west
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,

    // west side south to north
    2,2,2,2,2,

    // north side, west to east
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
  }
};
// clang-format on
#endif

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
#ifdef APPLE_FN_ENABLE
        case APPLE_FN:
            process_apple_fn(keycode, record);
            break;
#endif
#ifdef RGB_MATRIX_ENABLE
        case RGB_CYMD:
            if (record->event.pressed) {
                update_rgb_matrix_flags((g_user_config.rgb_led_mode + 1) & 0x03);
            }
            break;
    }
#endif
    return true;
}

#ifdef RGB_MATRIX_ENABLE
__attribute__((weak)) void board_init(void) {
    // JTAG-DP Disabled and SW-DP Enabled
    // TIM2 remap PA15, PB3, PA2, PA3
    // FK680ProV2 use PA15 as PWM for WS2812 LEDs
    AFIO->MAPR = (AFIO->MAPR & ~(AFIO_MAPR_SWJ_CFG | AFIO_MAPR_TIM2_REMAP)) | AFIO_MAPR_SWJ_CFG_JTAGDISABLE | AFIO_MAPR_TIM2_REMAP_0;
}

__attribute__((weak)) void keyboard_post_init_kb(void) {
    g_user_config.raw = eeconfig_read_user();
    update_rgb_matrix_flags(g_user_config.rgb_led_mode);
}

#    ifdef RGB_DISABLE_WHEN_USB_SUSPENDED
__attribute__((weak)) void suspend_power_down_kb(void) {
    rgb_matrix_set_suspend_state(true);
    suspend_power_down_user();
}

__attribute__((weak)) void suspend_wakeup_init_kb(void) {
    rgb_matrix_set_suspend_state(false);
    suspend_wakeup_init_user();
}
#    endif

void update_rgb_matrix_flags(uint8_t mode) {
    led_flags_t flags = LED_FLAG_NONE;
    flags |= (mode & 0x01) ? LED_FLAG_KEYLIGHT : 0;
    flags |= (mode & 0x02) ? LED_FLAG_UNDERGLOW : 0;
    rgb_matrix_set_flags(flags);
    if (mode != 3) {
        rgb_matrix_set_color_all(0, 0, 0);
    }
    if (mode != g_user_config.rgb_led_mode) {
        g_user_config.rgb_led_mode = mode;
        eeconfig_update_user(g_user_config.raw);
    }
}
#endif
