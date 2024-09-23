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

#include <dt-bindings/zmk/midi.h>

#include "zmk/beep.h"

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

static const uint32_t chromatic_base_periods[] = {
  /* note=0 C-1 */
  229740225UL,
  /* note=1 C#-1/Db-1 */
  216845897UL,
  /* note=2 D-1 */
  204675272UL,
  /* note=3 D#-1/Eb-1 */
  193187732UL,
  /* note=4 E-1 */
  182344937UL,
  /* note=5 F-1 */
  172110702UL,
  /* note=6 F#-1/Gb-1 */
  162450871UL,
  /* note=7 G-1/ */
  153333204UL,
  /* note=8 G#-1/Ab-1 */
  144727273UL,
  /* note=9 A-1 */
  136604355UL,
  /* note=10 A#-1/Bb-1 */
  128937342UL,
  /* note=11 B-1 */
  121700645UL,
};

static const uint8_t melody_disconnected[5] = {NOTE_A(5), NOTE_G(5), NOTE_E(5), NOTE_C(5),
                                               NOTE_A(4)};

/*
 * 0:_____  1:.____  2:..___  3:...__  4:...._
 * 5:.....  6:_....  7:__...  8:___..  9:____.
 */
static const uint8_t morse_num_codes[] = {0x0, 0x1, 0x3, 0x7, 0xf, 0x1f, 0x1e, 0x1c, 0x18, 0x10};

/* note number to PWM preiods */
#define PWM_NOTE(note) (chromatic_base_periods[(note) % 12] >> ((note) / 12))

#define MORSE_UNIT_MSEC 40
#define MORSE_DOT_UNIT K_MSEC(MORSE_UNIT_MSEC)
#define MORSE_DASH_UNIT K_MSEC(MORSE_UNIT_MSEC * 3)
#define MORSE_DOT_NOTE NOTE_Fs(5)
#define MORSE_DASH_NOTE NOTE_C(5)

struct beep_entity_state {
  bool on;
};

static struct beep_entity_state state = {
  .on = true,
};

struct output_status {
  uint8_t output_num;
  bool is_connected;
};

static struct output_status status = {.output_num = 0xff, .is_connected = false};

static void play_output_status(struct output_status *status) {
  // if (!state.on || num > 9) return;
  if (status->output_num > 9) return;

  uint8_t code = morse_num_codes[status->output_num];
  for (int i = 0; i < 5; i++) {
    bool is_dot = (code >> i) & 1;
    uint8_t note = (status->is_connected ? (is_dot ? MORSE_DOT_NOTE : MORSE_DASH_NOTE)
                                         : melody_disconnected[i]) +
                   status->output_num;
    uint32_t period = PWM_NOTE(note);
    (void)zmk_pwm_buzzer_beep(period, period / 2, is_dot ? MORSE_DOT_UNIT : MORSE_DASH_UNIT,
                              i == 4 ? MORSE_DASH_UNIT : MORSE_DOT_UNIT);
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
  play_output_status(&status);
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
  uint8_t output_num = 0;
  bool is_connected = true;
  switch (zmk_endpoints_selected().transport) {
    case ZMK_TRANSPORT_USB:
      output_num = 6;
      break;
    case ZMK_TRANSPORT_BLE:
      output_num = zmk_ble_active_profile_index() + 1;
      is_connected = zmk_ble_active_profile_is_connected();
      break;
  }
  if (output_num > 0 && (output_num != status.output_num || is_connected != status.is_connected)) {
    status.output_num = output_num;
    status.is_connected = is_connected;
    play_output_status(&status);
  }
  return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(output_status_beep, output_status_listener)
ZMK_SUBSCRIPTION(output_status_beep, zmk_ble_active_profile_changed);
ZMK_SUBSCRIPTION(output_status_beep, zmk_endpoint_changed);

SYS_INIT(zmk_beep_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
