#pragma once
#define DEBOUNCE 15

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
#define ENCODER_LOOKUP_TABLE {0, 0, 0, 1, 1, 0, 0, -1, 0, 0, 0, 0, -1, 0, 0, 0}

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
