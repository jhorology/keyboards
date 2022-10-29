#pragma once

#include <quantum.h>

typedef enum non_mac_fn_key {
  FN_F1 = 0,
  FN_F2,
  FN_F3,
  FN_F4,
  FN_F5,
  FN_F6,
  FN_F7,
  FN_F8,
  FN_F9,
  FN_F10,
  FN_F11,
  FN_F12,
  FN_SPC,                  // Hey Siri
  FN_Q,                    // Qick Notes
  FN_E,                    // Emoji & Symbols
  FN_A,                    // Focus Dock
  FN_D,                    // Start Dictation
  FN_F,                    // Toggle full screen mode
  FN_H,                    // Show Desktop
  FN_C,                    // Show Control Cnecter
  FN_N,                    // Show Notification
  FN_M,                    // Focus Menubar
  NON_MAC_FN_KEY_ENTRIES,  // num of total keys
  FN_UNKNOWN = 0xfff
  // TOTAL 22 keys
} non_mac_fn_key_t;

bool process_apple_fn(uint16_t keycode, keyrecord_t *record);
bool apple_fn_get_state(void);
