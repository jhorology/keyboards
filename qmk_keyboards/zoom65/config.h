#pragma once

#include "config_common.h"

/* key matrix size */
#define MATRIX_ROWS 5
#define MATRIX_COLS 15

#define MATRIX_ROW_PINS \
  { F0, E6, D5, F1, F4 }
#define MATRIX_COL_PINS \
  { C7, D3, D2, D1, D0, B7, B3, B2, C6, B6, B5, B4, D7, D6, D4 }

#define DIODE_DIRECTION COL2ROW

/* Enable encoder */
#define ENCODERS_PAD_A \
  { B1 }
#define ENCODERS_PAD_B \
  { B0 }

#define ENCODERS 1
#define ENCODER_RESOLUTION 1
/*
  encoder lookup table

  Index
    bit0: Pad A new  state
    bit1: Pad B new  state
    bit2: Pad A prev state
    bit3: Pad B prev state
  Value
    clockwise: -1
    counter clockwise: 1
*/
#define ENCODER_LOOKUP_TABLE \
  { 0, 0, 0, 1, 1, 0, 0, -1, 0, 0, 0, 0, -1, 0, 0, 0 }

#define LED_CAPS_LOCK_PIN F7
#define LED_PIN_ON_STATE 0

/* ViA layout options */
#define LAYOUT_OPTION_SPLIT_BS 0x8
#define LAYOUT_OPTION_ISO_ENTER 0x4
#define LAYOUT_OPTION_SPLIT_LEFT_SHIFT 0x2
#define LAYOUT_OPTION_SPLIT_SPACE 0x1

#if __has_include("secure_config.h")
#  include "secure_config.h"
#else
#  define VIA_FIRMWARE_VERSION 0
#  define SECURE_UNLOCK_SEQUENCE \
    {                            \
      {0, 0}, { 2, 12 }          \
    }
#endif
