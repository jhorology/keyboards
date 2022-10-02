/* Copyright 2022 zhaqian
 * Modified 2022 masasfumi
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
#include QMK_KEYBOARD_H

#include "radial_controller.h"

#include "custom_config.h"
#include "custom_keycodes.h"

static void process_dial(int16_t direction, bool pressed);
static void process_dial_encoder(int16_t direction);
static void process_dial_keyswitch(int16_t direction, bool pressed);
static void report_dial_keyswitch(void);
static uint32_t dial_rotation_service(uint32_t trigger_time, void* cb_arg);
static report_radial_controller_t radial_controller_report;
static int16_t rotating_direction;
static deferred_token dial_service_token;

bool process_radial_controller(uint16_t keycode, keyrecord_t* record) {
  switch (keycode) {
    case DIAL_BUT:
      radial_controller_report.button = record->event.pressed;
      radial_controller_report.dial = 0;
      host_radial_controller_send(&radial_controller_report);
      return false;
    case DIAL_L:
      process_dial(-1, record->event.pressed);
      return false;
    case DIAL_R:
      process_dial(1, record->event.pressed);
      return false;
  }
  return true;
}

static void process_dial(int16_t direction, bool pressed) {
  switch (custom_config_get_rc_dial_mode()) {
    case ENCODER:
      if (pressed) {
        process_dial_encoder(direction);
      }
      return;
    case KEYSWITCH:
      process_dial_keyswitch(direction, pressed);
      return;
  }
}

static void process_dial_encoder(int16_t direction) {
  uint16_t amount = custom_config_get_rc_deg_per_click() * 10;
  // 1/4 fine speed
  if (get_mods() & MOD_MASK_SHIFT) {
    amount >>= 2;
  }
  radial_controller_report.dial = direction * amount;
  host_radial_controller_send(&radial_controller_report);
  radial_controller_report.dial = 0;
}

static void process_dial_keyswitch(int16_t direction, bool pressed) {
  static uint8_t cw;
  static uint8_t ccw;
  if (direction > 0 && pressed) {
    cw++;
  } else if (direction > 0 && !pressed && cw) {
    cw--;
  } else if (direction < 0 && pressed) {
    ccw++;
  } else if (direction < 0 && !pressed && ccw) {
    ccw--;
  }
  if (pressed && direction) {
    rotating_direction = direction;
  } else if (cw && !ccw) {
    rotating_direction = 1;
  } else if (!cw && ccw) {
    rotating_direction = -1;
  } else {
    rotating_direction = 0;
    if (dial_service_token) {
      cancel_deferred_exec(dial_service_token);
      dial_service_token = 0;
    }
    return;
  }
  if (!dial_service_token) {
    report_dial_keyswitch();
    dial_service_token = defer_exec(RADIAL_CONTROLLER_REPORT_INTERVAL_MILLIS, &dial_rotation_service, NULL);
  }
}

/**
 * @typedef Callback to execute.
 * @param trigger_time[in] the intended trigger time to execute the callback -- equivalent time-space as timer_read32()
 * @param cb_arg[in] the callback argument specified when enqueueing the deferred executor
 * @return non-zero re-queues the callback to execute after the returned number of milliseconds. Zero cancels repeated
 * execution.
 */
static uint32_t dial_rotation_service(uint32_t trigger_time, void* cb_arg) {
  if (rotating_direction) {
    report_dial_keyswitch();
    return RADIAL_CONTROLLER_REPORT_INTERVAL_MILLIS;
  }
  dial_service_token = 0;
  return 0;
}

static void report_dial_keyswitch() {
  int16_t speed = custom_config_get_rc_deg_per_sec() * RADIAL_CONTROLLER_REPORT_INTERVAL_MILLIS / 100;
  if (speed > 3600) {
    speed = 3600;
  }
  // 1/4 fine speed
  if (get_mods() & MOD_MASK_SHIFT) {
    speed >>= 2;
  }
  radial_controller_report.dial = rotating_direction * speed;
  host_radial_controller_send(&radial_controller_report);
  radial_controller_report.dial = 0;
}
