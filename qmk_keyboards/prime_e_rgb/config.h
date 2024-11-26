#pragma once

#define CUSTOM_CONFIG_MAC_MODE_PIN B2
#define CUSTOM_CONFIG_USJ_MODE_PIN B3

#define DYNAMIC_KEYMAP_LAYER_COUNT 6

#if __has_include("secure_config.h")
#  include "secure_config.h"
#else
#  define VIA_FIRMWARE_VERSION 0
#  define SECURE_UNLOCK_SEQUENCE {{0, 0}, {1, 12}}
#endif
