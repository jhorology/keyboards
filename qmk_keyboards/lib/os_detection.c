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
#  define OS_DETECTION_TIMEOUT_MILLIS 500
#endif

#define DTYPE_STRING 0x03
#define DTYPE_CONFIG 0x02

#ifdef OS_DETECTION_DEBUG_ENABLE
#  define NUM_DESCRIPTOR_REQUESTS 24
static uint8_t fingerprint[NUM_DESCRIPTOR_REQUESTS][2];
static uint8_t descriptor_request_count;
static uint32_t test1;
static uint32_t test2;
static uint32_t test3;
#endif

static uint32_t wlengths;
static uint8_t string_count;
static bool end_detection;
static deferred_token timeout_token;
static os_variant_t detected_os;

static uint32_t os_detection_timeout_callback(uint32_t trigger_time, void* cb_arg);

__attribute__((weak)) void os_detection_update_kb(os_variant_t os) {}

void process_os_detection(const uint8_t dtype, const uint16_t w_length) {
  // don't need hi-byte
  uint8_t wlength = (uint8_t)(w_length > 0xff ? 0 : w_length);
#ifdef OS_DETECTION_DEBUG_ENABLE
  if (descriptor_request_count < NUM_DESCRIPTOR_REQUESTS) {
    fingerprint[descriptor_request_count][0] = dtype;
    fingerprint[descriptor_request_count][1] = wlength;
    descriptor_request_count++;
  }
#endif
  if (!end_detection) {
    if (dtype == DTYPE_STRING) {
      wlengths <<= 8;
      wlengths += wlength;
      string_count++;
      if (string_count == 3 && (wlengths & 0xff00ffUL) != 0x020002) {
#ifdef OS_DETECTION_DEBUG_ENABLE
        test1 = wlengths;
#endif
        detected_os = NOT_DARWIN;
#ifdef OS_DETECTION_NOTIFY_IMMEDIATELY
        os_detection_update_kb(detected_os);
#endif
        end_detection = true;
      }
    } else {
      if (dtype == DTYPE_CONFIG && string_count == 4 && (wlengths & 0xff00ff00UL) == 0x02000200UL) {
#ifdef OS_DETECTION_DEBUG_ENABLE
        test2 = wlengths;
#endif
        detected_os = DARWIN;
#ifdef OS_DETECTION_NOTIFY_IMMEDIATELY
        // TODO resting inside hook causes freez
        os_detection_update_kb(detected_os);
#endif
        end_detection = true;
      }
      string_count = 0;
    }
  }
  if (timeout_token) {
    extend_deferred_exec(timeout_token, OS_DETECTION_TIMEOUT_MILLIS);
  } else {
    timeout_token = defer_exec(OS_DETECTION_TIMEOUT_MILLIS, os_detection_timeout_callback, NULL);
  }
}

os_variant_t detected_host_os(void) { return detected_os; }

#ifdef OS_DETECTION_DEBUG_ENABLE
void send_os_fingerprint() {
  send_string("wlengths: 0x");
  send_dword(wlengths);
  send_string("\ntest1: 0x");
  send_dword(test1);
  send_string("\ntest2: 0x");
  send_dword(test2);
  send_string("\ntest3: 0x");
  send_dword(test3);
  send_string(",\ndetected_os: ");
  switch (detected_os) {
    case UNSURE:
      send_string("'UNSURE'");
      break;
    case DARWIN:
      send_string("'DARWIN'");
      break;
    case NOT_DARWIN:
      send_string("'NOT_DARWIN'");
      break;
  }
  send_string(",\nfingerprint: [");
  for (uint8_t i = 0; i < descriptor_request_count; i++) {
    if (i >= NUM_DESCRIPTOR_REQUESTS) break;
    if (i > 0) {
      send_string(", ");
    }
    send_string("[0x");
    send_byte(fingerprint[i][0]);
    send_string(", 0x");
    send_byte(fingerprint[i][1]);
    send_string("]");
  }
  send_string("]\n");
  // clear data
  descriptor_request_count = 0;
  test1 = 0;
  test2 = 0;
  test3 = 0;
}
#endif

static uint32_t os_detection_timeout_callback(uint32_t trigger_time, void* cb_arg) {
#ifdef OS_DETECTION_DEBUG_ENABLE
  if (descriptor_request_count < NUM_DESCRIPTOR_REQUESTS) {
    // end of request sequence
    fingerprint[descriptor_request_count][0] = 0;
    fingerprint[descriptor_request_count][1] = 0;
    descriptor_request_count++;
  }
#endif
  if (!end_detection && string_count >= 3) {
#ifdef OS_DETECTION_DEBUG_ENABLE
    test3 = wlengths;
#endif
    detected_os = NOT_DARWIN;
#ifdef OS_DETECTION_NOTIFY_IMMEDIATELY
    os_detection_update_kb(detected_os);
#endif
    end_detection = true;
  }
#ifndef OS_DETECTION_NOTIFY_IMMEDIATELY
  if (end_detection) {
    os_detection_update_kb(detected_os);
  }
#endif
  string_count = 0;
  end_detection = false;
  timeout_token = 0;
  return 0;
}
