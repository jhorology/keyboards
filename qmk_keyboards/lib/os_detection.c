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
#  define OS_DETECTION_TIMEOUT_MILLIS 1000
#endif

#define DTYPE_STRING 0x03

#ifdef OS_DETECTION_DEBUG_ENABLE
#  define NUM_DESCRIPTOR_REQUESTS 32
static uint8_t descriptor_requests[NUM_DESCRIPTOR_REQUESTS][2];
static uint8_t descriptor_requests_count;
#endif

// static deferred_token timeout_token;

typedef struct {
  uint8_t count;
  uint8_t cnt_02;
  uint8_t cnt_04;
  uint8_t cnt_ff;
  uint8_t last_wlength;
  os_detection_state_t state;
} setups_data_t;

setups_data_t setups_data = {0};
static os_variant_t detected_os;

// static void make_guess(void);
// static uint32_t os_detection_timeout_callback(uint32_t trigger_time, void* cb_arg);

__attribute__((weak)) void os_detection_update_kb(os_variant_t os) {}

void process_os_detection(const uint8_t dtype, const uint16_t w_length) {
  // don't need hi-byte
  uint8_t wlength = (uint8_t)(w_length > 0xff ? 0 : w_length);
#ifdef OS_DETECTION_DEBUG_ENABLE
  if (descriptor_requests_count < NUM_DESCRIPTOR_REQUESTS) {
    descriptor_requests[descriptor_requests_count][0] = dtype;
    descriptor_requests[descriptor_requests_count][1] = wlength;
    descriptor_requests_count++;
  }
#endif
  if (dtype == DTYPE_STRING) {
    setups_data.count++;
    setups_data.state = DETECTING;
    setups_data.last_wlength = wlength;
    if (wlength == 0x2) {
      setups_data.cnt_02++;
    } else if (wlength == 0x4) {
      setups_data.cnt_04++;
    } else if (wlength == 0xFF) {
      setups_data.cnt_ff++;
    }
  }
  // if (timeout_token) {
  //   extend_deferred_exec(timeout_token, OS_DETECTION_TIMEOUT_MILLIS);
  // } else {
  //   timeout_token = defer_exec(OS_DETECTION_TIMEOUT_MILLIS, os_detection_timeout_callback, NULL);
  // }
}

os_variant_t detected_host_os(void) { return detected_os; }
os_detection_state_t os_detection_state(void) { return setups_data.state; }

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
  send_string(",\nstate: ");
  switch (setups_data.state) {
    case INITIAL:
      send_string("'INITIAL'");
      break;
    case DETECTING:
      send_string("'DETECTING'");
      break;
    case DETECTED_ON_INTERRUPT:
      send_string("'DETECTED_ON_INTERRUPT'");
      break;
    case DETECTED_ON_TIMEOUT:
      send_string("'DETECTED_ON_TIMEOUT'");
      break;
    case IGNORE:
      send_string("'IGNORE'");
      break;
  }
  send_string(",\ndetected_os: ");
  switch (detected_os) {
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
      send_string("'IOS'");
      break;
  }
  send_string(",\nrequests: [");
  for (uint8_t i = 0; i < descriptor_requests_count; i++) {
    if (i >= NUM_DESCRIPTOR_REQUESTS) break;
    if (i > 0) {
      send_string(", ");
    }
    send_string("[0x");
    send_byte(descriptor_requests[i][0]);
    send_string(", 0x");
    send_byte(descriptor_requests[i][1]);
    send_string("]");
  }
  send_string("]\n");
  // clear data
  setups_data_t empty = {0};
  setups_data = empty;
  descriptor_requests_count = 0;
}
#endif

/*
// Some collected sequences of wLength can be found in tests.
static void make_guess(void) {
  if (setups_data.count < 3) {
    return;
  }
  if (setups_data.cnt_ff >= 2 && setups_data.cnt_04 >= 1) {
    detected_os = OS_WINDOWS;
    return;
  }

  if (setups_data.count == setups_data.cnt_ff) {
    // Linux has 3 packets with 0xFF.
    detected_os = OS_LINUX;
    return;
  }

  if (setups_data.count == 5 && setups_data.last_wlength == 0xFF && setups_data.cnt_ff == 1 &&
      setups_data.cnt_02 == 2) {
    // Mac mini M1
    detected_os = OS_MACOS;
    return;
  }

  if (setups_data.count == 7 && setups_data.last_wlength == 0xFF && setups_data.cnt_ff == 1 &&
      setups_data.cnt_02 == 3) {
    // iMac Pro
    detected_os = OS_MACOS;
    return;
  }

  if (setups_data.count == 4 && setups_data.cnt_ff == 0 && setups_data.cnt_02 == 2) {
    // iOS and iPadOS don't have the last 0xFF packet.
    detected_os = OS_IOS;
    return;
  }
  if (setups_data.cnt_ff == 0 && setups_data.cnt_02 == 3 && setups_data.cnt_04 == 1) {
    // This is actually PS5.
    detected_os = OS_LINUX;
    return;
  }
  if (setups_data.cnt_ff >= 1 && setups_data.cnt_02 == 0 && setups_data.cnt_04 == 0) {
    // This is actually Quest 2 or Nintendo Switch.
    detected_os = OS_LINUX;
    return;
  }
}
*/
/*
static uint32_t os_detection_timeout_callback(uint32_t trigger_time, void* cb_arg) {
  timeout_token = 0;
  make_guess();
  setups_data.state = DETECTED_ON_TIMEOUT;
  os_detection_update_kb(detected_os);
  return 0;
}
*/
