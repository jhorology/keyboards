/* Copyright 2022 Ruslan Sayfutdinov (@KapJI)
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

#include "os_detection.h"

#include <deferred_exec.h>
#include <send_string.h>

#ifndef OS_DETECTION_TIMEOUT_MILLIS
#  define OS_DETECTION_TIMEOUT_MILLIS 300
#endif

#define STORED_USB_SETUPS 50

#ifdef OS_DETECTION_DEBUG_ENABLE
static uint16_t usb_setups[STORED_USB_SETUPS];
#endif

static deferred_token timeout_token;

typedef struct {
  uint8_t count;
  uint8_t cnt_02;
  uint8_t cnt_04;
  uint8_t cnt_ff;
  uint16_t last_wlength;
  os_variant_t detected_os;
} setups_data_t;

setups_data_t setups_data = {0};

static void make_guess(void);
static uint32_t os_detection_timeout_callback(uint32_t trigger_time, void* cb_arg);

__attribute__((weak)) void os_detection_update_kb(os_variant_t os) {}

void process_wlength(const uint16_t w_length) {
  if (timeout_token == 0) {
    setups_data_t empty = {0};
    setups_data = empty;
  }
#ifdef OS_DETECTION_DEBUG_ENABLE
  if (setups_data.count < STORED_USB_SETUPS) usb_setups[setups_data.count] = w_length;
#endif
  setups_data.count++;
  setups_data.last_wlength = w_length;
  if (w_length == 0x2) {
    setups_data.cnt_02++;
  } else if (w_length == 0x4) {
    setups_data.cnt_04++;
  } else if (w_length == 0xFF) {
    setups_data.cnt_ff++;
  }
  if (timeout_token) {
    extend_deferred_exec(timeout_token, OS_DETECTION_TIMEOUT_MILLIS);
  } else {
    timeout_token = defer_exec(OS_DETECTION_TIMEOUT_MILLIS, os_detection_timeout_callback, NULL);
  }
}

os_variant_t detected_host_os(void) { return setups_data.detected_os; }

#ifdef OS_DETECTION_DEBUG_ENABLE
void send_os_detection_result() {
  send_string("count: 0x");
  send_byte(setups_data.count);
  send_string(",\ncnt_02: 0x");
  send_byte(setups_data.cnt_02);
  send_string(",\ncnt_04: 0x");
  send_byte(setups_data.cnt_04);
  send_string(",\ncnt_ff: 0x");
  send_byte(setups_data.cnt_ff);
  send_string(",\nlast_wlength: 0x");
  send_byte(setups_data.last_wlength);
  send_string(",\ndetected_os: ");
  switch (setups_data.detected_os) {
    case OS_UNSURE:
      send_string("'UNSURE'");
      break;
    case OS_LINUX:
      send_string("'LINUX'");
      break;
    case OS_WINDOWS:
      send_string("'WINDOWS'");
      break;
    case OS_MACOS:
      send_string("'MACOS'");
      break;
    case OS_IOS:
      send_string("IOS");
      break;
  }
  send_string(",\nwlengths: [");
  for (uint8_t i = 0; i < setups_data.count; i++) {
    if (i >= STORED_USB_SETUPS) break;
    if (i == 0) {
      send_string("0x");
    } else {
      send_string(", 0x");
    }
    send_word(usb_setups[i]);
  }
  send_string("]\n");
}
#endif

// Some collected sequences of wLength can be found in tests.
static void make_guess(void) {
  if (setups_data.count < 3) {
    return;
  }
  if (setups_data.cnt_ff >= 2 && setups_data.cnt_04 >= 1) {
    setups_data.detected_os = OS_WINDOWS;
    return;
  }

  if (setups_data.count == setups_data.cnt_ff) {
    // Linux has 3 packets with 0xFF.
    setups_data.detected_os = OS_LINUX;
    return;
  }

  if (setups_data.count == 5 && setups_data.last_wlength == 0xFF && setups_data.cnt_ff == 1 &&
      setups_data.cnt_02 == 2) {
    // Mac mini M1
    setups_data.detected_os = OS_MACOS;
    return;
  }

  if (setups_data.count == 7 && setups_data.last_wlength == 0xFF && setups_data.cnt_ff == 1 &&
      setups_data.cnt_02 == 3) {
    // iMac Pro
    setups_data.detected_os = OS_MACOS;
    return;
  }

  if (setups_data.count == 4 && setups_data.cnt_ff == 0 && setups_data.cnt_02 == 2) {
    // iOS and iPadOS don't have the last 0xFF packet.
    setups_data.detected_os = OS_IOS;
    return;
  }
  if (setups_data.cnt_ff == 0 && setups_data.cnt_02 == 3 && setups_data.cnt_04 == 1) {
    // This is actually PS5.
    setups_data.detected_os = OS_LINUX;
    return;
  }
  if (setups_data.cnt_ff >= 1 && setups_data.cnt_02 == 0 && setups_data.cnt_04 == 0) {
    // This is actually Quest 2 or Nintendo Switch.
    setups_data.detected_os = OS_LINUX;
    return;
  }
}

static uint32_t os_detection_timeout_callback(uint32_t trigger_time, void* cb_arg) {
  timeout_token = 0;
  make_guess();
  os_detection_update_kb(setups_data.detected_os);
  return 0;
}
