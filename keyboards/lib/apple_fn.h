#pragma once

#include "quantum.h"

// VIA custom keycodes
#ifdef APPLE_FN_ENABLE
#    define APPLE_FN USER00
void process_apple_fn(uint16_t keycode, keyrecord_t *record);
#else
#    define APPLE_FN KC_RALT
#endif
