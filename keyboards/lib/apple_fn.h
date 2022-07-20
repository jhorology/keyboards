#pragma once

#include "quantum.h"

// VIA custom keycodes
#ifdef APPLE_FN_ENABLE
#define APPLE_FN USER00  // Apple fn/globe key
#define APPLE_FF USER01  // apple fn/globe key with remapping F1-12 to 1-0,minus,equals keys
void process_apple_fn(uint16_t keycode, keyrecord_t *record);
void process_apple_ff(uint16_t keycode, keyrecord_t *record);
bool process_apple_ff_fkey(uint16_t fkey_index, keyrecord_t *record);
#else
#define APPLE_FN KC_RALT
#define APPLE_FF KC_RALT
#endif
