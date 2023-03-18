/* Copyright 2022 @ Keychron (https://www.keychron.com)
 * Modified 2022 Masafumi
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
#include "k6_pro_ansi_rgb.h"

#include <stdint.h>

#include "bat_level_animation.h"
#include "battery.h"
#include "bluetooth.h"
#include "ckbt51.h"
#include "eeprom.h"
#include "indicator.h"
#include "lpm.h"
#include "raw_hid.h"
#include "transport.h"

// clang-format off
const ckled2001_led g_ckled2001_leds[RGB_MATRIX_LED_COUNT] = {
/* Refer to IS31 manual for these locations
 *   driver
 *   |  R location
 *   |  |       G location
 *   |  |       |       B location
 *   |  |       |       | */
   {0, C_16,    A_16,   B_16},
   {0, C_15,    A_15,   B_15},
   {0, C_14,    A_14,   B_14},
   {0, C_13,    A_13,   B_13},
   {0, C_12,    A_12,   B_12},
   {0, C_11,    A_11,   B_11},
   {0, C_10,    A_10,   B_10},
   {0, C_9,     A_9,    B_9},
   {0, C_8,     A_8,    B_8},
   {0, C_7,     A_7,    B_7},
   {0, C_6,     A_6,    B_6},
   {0, C_5,     A_5,    B_5},
   {0, C_4,     A_4,    B_4},
   {0, C_3,     A_3,    B_3},
   {0, C_2,     A_2,    B_2},

   {0, F_16,    D_16,   E_16},
   {0, F_15,    D_15,   E_15},
   {0, F_14,    D_14,   E_14},
   {0, F_13,    D_13,   E_13},
   {0, F_12,    D_12,   E_12},
   {0, F_11,    D_11,   E_11},
   {0, F_10,    D_10,   E_10},
   {0, F_9,     D_9,    E_9},
   {0, F_8,     D_8,    E_8},
   {0, F_7,     D_7,    E_7},
   {0, F_6,     D_6,    E_6},
   {0, F_5,     D_5,    E_5},
   {0, F_4,     D_4,    E_4},
   {0, F_3,     D_3,    E_3},
   {0, F_2,     D_2,    E_2},

   {1, C_16,    A_16,   B_16},
   {1, C_15,    A_15,   B_15},
   {1, C_14,    A_14,   B_14},
   {1, C_13,    A_13,   B_13},
   {1, C_12,    A_12,   B_12},
   {1, C_11,    A_11,   B_11},
   {1, C_10,    A_10,   B_10},
   {1, C_9,     A_9,    B_9},
   {1, C_8,     A_8,    B_8},
   {1, C_7,     A_7,    B_7},
   {1, C_6,     A_6,    B_6},
   {1, C_5,     A_5,    B_5},
   {1, C_3,     A_3,    B_3},
   {1, C_2,     A_2,    B_2},

   {1, I_16,    G_16,   H_16},
   {1, I_14,    G_14,   H_14},
   {1, I_13,    G_13,   H_13},
   {1, I_12,    G_12,   H_12},
   {1, I_11,    G_11,   H_11},
   {1, I_10,    G_10,   H_10},
   {1, I_9,     G_9,    H_9},
   {1, I_8,     G_8,    H_8},
   {1, I_7,     G_7,    H_7},
   {1, I_6,     G_6,    H_6},
   {1, I_5,     G_5,    H_5},
   {1, I_4,     G_4,    H_4},
   {1, I_3,     G_3,    H_3},
   {1, I_2,     G_2,    H_2},

   {1, F_16,    D_16,   E_16},
   {1, F_15,    D_15,   E_15},
   {1, F_14,    D_14,   E_14},
   {1, F_10,    D_10,   E_10},
   {1, F_7,     D_7,    E_7},
   {1, F_6,     D_6,    E_6},
   {1, F_5,     D_5,    E_5},
   {1, F_4,     D_4,    E_4},
   {1, F_3,     D_3,    E_3},
   {1, F_2,     D_2,    E_2}
};

led_config_t g_led_config = {
    {
        {  0,      1,      2,      3,      4,      5,      6,      7,      8,      9,      10,     11,     12,     13,     14 },
        {  15,     16,     17,     18,     19,     20,     21,     22,     23,     24,     25,     26,     27,     28,     29 },
        {  30,     31,     32,     33,     34,     35,     36,     37,     38,     39,     40,     41,     NO_LED, 42,     43 },
        {  44,     NO_LED, 45,     46,     47,     48,     49,     50,     51,     52,     53,     54,     55,     56,     57 },
        {  58,	   59,     60,     NO_LED, NO_LED, NO_LED, 61,     NO_LED, NO_LED, 62, 	   63,     64,     65,     66,     67 }
    },
    {
        {0, 0}, {14, 0}, {29, 0}, { 44, 0}, { 59, 0}, { 74, 0}, { 89, 0}, {104, 0}, {119, 0}, {134, 0}, {149, 0}, {164, 0}, {179, 0}, {201, 0}, {224, 0},
        {3,16}, {22,16}, {37,16}, { 52,16}, { 67,16}, { 82,16}, { 97,16}, {112,16}, {126,16}, {141,16}, {156,16}, {171,16}, {186,16}, {205,16}, {224,16},
        {5,32}, {26,32}, {41,32}, { 55,32}, { 70,32}, { 85,32}, {100,32}, {115,32}, {130,32}, {145,32}, {160,32}, {175,32}, {199,32}, 			{224,32},
        {9,48}, 		 {33,48}, {48,48},  { 63,48}, { 78,48}, { 93,48}, {108,48}, {123,48}, {138,48}, {153,48}, {167,48}, {188,48}, {209,48}, {224,48},
        {1,64}, {20,64}, {39,64},                               { 95,64},                     {149,64}, {164,64}, {179,64}, {194,64}, {209,64}, {224,64}
    },
    {
        4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,
        4,    8,    8,    8,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,
        4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,          4,    4,
        4,          4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,
        4,    4,    4,                      4,                4,    4,    4,    4,    4,    4,
	}
};
// clang-format on

#define POWER_ON_LED_DURATION 3000

typedef struct PACKED {
  uint8_t len;
  uint8_t keycode[3];
} key_combination_t;

static uint32_t power_on_indicator_timer_buffer;

bool firstDisconnect = true;
bool bt_factory_reset = false;
static virtual_timer_t pairing_key_timer;
extern uint8_t g_pwm_buffer[DRIVER_COUNT][192];

static void pairing_key_timer_cb(void *arg) { bluetooth_pairing_ex(*(uint8_t *)arg, NULL); }

bool dip_switch_update_user(uint8_t index, bool active) {
  if (index == 0) {
    if (!custom_config_auto_detect_is_enable()) {
      custom_config_mac_set_enable(!active);
    }
  }
  return true;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  if (get_transport() != TRANSPORT_BLUETOOTH) return true;

  lpm_timer_reset();

#if defined(BAT_LOW_LED_PIN) || defined(LOW_BAT_IND_INDEX)
  if (battery_is_empty() && bluetooth_get_state() == BLUETOOTH_CONNECTED && record->event.pressed) {
#  if defined(BAT_LOW_LED_PIN)
    indicator_battery_low_enable(true);
#  endif
#  if defined(LOW_BAT_IND_INDEX)
    indicator_battery_low_backlit_enable(true);
#  endif
  }
#endif

  switch (keycode) {
    case BT_HST1 ... BT_HST3:
      if (record->event.pressed) {
        uint8_t host_idx = keycode - BT_HST1 + 1;
        chVTSet(&pairing_key_timer, TIME_MS2I(2000), (vtfunc_t)pairing_key_timer_cb, &host_idx);
        bluetooth_connect_ex(host_idx, 0);
      } else {
        chVTReset(&pairing_key_timer);
      }
      return false;
    case BAT_LVL:
      if (record->event.pressed && !usb_power_connected()) {
        bat_level_animiation_start(battery_get_percentage());
      }
      return false;
  }
  return true;
}

void keyboard_post_init_user(void) {
  /* Currently we don't use this reset pin */
  palSetLineMode(CKBT51_RESET_PIN, PAL_MODE_UNCONNECTED);

  /* IMPORTANT: DO NOT enable internal pull-up resistor
   * as there is an external pull-down resistor.
   */
  palSetLineMode(USB_BT_MODE_SELECT_PIN, PAL_MODE_INPUT);

  ckbt51_init(false);
  bluetooth_init();

  power_on_indicator_timer_buffer = sync_timer_read32() | 1;
  writePin(BAT_LOW_LED_PIN, BAT_LOW_LED_PIN_ON_STATE);
}

void matrix_scan_user(void) {
  if (power_on_indicator_timer_buffer) {
    if (sync_timer_elapsed32(power_on_indicator_timer_buffer) > POWER_ON_LED_DURATION) {
      power_on_indicator_timer_buffer = 0;
      writePin(BAT_LOW_LED_PIN, !BAT_LOW_LED_PIN_ON_STATE);
    } else {
      writePin(BAT_LOW_LED_PIN, BAT_LOW_LED_PIN_ON_STATE);
    }
  }
}

static void ckbt51_param_init(void) {
  /* Set bluetooth device name */
  ckbt51_set_local_name(PRODUCT);
  /* Set bluetooth parameters */
  module_param_t param = {.event_mode = 0x02,
                          .connected_idle_timeout = 7200,
                          .pairing_timeout = 180,
                          .pairing_mode = 0,
                          .reconnect_timeout = 5,
                          .report_rate = 90,
                          .vendor_id_source = 1,
                          .verndor_id = 0,  // Must be 0x3434
                          .product_id = PRODUCT_ID};
  ckbt51_set_param(&param);
}

void bluetooth_enter_disconnected_kb(uint8_t host_idx) {
  if (bt_factory_reset) {
    bt_factory_reset = false;
    ckbt51_param_init();
  }
  /* CKBT51 bluetooth module boot time is slower, it enters disconnected after boot,
     so we place initialization here. */
  if (firstDisconnect && sync_timer_read32() < 1000 && get_transport() == TRANSPORT_BLUETOOTH) {
    ckbt51_param_init();
    bluetooth_connect();
    firstDisconnect = false;
  }
}

void ckbt51_default_ack_handler(uint8_t *data, uint8_t len) {
  if (data[1] == 0x45) {
    module_param_t param = {.event_mode = 0x02,
                            .connected_idle_timeout = 7200,
                            .pairing_timeout = 180,
                            .pairing_mode = 0,
                            .reconnect_timeout = 5,
                            .report_rate = 90,
                            .vendor_id_source = 1,
                            .verndor_id = 0,  // Must be 0x3434
                            .product_id = PRODUCT_ID};
    ckbt51_set_param(&param);
  }
}

void bluetooth_pre_task(void) {
  static uint8_t mode = 1;

  if (readPin(USB_BT_MODE_SELECT_PIN) != mode) {
    if (readPin(USB_BT_MODE_SELECT_PIN) != mode) {
      mode = readPin(USB_BT_MODE_SELECT_PIN);
      set_transport(mode == 0 ? TRANSPORT_BLUETOOTH : TRANSPORT_USB);
    }
  }
}

void battery_calculte_voltage(uint16_t value) {
  uint16_t voltage = ((uint32_t)value) * 2246 / 1000;

  if (rgb_matrix_is_enabled()) {
    uint32_t totalBuf = 0;

    for (uint8_t i = 0; i < DRIVER_COUNT; i++)
      for (uint8_t j = 0; j < 192; j++) totalBuf += g_pwm_buffer[i][j];
    /* We assumpt it is linear relationship*/
    uint32_t compensation = 60 * totalBuf / RGB_MATRIX_LED_COUNT / 255 / 3;
    voltage += compensation;
  }
  battery_set_voltage(voltage);
}

bool raw_hid_receive_user(uint8_t *data, uint8_t length) {
  switch (data[0]) {
    case 0xAA:
      ckbt51_dfu_rx(data, length);
      raw_hid_send(data, length);
      return false;
  }
  return true;
}
