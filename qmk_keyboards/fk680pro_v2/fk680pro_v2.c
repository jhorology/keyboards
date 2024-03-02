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

static void process_rgb_enable_changed(void);

user_config_t g_user_config;

//  qmk/vial/my_keyboard custom hook functsions
//------------------------------------------

void board_init(void) {
  // JTAG-DP Disabled and SW-DP Enabled
  // TIM2 remap PA15, PB3, PA2, PA3
  // FK680ProV2 use PA15 as PWM for WS2812 LEDs
  AFIO->MAPR = (AFIO->MAPR & ~(AFIO_MAPR_SWJ_CFG | AFIO_MAPR_TIM2_REMAP)) |
               AFIO_MAPR_SWJ_CFG_JTAGDISABLE | AFIO_MAPR_TIM2_REMAP_0;
}

void eeconfig_init_user(void) {
  // qmk user/kb scope
  //   *_kb scope -> my keyboards common scope
  //   *_user scope ->keyboard-specific scope
  g_user_config.raw = 0;
  g_user_config.keylight_enable = true;
  g_user_config.underglow_enable = true;
  eeconfig_update_user(g_user_config.raw);
}

void keyboard_post_init_user(void) {
  // qmk user/kb scope
  //   *_kb scope -> my keyboards common scope
  //   *_user scope ->keyboard-specific scope
  g_user_config.raw = eeconfig_read_user();
  process_rgb_enable_changed();
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  switch (keycode) {
    case RGB_TOGX:
      if (record->event.pressed) {
        if (get_mods() & MOD_MASK_SHIFT) {
          set_underglow_enable(!g_user_config.underglow_enable);
        } else {
          set_keylight_enable(!g_user_config.keylight_enable);
        }
        return false;
      }
  }
  return true;
}

//  global functions
//------------------------------------------

void set_keylight_enable(bool enable) {
  if (g_user_config.keylight_enable != enable) {
    g_user_config.keylight_enable = enable;
    process_rgb_enable_changed();
    eeconfig_update_user(g_user_config.raw);
  }
}

void set_underglow_enable(bool enable) {
  if (g_user_config.underglow_enable != enable) {
    g_user_config.underglow_enable = enable;
    process_rgb_enable_changed();
    eeconfig_update_user(g_user_config.raw);
  }
}

//  local functions
//------------------------------------------

static void process_rgb_enable_changed() {
  rgb_matrix_set_flags((g_user_config.keylight_enable ? LED_FLAG_KEYLIGHT : 0) |
                       (g_user_config.underglow_enable ? LED_FLAG_UNDERGLOW : 0));
  if (!g_user_config.keylight_enable || !g_user_config.keylight_enable) {
    rgb_matrix_set_color_all(0, 0, 0);
  }
  if (g_user_config.keylight_enable || g_user_config.underglow_enable) {
    rgb_matrix_enable_noeeprom();
  }
  if (!g_user_config.keylight_enable && !g_user_config.underglow_enable) {
    rgb_matrix_disable_noeeprom();
  }
}
