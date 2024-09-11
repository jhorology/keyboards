/* Copyright (c) 2021 Megamind (megamind4089)
 * Copyright (c) 2024 The ZMK Contributors
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/settings/settings.h>

#include <zmk/event_manager.h>
#include <zmk/endpoints.h>
#include <zmk/pwm_buzzer.h>
#include <zmk/events/endpoint_changed.h>
#include <zmk/events/ble_active_profile_changed.h>

#include "zmk/beep.h"

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#define BEEP_ON_DURATION K_MSEC(60)
#define BEEP_OFF_DURATION K_MSEC(50)

struct beep_entity_state {
  bool on;
};

static struct beep_entity_state state = {
  .on = true,
};

static int beep_index = -1;

static inline void beep(uint32_t period) {
  (void)zmk_pwm_buzzer_beep(period, period / 2, BEEP_ON_DURATION, BEEP_OFF_DURATION);
}

static void play_beep_ble_0() {
  beep(1000000);
  beep(500000);
  beep(250000);
  beep(100000);
  beep(50000);
}

static void play_beep_ble_1() {
  beep(1500000);
  beep(3900000);
  beep(1500000);
  beep(1500000);
}

static void play_beep_ble_2() {
  beep(1500000);
  beep(3900000);
}

static void play_beep_ble_3() {
  beep(2000000);
  beep(3900000);
}

static void play_beep_ble_4() {
  beep(2500000);
  beep(3900000);
}

static void play_beep_usb() {
  beep(3000000);
  beep(1500000);
  beep(750000);
}

static void play_beep(int index) {
  if (state.on) {
    switch (index) {
      case 0:
        play_beep_ble_0();
        break;
      case 1:
        play_beep_ble_1();
        break;
      case 2:
        play_beep_ble_2();
        break;
      case 3:
        play_beep_ble_3();
        break;
      case 4:
        play_beep_ble_4();
        break;
      case 5:
        play_beep_usb();
        break;
      default:
        break;
    }
  }
}

static int zmk_beep_update(void) { return 0; }

#if IS_ENABLED(CONFIG_SETTINGS)

static int beep_settings_load_cb(const char *name, size_t len, settings_read_cb read_cb,
                                 void *cb_arg) {
  const char *next;
  if (settings_name_steq(name, "state", &next) && !next) {
    if (len != sizeof(state)) {
      return -EINVAL;
    }

    int rc = read_cb(cb_arg, &state, sizeof(state));
    if (rc >= 0) {
      rc = zmk_beep_update();
    }
    return MIN(rc, 0);
  }
  return -ENOENT;
}

SETTINGS_STATIC_HANDLER_DEFINE(beep, "beep", NULL, beep_settings_load_cb, NULL, NULL);

static void beep_save_work_handler(struct k_work *work) {
  settings_save_one("beep/state", &state, sizeof(state));
}

K_WORK_DELAYABLE_DEFINE(beep_save_work, beep_save_work_handler);

#endif /* IS_ENABLED(CONFIG_SETTINGS) */

static int zmk_beep_init(void) { return zmk_beep_update(); }

static int zmk_beep_update_and_save(void) {
  int rc = zmk_beep_update();
  if (rc != 0) {
    return rc;
  }

#if IS_ENABLED(CONFIG_SETTINGS)
  int ret = k_work_reschedule(&beep_save_work, K_MSEC(CONFIG_ZMK_SETTINGS_SAVE_DEBOUNCE));
  return MIN(ret, 0);
#else
  return 0;
#endif /* IS_ENABLED(CONFIG_SETTINGS) */
}

// API --->

int zmk_beep_play() {
  play_beep(beep_index);
  return 0;
}

int zmk_beep_on(void) {
  state.on = true;
  return zmk_beep_update_and_save();
}

int zmk_beep_off(void) {
  state.on = false;
  return zmk_beep_update_and_save();
}

int zmk_beep_toggle(void) { return state.on ? zmk_beep_off() : zmk_beep_on(); }

bool zmk_beep_is_on(void) { return state.on; }

// <--- API

static int output_status_listener(const zmk_event_t *eh) {
  int new_index = -1;
  switch (zmk_endpoints_selected().transport) {
    case ZMK_TRANSPORT_USB:
      new_index = 5;
      break;
    case ZMK_TRANSPORT_BLE:
      new_index = zmk_ble_active_profile_index();
      break;
  }
  if (new_index != beep_index) {
    play_beep(new_index);
    beep_index = new_index;
  }
  return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(output_status_beep, output_status_listener)
ZMK_SUBSCRIPTION(output_status_beep, zmk_ble_active_profile_changed);
ZMK_SUBSCRIPTION(output_status_beep, zmk_endpoint_changed);

SYS_INIT(zmk_beep_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
