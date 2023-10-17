/* Copyright 2022 Ruslan Sayfutdinov (@KapJI)
 * Modfiied 2022 Masafumi
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

#include "os_fingerprint.h"

#include <deferred_exec.h>

#include "send_string_macro.h"

#ifndef OS_FINGERPRINT_TIMEOUT_MILLIS
#  define OS_FINGERPRINT_TIMEOUT_MILLIS 1000
#endif

#define DTYPE_DEVICE 0x01
#define DTYPE_CONFIG 0x02
#define DTYPE_STRING 0x03

#ifdef OS_FINGERPRINT_DEBUG_ENABLE
#  define NUM_DESCRIPTOR_REQUESTS 32
static uint8_t fingerprint[NUM_DESCRIPTOR_REQUESTS][2];
#endif

static uint8_t request_cnt;
static bool detecting = false;
static uint32_t wlengths = 0;
static deferred_token timeout_token;
static os_variant_t detected_os;

static uint32_t os_fingerprint_timeout_callback(uint32_t trigger_time, void *cb_arg);

__attribute__((weak)) void os_fingerprint_update_kb(os_variant_t os) {}

void trace_usb_get_descriptor(const uint8_t dtype, const uint16_t w_length) {
  // don't need hi-byte
  uint8_t wlength = (uint8_t)(w_length > 0xff ? 0 : w_length);

  if (!detecting && dtype == DTYPE_DEVICE) {
    request_cnt = 0;
    detecting = true;
    wlengths = 0;
    detected_os = UNSURE;
  }
  if (detecting) {
#ifdef OS_FINGERPRINT_DEBUG_ENABLE
    if (request_cnt < NUM_DESCRIPTOR_REQUESTS) {
      fingerprint[request_cnt][0] = dtype;
      fingerprint[request_cnt][1] = wlength;
      request_cnt++;
    }
#endif
    if (detected_os == UNSURE) {
      // detect DARWIN (macOS iOS iPadOS)
      if (dtype == DTYPE_CONFIG && (wlengths & 0xff00ff00) == 0x02000200) {
        detected_os = DARWIN;
#ifdef OS_FINGERPRINT_NOTIFY_IMMEDIATELY
        os_fingerprint_update_kb(detected_os);
#endif
      }
      // TODO other OS

      if (dtype == DTYPE_STRING) {
        wlengths <<= 8;
        wlengths += wlength & 0xff;
      }
    }
    if (timeout_token) {
      extend_deferred_exec(timeout_token, OS_FINGERPRINT_TIMEOUT_MILLIS);
    } else {
      timeout_token =
        defer_exec(OS_FINGERPRINT_TIMEOUT_MILLIS, os_fingerprint_timeout_callback, NULL);
    }
  }
}

static uint32_t os_fingerprint_timeout_callback(uint32_t trigger_time, void *cb_arg) {
#ifdef OS_FINGERPRINT_DEBUG_ENABLE
  if (request_cnt < NUM_DESCRIPTOR_REQUESTS) {
    // end of request sequence
    fingerprint[request_cnt][0] = 0;
    fingerprint[request_cnt][1] = 0;
    request_cnt++;
  }
#endif
  if (detected_os != UNSURE) {
#ifndef OS_FINGERPRINT_NOTIFY_IMMEDIATELY
    os_fingerprint_update_kb(detected_os);
#endif
  } else {
    detected_os = NOT_DARWIN;
    os_fingerprint_update_kb(detected_os);
  }
  detecting = false;
  timeout_token = 0;
  return 0;
}

#ifdef OS_FINGERPRINT_DEBUG_ENABLE
void send_os_fingerprint() {
  send_string("const os_fingerprint = {\n");
  SEND_JS_PROP_VALUE(wlengths, WORD, ",\n");
  switch (detected_os) {
    case UNSURE:
      SEND_JS_SYMBOL_PROP_VALUE(detected_os, "'UNSURE'", STRING, ",\n");
      break;
    case DARWIN:
      SEND_JS_SYMBOL_PROP_VALUE(detected_os, "'DARWIN'", STRING, ",\n");
      break;
    case NOT_DARWIN:
      SEND_JS_SYMBOL_PROP_VALUE(detected_os, "'NOT_DARWIN'", STRING, ",\n");
      break;
  }
  SEND_JS_PROP_2D_ARRAY(fingerprint, request_cnt, 2, BYTE, "\n");
  send_string("}\n");
}
#endif
