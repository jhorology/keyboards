#pragma once

/* ViA layout options */

// option 0: 5 bit
#define LAYOUT_OPTION_ISO_ENTER (0x1 << 5)

// option 1: 3-4 bit
#define LAYOUT_OPTION_BOTTOM_ROW_ANSI 0
#define LAYOUT_OPTION_BOTTOM_ROW_TSANGAN (0x1 << 3)
#define LAYOUT_OPTION_BOTTOM_ROW_HHKB (0x2 << 3)
#define LAYOUT_OPTION_BOTTOM_ROW_WKL (0x3 << 3)

// option 2: 2 bit
#define LAYOUT_OPTION_SPLIT_BS (0x1 << 2)

// option 3: 1 bit
#define LAYOUT_OPTION_SPLIT_LEFT_SHIFT (0x1 << 1)

// option 4: 0 bit
#define LAYOUT_OPTION_SPLIT_RIGHT_SHIFT 0x1

#if __has_include("secure_config.h")
#  include "secure_config.h"
#else
#  define VIA_FIRMWARE_VERSION 0
#  define SECURE_UNLOCK_SEQUENCE \
    {                            \
      {0, 0}, { 2, 13 }          \
    }
#endif
