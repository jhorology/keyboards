#pragma once

#include "lib/my_keyboard_common.h"

// custoim keycodes
enum ec_keycodes {
  EC_PRESET_START = CUSTOM_KEYCODES_SAFE_RANGE,  // send calibration data as keystrokes
  EC_PRESET_END = EC_PRESET_START + EC_NUM_PRESETS - 1,
  EC_CALD,  // send calibration data
  EC_PSET,  // Show presets
  EC_PMAP,  // Show preset map
#ifdef EC_DEBUG
  EC_DBG
#endif
};

#define EC(preset_index) (EC_PRESET_START + preset_index)
