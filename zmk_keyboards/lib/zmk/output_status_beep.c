/* Copyright (c) 2021 Megamind (megamind4089)
 * Copyright (c) 2024 The ZMK Contributors
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zmk/event_manager.h>
#include <zmk/endpoints.h>
#include <zmk/events/endpoint_changed.h>
#include <zmk/events/ble_active_profile_changed.h>

#include "zmk/pwm_buzzer.h"

static void play_sound_ble_0() {
  (void)pwm_buzzer_beep(1000000);
  pwm_buzzer_beep(500000);
  pwm_buzzer_beep(250000);
  pwm_buzzer_beep(100000);
  pwm_buzzer_beep(50000);
}

static void play_sound_ble_1() {
  pwm_buzzer_beep(1500000);
  pwm_buzzer_beep(3900000);
  pwm_buzzer_beep(1500000);
  pwm_buzzer_beep(1500000);
}

static void play_sound_ble_2() {
  pwm_buzzer_beep(1500000);
  pwm_buzzer_beep(3900000);
}

static void play_sound_ble_3() {
  pwm_buzzer_beep(2000000);
  pwm_buzzer_beep(3900000);
}

static void play_sound_ble_4() {
  pwm_buzzer_beep(2500000);
  pwm_buzzer_beep(3900000);
}

static void play_sound_usb() {
  pwm_buzzer_beep(3000000);
  pwm_buzzer_beep(1500000);
  pwm_buzzer_beep(750000);
}

static void play_beep(uint8_t index) {
  switch (index) {
    case 0:
      play_sound_ble_0();
      break;
    case 1:
      play_sound_ble_1();
      break;
    case 2:
      play_sound_ble_2();
      break;
    case 3:
      play_sound_ble_3();
      break;
    case 4:
      play_sound_ble_4();
      break;
    case 5:
      play_sound_usb();
      break;
    default:
      break;
  }
}

static int output_status_listener(const zmk_event_t *eh) {
  static int index = -1;
  int new_index = -1;
  switch (zmk_endpoints_selected().transport) {
    case ZMK_TRANSPORT_USB:
      new_index = 5;
      break;
    case ZMK_TRANSPORT_BLE:
      new_index = zmk_ble_active_profile_index();
      break;
  }
  if (new_index != index) {
    play_beep(new_index);
    index = new_index;
  }
  return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(output_status_beep, output_status_listener)
ZMK_SUBSCRIPTION(output_status_beep, zmk_ble_active_profile_changed);
ZMK_SUBSCRIPTION(output_status_beep, zmk_endpoint_changed);
