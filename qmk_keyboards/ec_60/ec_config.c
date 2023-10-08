#include "ec_config.h"

#include <eeprom.h>

#include "ec_60.h"
#include "ec_switch_matrix.h"

ec_eeprom_config_t ec_eeprom_config;
ec_key_config_t ec_config_keys[MATRIX_ROWS][MATRIX_COLS];
bool bottoming_calibration;

#ifdef EC_DEBUG
static bool ec_eeprom_config_reseted;
static int8_t ec_eeprom_config_error;
static uint8_t debug_byte_value;
static uint16_t debug_word_value;
#endif /* EC_DEBUG  */

static deferred_token send_data_token;  // defer_exec token

// if defined in ec_60/config.h or ec_60/keymaps/<keymap name>/config.h

// 0 | 100% 10bit
#define SCALE_RANGE 0x3ff
// 0 | 50% 9bit
#define HALF_SCALE_RANGE 0x1ff

static uint8_t get_preset_index(uint8_t row, uint8_t col);
static int is_eeprom_valid(void);
static int is_preset_valid(ec_preset_t* preset);
static void defer_eeprom_update_preset(uint8_t preset_index);
static void update_matrix(uint8_t preset_index);
static ec_preset_t* get_preset_key(uint8_t row, uint8_t col);
static ec_preset_t* get_preset(uint8_t preset_index);
static uint32_t send_calibration_data_cb(uint32_t trigger_time, void* cb_arg);
static uint32_t send_presets_cb(uint32_t trigger_time, void* cb_arg);
static uint32_t send_preset_map_cb(uint32_t trigger_time, void* cb_arg);
#ifdef EC_DEBUG
static uint32_t debug_send_config_cb(uint32_t trigger_time, void* cb_arg);
#endif /* EC_DEBUG  */

#define RESCALE_TRAVEL(noise_floor, bottoming, x) (x * (bottoming - noise_floor) / SCALE_RANGE)
#define RESCALE_THRESHOLD(noise_floor, bottoming, x) (RESCALE_TRAVEL(noise_floor, bottoming, x) + noise_floor)

void ec_config_reset(void) {
  // Default values
  for (uint8_t preset_index = 0; preset_index < EC_NUM_PRESETS; preset_index++) {
    ec_preset_t* preset = get_preset(preset_index);
    // I don't want to lose presets for each update firmware
    memcpy_P(preset, &ec_presets_default[preset_index], sizeof(ec_preset_t));
    if (is_preset_valid(preset) != 0) {
      preset->actuation_mode = EC_ACTUATION_MODE_STATIC;
      preset->release_mode = EC_RELEASE_MODE_STATIC;
      preset->actuation_threshold = EC_ACTUATION_THRESHOLD_DEFAULT;
      preset->release_threshold = EC_RELEASE_THRESHOLD_DEFAULT;
      preset->actuation_travel = EC_ACTUATION_TRAVEL_DEFAULT;
      preset->release_travel = EC_RELEASE_TRAVEL_DEFAULT;
      preset->deadzone = EC_DEADZONE_DEFAULT;
    }
  }
  for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
    for (uint8_t col = 0; col < MATRIX_COLS; col++) {
      // I don't want to lose calibration data for each update firmware
      ec_eeprom_config.bottoming_reading[row][col] = pgm_read_word(&ec_bottoming_reading_default[row][col]);
    }
  }
  memcpy_P(&ec_eeprom_config.bottoming_reading[0][0], &ec_bottoming_reading_default[0][0],
           MATRIX_ROWS * MATRIX_COLS * 2);
  ec_eeprom_config.preset_map = 0;

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
  for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
    for (uint8_t col = 0; col < MATRIX_COLS; col++) {
      ec_config_update_key(row, col);
    }
  }
}

void ec_config_update_key(uint8_t row, uint8_t col) {
  ec_preset_t* preset = get_preset_key(row, col);
  ec_key_config_t* key = &ec_config_keys[row][col];
  uint16_t bottoming = ec_eeprom_config.bottoming_reading[row][col];
  uint8_t prev_actuation_mode = key->actuation_mode;
  uint8_t prev_release_mode = key->release_mode;
  // actuation
  key->actuation_mode = preset->actuation_mode;
  switch (preset->actuation_mode) {
    case EC_ACTUATION_MODE_STATIC:
      key->actuation_reference = RESCALE_THRESHOLD(key->noise_floor, bottoming, preset->actuation_threshold);
      break;
    case EC_ACTUATION_MODE_DYNAMIC:
      key->actuation_reference = RESCALE_TRAVEL(key->noise_floor, bottoming, preset->actuation_threshold);
      break;
  }
  // release
  key->release_mode = preset->release_mode;
  switch (preset->release_mode) {
    case EC_RELEASE_MODE_STATIC:
      key->release_reference = RESCALE_THRESHOLD(key->noise_floor, bottoming, preset->release_threshold);
      break;
    case EC_RELEASE_MODE_DYNAMIC:
      key->release_reference = RESCALE_TRAVEL(key->noise_floor, bottoming, preset->release_threshold);
      break;
  }
  // deadzone
  key->deadzone = RESCALE_THRESHOLD(key->noise_floor, bottoming, preset->deadzone);

  if (prev_actuation_mode != preset->actuation_mode || prev_release_mode != preset->release_mode) {
    key->extremum = key->noise_floor;
  }
  if (key->extremum < key->deadzone) {
    key->extremum = key->deadzone;
  }
}

void ec_config_set_actuation_mode(uint8_t preset_index, ec_actuation_mode_t actuation_mode) {
  ec_preset_t* preset = get_preset(preset_index);
  if (preset->actuation_mode != actuation_mode) {
    preset->actuation_mode = actuation_mode;
    update_matrix(preset_index);
    defer_eeprom_update_preset(preset_index);
  }
}

void ec_config_set_release_mode(uint8_t preset_index, ec_release_mode_t release_mode) {
  ec_preset_t* preset = get_preset(preset_index);
  if (preset->release_mode != release_mode) {
    preset->release_mode = release_mode;
    update_matrix(preset_index);
    defer_eeprom_update_preset(preset_index);
  }
}
void ec_config_set_actuation_threshold(uint8_t preset_index, uint16_t actuation_threshold) {
  ec_preset_t* preset = get_preset(preset_index);
  if (preset->actuation_threshold != actuation_threshold) {
    preset->actuation_threshold = actuation_threshold;
    update_matrix(preset_index);
    defer_eeprom_update_preset(preset_index);
  }
}

void ec_config_set_release_threshold(uint8_t preset_index, uint16_t release_threshold) {
  ec_preset_t* preset = get_preset(preset_index);
  if (preset->release_threshold != release_threshold) {
    preset->release_threshold = release_threshold;
    update_matrix(preset_index);
    defer_eeprom_update_preset(preset_index);
  }
}

void ec_config_set_actuation_travel(uint8_t preset_index, uint16_t actuation_travel) {
  ec_preset_t* preset = get_preset(preset_index);
  if (preset->actuation_travel != actuation_travel) {
    preset->actuation_travel = actuation_travel;
    update_matrix(preset_index);
    defer_eeprom_update_preset(preset_index);
  }
}

void ec_config_set_release_travel(uint8_t preset_index, uint16_t release_travel) {
  ec_preset_t* preset = get_preset(preset_index);
  if (preset->release_travel != release_travel) {
    preset->release_travel = release_travel;
    update_matrix(preset_index);
    defer_eeprom_update_preset(preset_index);
  }
}
void ec_config_set_deadzone(uint8_t preset_index, uint16_t deadzone) {
  ec_preset_t* preset = get_preset(preset_index);
  if (preset->deadzone != deadzone) {
    preset->deadzone = deadzone;
    update_matrix(preset_index);
    defer_eeprom_update_preset(preset_index);
  }
}

void ec_config_set_preset_map(uint8_t preset_map_index) {
  if (ec_eeprom_config.preset_map != preset_map_index) {
    ec_eeprom_config.preset_map = preset_map_index;
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
      for (uint8_t col = 0; col < MATRIX_COLS; col++) {
        ec_config_update_key(row, col);
      }
    }
    eeprom_update_word((void*)EC_VIA_EEPROM_PRESET_MAP, ec_eeprom_config.preset_map);
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

void ec_config_send_calibration_data(uint32_t delay_ms) {
  send_data_token = defer_exec(delay_ms, &send_calibration_data_cb, NULL);
}

void ec_config_send_presets(uint32_t delay_ms) { send_data_token = defer_exec(delay_ms, &send_presets_cb, NULL); }

void ec_config_send_preset_map(uint32_t delay_ms) { send_data_token = defer_exec(delay_ms, &send_preset_map_cb, NULL); }

#ifdef EC_DEBUG
void ec_config_debug_send_config(uint32_t delay_ms) {
  send_data_token = defer_exec(delay_ms, &debug_send_config_cb, NULL);
}
#endif

//
// sttatic routines
//-----------------------------------------------------

static uint8_t get_preset_index(uint8_t row, uint8_t col) {
  uint16_t keycode = dynamic_keymap_get_keycode(EC_PRESET_MAP(ec_eeprom_config.preset_map), row, col);
  if (keycode >= EC_PRESET_START && keycode <= EC_PRESET_END) {
    return keycode - EC_PRESET_START;
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
  if (ec_eeprom_config.preset_map >= EC_NUM_PRESET_MAPS) return -10;
  return 0;
}

static int is_preset_valid(ec_preset_t* preset) {
  if (preset->actuation_mode > EC_ACTUATION_MODE_DYNAMIC) return -1;
  if (preset->release_mode > EC_RELEASE_MODE_DYNAMIC) return -2;
  if (preset->actuation_threshold > SCALE_RANGE) return -3;
  if (preset->release_threshold > SCALE_RANGE) return -4;
  if (preset->actuation_travel > HALF_SCALE_RANGE) return -5;
  if (preset->release_travel > HALF_SCALE_RANGE) return -6;
  if (preset->deadzone > HALF_SCALE_RANGE) return -7;

  // EC_PRESETS_DEFAUL_USER may not have bean defined all presets.
  if (preset->actuation_threshold == 0 && preset->actuation_travel == 0 && preset->release_threshold == 0 &&
      preset->release_travel == 0 && preset->deadzone == 0) {
    return -8;
  }
  return 0;
}

static void defer_eeprom_update_preset(uint8_t preset_index) {
  defer_eeprom_update_block(EC_VIA_CUSTOM_CHANNEL_ID_START + preset_index, 0, &ec_eeprom_config.presets[preset_index],
                            (void*)(EC_VIA_EEPROM_PRESETS + sizeof(ec_preset_t) * preset_index), sizeof(ec_preset_t));
}

static void update_matrix(uint8_t preset_index) { MATRIX_LOOP(ec_config_update_key(row, col);) }

static ec_preset_t* get_preset_key(uint8_t row, uint8_t col) { return get_preset(get_preset_index(row, col)); }

static ec_preset_t* get_preset(uint8_t preset_index) { return &(ec_eeprom_config.presets[preset_index]); }

static uint32_t send_calibration_data_cb(uint32_t trigger_time, void* cb_arg) {
  send_data_token = 0;
  send_string(
    "// clang-format off\nconst uint16_t PROGMEM ec_bottoming_reading_default[MATRIX_ROWS][MATRIX_COLS] = {\n");
  wait_ms(50);
  for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
    send_string("{");
    for (uint8_t col = 0; col < MATRIX_COLS; col++) {
      send_string("0x");
      send_word(ec_eeprom_config.bottoming_reading[row][col]);
      if (col < (MATRIX_COLS - 1)) {
        send_string(",");
      }
    }
    send_string("}");
    if (row < (MATRIX_ROWS - 1)) {
      send_string(",");
    }
    send_string(" \n");
    wait_ms(50);
  }
  send_string("};\n// clang-format on\n");
  return 0;
}

static void send_preset_value(char* name, uint16_t value, uint16_t defaultValue) {
  send_string(", \n.");
  send_string(name);
  send_string(" = ");
  if (value == defaultValue) {
    send_string("EC_");
    for (char* p = name; *p; p++) {
      char c = *p;
      if (*p >= 'a' && *p <= 'z') c -= 0x20;
      send_char(c);
    }
    send_string("_DEFAULT");
  } else {
    send_string("0x");
    send_word(value);
  }
}

static uint32_t send_presets_cb(uint32_t trigger_time, void* cb_arg) {
  send_string("// clang-format off\nconst ec_preset_t PROGMEM ec_presets_default[EC_NUM_PRESETS] = {");
  wait_ms(50);
  for (int preset_index = 0; preset_index < EC_NUM_PRESETS; preset_index++) {
    ec_preset_t* preset = get_preset(preset_index);
    send_string(" \n[");
    send_nibble(preset_index);
    send_string("] = {");
    send_string(" \n.actuation_mode = EC_ACTUATION_MODE_");
    send_string(preset->actuation_mode == 0 ? "STATIC" : "DYNAMIC");
    send_preset_value("actuation_threshold", preset->actuation_threshold, EC_ACTUATION_THRESHOLD_DEFAULT);
    send_preset_value("actuation_travel", preset->actuation_travel, EC_ACTUATION_TRAVEL_DEFAULT);
    send_string(", \n.release_mode = EC_RELEASE_MODE_");
    send_string(preset->release_mode == 0 ? "STATIC" : "DYNAMIC");
    send_preset_value("release_threshold", preset->release_threshold, EC_RELEASE_THRESHOLD_DEFAULT);
    send_preset_value("release_travel", preset->release_travel, EC_RELEASE_TRAVEL_DEFAULT);
    send_preset_value("deadzone", preset->deadzone, EC_DEADZONE_DEFAULT);
    if (preset_index < (EC_NUM_PRESETS - 1)) {
      send_string(" \n},");
    } else {
      send_string(" \n}");
    }
    wait_ms(50);
  }
  send_string(" \n};\n// clang-format on\n");
  return 0;
}

static uint32_t send_preset_map_cb(uint32_t trigger_time, void* cb_arg) {
  // TODO matrix/keymap conversion
  return 0;
}

#ifdef EC_DEBUG

typedef enum {
  _sw_value = 0,
  _bottoming_reading,
  _actuation_mode,
  _actuation_reference,
  _release_mode,
  _release_reference,
  _deadzone,
  _noise,
  _noise_floor,
  _calibration_starter
} matrix_array_type_t;
// type 0
void _send_matrix_array(matrix_array_type_t type) {
  wait_ms(50);
  send_string("[\n");
  for (int row = 0; row < MATRIX_ROWS; row++) {
    send_string("[");
    for (int col = 0; col < MATRIX_COLS; col++) {
      ec_key_config_t* key = &ec_config_keys[row][col];
      switch (type) {
        case _sw_value:
          send_string("0x");
          send_word(key->sw_value);
          break;
        case _bottoming_reading:
          send_string("0x");
          send_word(ec_eeprom_config.bottoming_reading[row][col]);
          break;
        case _actuation_mode:
          send_nibble(key->actuation_mode);
          break;
        case _actuation_reference:
          send_string("0x");
          send_word(key->actuation_reference);
          break;
        case _release_mode:
          send_nibble(key->release_mode);
          break;
        case _release_reference:
          send_string("0x");
          send_word(key->release_reference);
          break;
        case _deadzone:
          send_string("0x");
          send_word(key->deadzone);
          break;
        case _noise:
          send_string("0x");
          send_word(key->noise);
          break;
        case _noise_floor:
          send_string("0x");
          send_word(key->noise_floor);
          break;
        case _calibration_starter:
          send_string(key->bottoming_calibration_starter ? "true" : "false");
          break;
      }
      if (col < (MATRIX_COLS - 1)) {
        send_string(",");
      }
    }
    send_string("]");
    if (row < (MATRIX_ROWS - 1)) {
      send_string(",");
    }
    send_string("\n");
    wait_ms(50);
  }
  send_string("]");
}

static uint32_t debug_send_config_cb(uint32_t trigger_time, void* cb_arg) {
  send_data_token = 0;
  send_string("const data = {\nec_eeprom_config_reseted: ");
  send_string(ec_eeprom_config_reseted ? "true" : "false");
  send_string(",\nec_eeprom_config_error: 0x");
  send_word(ec_eeprom_config_error);
  send_string(",\ndebug_byte_value: 0x");
  send_byte(debug_byte_value);
  send_string(",\ndebug_word_value: 0x");
  send_word(debug_word_value);
  send_string(",\nbottoming_calibration: ");
  send_string(bottoming_calibration ? "true" : "false");
  send_string(",\nsw_value: ");
  _send_matrix_array(_sw_value);
  send_string(",\nec_eeprom_config: {\npresets: [\n");
  for (uint8_t i = 0; i < EC_NUM_PRESETS; i++) {
    ec_preset_t* preset = get_preset(i);
    send_string("{\nactuation_mode: ");
    send_nibble(preset->actuation_mode);
    send_string(",\nrelease_mode: ");
    send_nibble(preset->release_mode);
    send_string(",\nactuation_threshold: 0x");
    send_word(preset->actuation_threshold);
    send_string(",\nrelease_threshold: 0x");
    send_word(preset->release_threshold);
    send_string(",\nactuation_travel: 0x");
    send_word(preset->actuation_travel);
    send_string(",\nrelease_travel: 0x");
    send_word(preset->release_travel);
    send_string(",\ndeadzone: 0x");
    send_word(preset->deadzone);
    send_string("\n}");
    if (i < (EC_NUM_PRESETS - 1)) {
      send_string(",");
    }
    send_string("\n");
    wait_ms(100);
  }
  send_string("],\nbottoming_reading: ");
  _send_matrix_array(_bottoming_reading);
  send_string(",\npreset_map: ");
  send_nibble(ec_eeprom_config.preset_map);
  // end eepro_ec_config
  send_string("\n}");

  // start ec_config
  send_string(",\nec_config: {\n");

  send_string("actuation_mode: ");
  _send_matrix_array(_actuation_mode);
  send_string(",\nactuation_reference: ");
  _send_matrix_array(_actuation_reference);
  send_string(",\nrelease_mode: ");
  _send_matrix_array(_release_mode);
  send_string(",\nrelease_reference: ");
  _send_matrix_array(_release_reference);
  send_string(",\ndeadzone: ");
  _send_matrix_array(_deadzone);
  send_string(",\nnoise: ");
  _send_matrix_array(_noise);
  send_string(",\nnoise_floor: ");
  _send_matrix_array(_noise_floor);

  send_string(",\nbottoming_calibration_starter: ");
  _send_matrix_array(_calibration_starter);
  // end eepro_ec_config
  send_string("\n}\n}\n");
  return 0;
}
#endif /* EC_DEBUG */
