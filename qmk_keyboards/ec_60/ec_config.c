#include "ec_config.h"

#include <eeprom.h>

#include "ec_60.h"
#include "ec_switch_matrix.h"
#include "lib/send_string_macro.h"

ec_eeprom_config_t ec_eeprom_config;
ec_key_config_t ec_config_keys[MATRIX_ROWS][MATRIX_COLS];
bool bottoming_calibration;

#ifdef EC_DEBUG
static bool ec_eeprom_config_reseted;
static int8_t ec_eeprom_config_error;
#endif /* EC_DEBUG  */

// if defined in ec_60/config.h or ec_60/keymaps/<keymap name>/config.h

#define KEY_TRAVEL(noise_floor, bottoming, x) ((uint32_t)x * (bottoming - noise_floor) / EC_SCALE_RANGE)
#define KEY_THRESHOLD(noise_floor, bottoming, x) (KEY_TRAVEL(noise_floor, bottoming, x) + noise_floor)

#define UPDATE_PRESET_PARAM(preset_index, member, value) \
  ec_preset_t* preset = get_preset(preset_index);        \
  if (preset->member != value) {                         \
    preset->member = value;                              \
    update_matrix(preset_index);                         \
    defer_eeprom_update_preset(preset_index);            \
  }

static uint8_t get_key_preset_index(uint8_t row, uint8_t col);

//  inline functions
// -----------------------------------------------------------------------------------
static inline ec_preset_t* get_preset(uint8_t preset_index) { return &(ec_eeprom_config.presets[preset_index]); }

static inline ec_preset_t* get_key_preset(uint8_t row, uint8_t col) {
  return get_preset(get_key_preset_index(row, col));
}

static inline void defer_eeprom_update_preset(uint8_t preset_index) {
  defer_eeprom_update_block(EC_VIA_CUSTOM_CHANNEL_ID_START + preset_index, 0, &ec_eeprom_config.presets[preset_index],
                            (void*)(EC_VIA_EEPROM_PRESETS + sizeof(ec_preset_t) * preset_index), sizeof(ec_preset_t));
}

//  static routine
// -----------------------------------------------------------------------------------
static uint8_t get_key_preset_index(uint8_t row, uint8_t col) {
  uint16_t keycode = dynamic_keymap_get_keycode(EC_PRESET_MAP(ec_eeprom_config.selected_preset_map_index), row, col);
  if (keycode >= EC_PRESET_START && keycode <= EC_PRESET_END) {
    return keycode - EC_PRESET_START;
  }
  return 0;
}

static int is_preset_valid(ec_preset_t* preset) {
  if (preset->actuation_mode > EC_ACTUATION_MODE_DYNAMIC) return -1;
  if (preset->release_mode > EC_RELEASE_MODE_DYNAMIC) return -2;
  if (preset->actuation_threshold > EC_SCALE_RANGE) return -3;
  if (preset->release_threshold > EC_SCALE_RANGE) return -4;
  if (preset->actuation_travel > (EC_SCALE_RANGE >> 1)) return -5;
  if (preset->release_travel > (EC_SCALE_RANGE >> 1)) return -6;
  if (preset->deadzone > (EC_SCALE_RANGE >> 1)) return -7;
  if (preset->sub_action_actuation_threshold > EC_SCALE_RANGE) return -8;
  if (preset->sub_action_release_threshold > EC_SCALE_RANGE) return -9;

  // EC_PRESETS_DEFAUL_USER may not have bean defined all presets.
  if (preset->actuation_threshold == 0 && preset->actuation_travel == 0 && preset->release_threshold == 0 &&
      preset->release_travel == 0 && preset->deadzone == 0) {
    return -10;
  }
  return 0;
}

static int is_eeprom_valid(void) {
  for (uint8_t index = 0; index < EC_NUM_PRESETS; index++) {
    ec_preset_t* preset = get_preset(index);
    int result = is_preset_valid(preset);
    if (result != 0) return result;
  }
  MATRIX_LOOP_WITH_KEY(if (ec_eeprom_config.bottoming_reading[row][col] <= key->noise_floor ||
                           ec_eeprom_config.bottoming_reading[row][col] > 0xfff) return -9;)
  if (ec_eeprom_config.selected_preset_map_index >= EC_NUM_PRESET_MAPS) return -10;
  return 0;
}

static void update_key(ec_key_config_t* key, ec_preset_t* preset, uint16_t bottoming) {
  uint8_t prev_actuation_mode = key->actuation_mode;
  uint8_t prev_release_mode = key->release_mode;
  // actuation
  key->actuation_mode = preset->actuation_mode;
  switch (preset->actuation_mode) {
    case EC_ACTUATION_MODE_STATIC:
      key->actuation_reference = KEY_THRESHOLD(key->noise_floor, bottoming, preset->actuation_threshold);
      break;
    case EC_ACTUATION_MODE_DYNAMIC:
      key->actuation_reference = KEY_TRAVEL(key->noise_floor, bottoming, preset->actuation_travel);
      break;
  }
  // release
  key->release_mode = preset->release_mode;
  switch (preset->release_mode) {
    case EC_RELEASE_MODE_STATIC:
      key->release_reference = KEY_THRESHOLD(key->noise_floor, bottoming, preset->release_threshold);
      break;
    case EC_RELEASE_MODE_DYNAMIC:
      key->release_reference = KEY_TRAVEL(key->noise_floor, bottoming, preset->release_travel);
      break;
  }
  // deadzone
  key->deadzone = KEY_THRESHOLD(key->noise_floor, bottoming, preset->deadzone);

  // sub action
  key->sub_action_keycode = preset->sub_action_enable ? preset->sub_action_keycode : KC_NO;
  key->sub_action_actuation_threshold =
    KEY_THRESHOLD(key->noise_floor, bottoming, preset->sub_action_actuation_threshold);
  key->sub_action_release_threshold = KEY_THRESHOLD(key->noise_floor, bottoming, preset->sub_action_release_threshold);

  // reset extremum
  if (prev_actuation_mode != preset->actuation_mode || prev_release_mode != preset->release_mode) {
    key->extremum = key->noise_floor;
  }
  if (key->extremum < key->deadzone) {
    key->extremum = key->deadzone;
  }
}

static void update_matrix(uint8_t preset_index) {
  MATRIX_LOOP_WITH_KEY(                                         //
    uint8_t key_preset_index = get_key_preset_index(row, col);  //
    if (preset_index == key_preset_index) {
      update_key(key, get_preset(preset_index), ec_eeprom_config.bottoming_reading[row][col]);
    }  //
  )
}

//  export functions
// -----------------------------------------------------------------------------------
void ec_config_reset(void) {
  ec_preset_t preset_default = EC_STATIC_PRESET_DEFAULT;
  // Default values
  for (uint8_t preset_index = 0; preset_index < EC_NUM_PRESETS; preset_index++) {
    ec_preset_t* preset = get_preset(preset_index);
    // I don't want to lose presets for each update firmware
    memcpy_P(preset, &ec_presets_default[preset_index], sizeof(ec_preset_t));
    if (is_preset_valid(preset) != 0) {
      ec_eeprom_config.presets[preset_index] = preset_default;
    }
  }
  MATRIX_LOOP(ec_eeprom_config.bottoming_reading[row][col] = pgm_read_word(&ec_bottoming_reading_default[row][col]);)

  memcpy_P(&ec_eeprom_config.bottoming_reading[0][0], &ec_bottoming_reading_default[0][0],
           MATRIX_ROWS * MATRIX_COLS * 2);
  ec_eeprom_config.selected_preset_map_index = 0;

  // Write default value to EEPROM now
  eeprom_update_block(&ec_eeprom_config, (void*)VIA_EEPROM_CUSTOM_CONFIG_USER_ADDR, sizeof(ec_eeprom_config_t));
#ifdef EC_DEBUG
  ec_eeprom_config_reseted = true;
#endif /* EC_DEBUG  */
}

void ec_config_init(void) {
  eeprom_read_block(&ec_eeprom_config, (void*)VIA_EEPROM_CUSTOM_CONFIG_USER_ADDR, sizeof(ec_eeprom_config_t));
  int16_t result = is_eeprom_valid();
#ifdef EC_DEBUG
  ec_eeprom_config_error = result;
#endif
  if (result != 0) {
    ec_config_reset();
  }
  MATRIX_LOOP(ec_config_update_key(row, col);)
}

void ec_config_update_key(uint8_t row, uint8_t col) {
  update_key(&ec_config_keys[row][col], get_key_preset(row, col), ec_eeprom_config.bottoming_reading[row][col]);
}

void ec_config_set_actuation_mode(uint8_t preset_index, ec_actuation_mode_t value) {
  UPDATE_PRESET_PARAM(preset_index, actuation_mode, value)
}
void ec_config_set_release_mode(uint8_t preset_index, ec_release_mode_t value) {
  UPDATE_PRESET_PARAM(preset_index, release_mode, value);
}
void ec_config_set_actuation_threshold(uint8_t preset_index, uint16_t value) {
  UPDATE_PRESET_PARAM(preset_index, actuation_threshold, value);
}
void ec_config_set_release_threshold(uint8_t preset_index, uint16_t value) {
  UPDATE_PRESET_PARAM(preset_index, release_threshold, value);
}
void ec_config_set_actuation_travel(uint8_t preset_index, uint16_t value) {
  UPDATE_PRESET_PARAM(preset_index, actuation_travel, value);
}
void ec_config_set_release_travel(uint8_t preset_index, uint16_t value) {
  UPDATE_PRESET_PARAM(preset_index, release_travel, value);
}
void ec_config_set_deadzone(uint8_t preset_index, uint16_t value) {
  UPDATE_PRESET_PARAM(preset_index, deadzone, value);
}
void ec_config_set_sub_action_enable(uint8_t preset_index, bool value) {
  UPDATE_PRESET_PARAM(preset_index, sub_action_enable, value);
}
void ec_config_set_sub_action_keycode(uint8_t preset_index, uint16_t value) {
  UPDATE_PRESET_PARAM(preset_index, sub_action_keycode, value);
}
void ec_config_set_sub_action_actuation_threshold(uint8_t preset_index, uint16_t value) {
  UPDATE_PRESET_PARAM(preset_index, sub_action_actuation_threshold, value);
}
void ec_config_set_sub_action_release_threshold(uint8_t preset_index, uint16_t value) {
  UPDATE_PRESET_PARAM(preset_index, sub_action_release_threshold, value);
}

void ec_config_set_preset_map(uint8_t preset_map_index) {
  if (ec_eeprom_config.selected_preset_map_index != preset_map_index) {
    ec_eeprom_config.selected_preset_map_index = preset_map_index;
    MATRIX_LOOP(ec_config_update_key(row, col);)
    eeprom_update_word((void*)EC_VIA_EEPROM_PRESET_MAP, ec_eeprom_config.selected_preset_map_index);
  }
}

void ec_config_start_calibration(void) {
  MATRIX_LOOP_WITH_KEY(key->bottoming_calibration_starter = true;)
  bottoming_calibration = true;
}

void ec_config_end_calibration(void) {
  bottoming_calibration = false;
  ec_calibrate_noise_floor();
  MATRIX_LOOP_WITH_KEY(
    if (key->bottoming_calibration_starter) { key->bottoming_calibration_starter = false; } else {
      // bottoming = max - nose / 2
      ec_eeprom_config.bottoming_reading[row][col] -= key->noise / 2;
      ec_config_update_key(row, col);
    })
  eeprom_update_block(&ec_eeprom_config.bottoming_reading[0][0], (void*)EC_VIA_EEPROM_BOTTOMING_READING,
                      MATRIX_COLS * MATRIX_ROWS * 2);
}

void ec_config_send_calibration_data(void) {
  send_string("// clang-format off\nconst uint16_t PROGMEM ec_bottoming_reading_default[MATRIX_ROWS][MATRIX_COLS] = ");
  SEND_C_MATRIX_LOOP(SEND_WORD(ec_eeprom_config.bottoming_reading[row][col]);)
  send_string(";\n// clang-format on\n");
}

void ec_config_send_presets(void) {
  send_string("// clang-format off\nconst ec_preset_t PROGMEM ec_presets_default[EC_NUM_PRESETS] = {\n");
  wait_ms(50);
  for (int preset_index = 0; preset_index < EC_NUM_PRESETS; preset_index++) {
    ec_preset_t* preset = get_preset(preset_index);
    send_string("[");
    if (preset_index >= 10) {
      send_char('0' + preset_index / 10);
    }
    send_char('0' + preset_index % 10);
    send_string("] = {\n");
    switch (preset->actuation_mode) {
      case EC_ACTUATION_MODE_STATIC:
        SEND_C_ENUM_PROP_VALUE(actuation_mode, EC_ACTUATION_MODE_STATIC, true);
        break;
      case EC_ACTUATION_MODE_DYNAMIC:
        SEND_C_ENUM_PROP_VALUE(actuation_mode, EC_ACTUATION_MODE_DYNAMIC, true);
        break;
    }
    SEND_C_ARROW_PROP_VALUE(preset, actuation_threshold, WORD, true);
    SEND_C_ARROW_PROP_VALUE(preset, actuation_travel, WORD, true);
    switch (preset->release_mode) {
      case EC_ACTUATION_MODE_STATIC:
        SEND_C_ENUM_PROP_VALUE(release_mode, EC_ACTUATION_MODE_STATIC, true);
        break;
      case EC_ACTUATION_MODE_DYNAMIC:
        SEND_C_ENUM_PROP_VALUE(release_mod, EC_ACTUATION_MODE_DUNAMIC, true);
        break;
    }
    SEND_C_ARROW_PROP_VALUE(preset, release_threshold, WORD, true);
    SEND_C_ARROW_PROP_VALUE(preset, release_travel, WORD, true);
    SEND_C_ARROW_PROP_VALUE(preset, deadzone, WORD, true);
    SEND_C_ARROW_PROP_VALUE(preset, sub_action_keycode, WORD, true);
    SEND_C_ARROW_PROP_VALUE(preset, sub_action_actuation_threshold, WORD, true);
    SEND_C_ARROW_PROP_VALUE(preset, sub_action_release_threshold, WORD, false);
    send_string("}");
    if (preset_index < (EC_NUM_PRESETS - 1)) {
      send_string(",");
    }
    send_string("\n");
    wait_ms(50);
  }
  send_string("};\n// clang-format on\n");
}

#ifdef EC_DEBUG

void ec_config_debug_send_misc_state(void) {
  send_string("const misc_state = {\n");
#  ifdef DEBUG_MATRIX_SCAN_RATE
  uint32_t matrix_scan_rate = get_matrix_scan_rate();
  SEND_JS_PROP_VALUE(matrix_scan_rate, WORD, true);
#  endif
  SEND_JS_PROP_VALUE(ec_eeprom_config_reseted, BOOL, true);
  SEND_JS_PROP_VALUE(ec_eeprom_config_error, WORD, true);
  SEND_JS_PROP_VALUE(bottoming_calibration, BOOL, true);
  SEND_JS_PROP_NAME("bottoming_calibration_starter");
  SEND_C_MATRIX_LOOP(SEND_BOOL(ec_config_keys[row][col].bottoming_calibration_starter);)
  send_string("\n}\n");
}

void ec_config_debug_send_calibration(void) {
  send_string("const calibrtion = {\n");
  SEND_JS_PROP_NAME("noise");
  SEND_JS_MATRIX_LOOP(SEND_WORD(ec_config_keys[row][col].noise););
  send_string(",\n");
  SEND_JS_PROP_NAME("noise_floor");
  SEND_JS_MATRIX_LOOP(SEND_WORD(ec_config_keys[row][col].noise_floor););
  send_string(",\n");
  SEND_JS_PROP_NAME("bottoming_readiong");
  SEND_JS_MATRIX_LOOP(SEND_WORD(ec_eeprom_config.bottoming_reading[row][col]););
  send_string("\n}\n");
}

void ec_config_debug_send_config_keys(void) {
  send_string("const key_config = {\n");
  SEND_JS_PROP_NAME("actuation_mode");
  SEND_JS_MATRIX_LOOP(SEND_NIBBLE(ec_config_keys[row][col].actuation_mode);)
  send_string(",\n");
  SEND_JS_PROP_NAME("actuation_reference");
  SEND_JS_MATRIX_LOOP(SEND_WORD(ec_config_keys[row][col].actuation_reference);)
  send_string(",\n");
  SEND_JS_PROP_NAME("release_mode");
  SEND_JS_MATRIX_LOOP(SEND_NIBBLE(ec_config_keys[row][col].release_mode);)
  send_string(",\n");
  SEND_JS_PROP_NAME("release_reference");
  SEND_JS_MATRIX_LOOP(SEND_WORD(ec_config_keys[row][col].release_reference);)
  send_string(",\n");
  SEND_JS_PROP_NAME("deadzone");
  SEND_JS_MATRIX_LOOP(SEND_WORD(ec_config_keys[row][col].deadzone);)
  send_string(",\n");
  SEND_JS_PROP_NAME("sub_action_keycode");
  SEND_JS_MATRIX_LOOP(SEND_WORD(ec_config_keys[row][col].sub_action_keycode);)
  send_string(",\n");
  SEND_JS_PROP_NAME("sub_action_actuation_threshold");
  SEND_JS_MATRIX_LOOP(SEND_WORD(ec_config_keys[row][col].sub_action_actuation_threshold);)
  send_string(",\n");
  SEND_JS_PROP_NAME("sub_action_release_threshold");
  SEND_JS_MATRIX_LOOP(SEND_WORD(ec_config_keys[row][col].sub_action_release_threshold);)
  send_string("\n}\n");
}

void ec_config_debug_send_all(void) {
  ec_config_debug_send_misc_state();
  ec_config_debug_send_calibration();
  ec_config_debug_send_config_keys();
}
#endif
