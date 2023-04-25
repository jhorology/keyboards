#pragma once

/* VIA Layout Options */
// bit 5
#define LAYOUT_OPTION_SPLIT_BS 0x20
// bit 4
#define LAYOUT_OPTION_ISO_ENTER 0x10
// bit 3
#define LAYOUT_OPTION_SPLIT_LEFT_SHIFT 0x08
// bit 2
#define LAYOUT_OPTION_SPLIT_RIGHT_SHIFT 0x04
// bit 0-1
#define LAYOUT_OPTION_BOTTOM_ROW_ANSI 0x0
#define LAYOUT_OPTION_BOTTOM_ROW_TSANGAN 0x1
#define LAYOUT_OPTION_BOTTOM_ROW_HHKB 0x2
#define LAYOUT_OPTION_BOTTOM_ROW_WKL 0x3

#if __has_include("secure_config.h")
#  include "secure_config.h"
#else
#  define VIA_FIRMWARE_VERSION 0
#  define SECURE_UNLOCK_SEQUENCE \
    {                            \
      {0, 0}, { 2, 12 }          \
    }
#endif
