#pragma once

#include "ec_config.h"
#include "lib/my_keyboard_common.h"

// custoim keycodes
enum ec_keycodes {
  EC_CALD = CUSTOM_KEYCODES_SAFE_RANGE,  // send calibration data
  EC_PSET,                               // Show presets
  EC_SCAL,                               // save calibration data
  EC_PRESET_MAP_START,                   // send calibration data as keystrokes
  EC_PRESET_MAP_END = EC_PRESET_MAP_START + EC_NUM_PRESET_MAPS - 1,
  EC_PRESET_START = EC_PRESET_MAP_END + 1,  // send calibration data as keystrokes
  EC_PRESET_END = EC_PRESET_START + EC_NUM_PRESETS - 1,
#ifdef EC_DEBUG_ENABLE
  EC_DBG0,  // debug send misc state
  EC_DBG1,  // debug send calibration data
  EC_DBG2,  // debug send key config
#endif
};

// custom keycode for selecting EC preset map
#define ECM(preset_map_index) (EC_PRESET_MAP_START + preset_map_index)

// EC preset
#define EC(preset_index) (EC_PRESET_START + preset_index)

// layer index
#define EC_PRESET_MAP_LAYER_START (DYNAMIC_KEYMAP_LAYER_COUNT - EC_NUM_PRESET_MAPS)
#define EC_PRESET_MAP_LAYER_END (DYNAMIC_KEYMAP_LAYER_COUNT - 1)
#define EC_PRESET_MAP(map_index) (EC_PRESET_MAP_LAYER_START + map_index)

extern const uint16_t ec_bottoming_reading_default[MATRIX_ROWS][MATRIX_COLS];
extern const uint16_t ec_noise_floor_default[MATRIX_ROWS][MATRIX_COLS];
extern const ec_preset_t ec_presets_default[EC_NUM_PRESETS];
