#pragma once

/* ViA layout options */
#define LAYOUT_OPTION_SPLIT_BS 0x10
#define LAYOUT_OPTION_ISO_ENTER 0x8
#define LAYOUT_OPTION_SPLIT_LEFT_SHIFT 0x4
#define LAYOUT_OPTION_SPLIT_RIGHT_SHIFT 0x2
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
