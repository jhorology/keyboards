#include "ec_config.h"

#include <eeprom.h>

#include "ec_60.h"
#include "ec_switch_matrix.h"

ec_config_t ec_config;
eeprom_ec_config_t eeprom_ec_config;

#ifdef EC_DEBUG
static bool eeprom_ec_config_reseted;
static int8_t eeprom_ec_config_error;
static uint8_t debug_byte_value;
static uint16_t debug_word_value;
#endif /* EC_DEBUG  */

static deferred_token send_data_token;  // defer_exec token

// if defined in ec_60/config.h or ec_60/keymaps/<keymap name>/config.h
#ifdef EC_BOTTOMING_READING_DEFAULT_USER
const uint16_t PROGMEM bottming_reading_default[MATRIX_ROWS][MATRIX_COLS] = EC_BOTTOMING_READING_DEFAULT_USER;
#endif

// 0 | 100% 10bit
#define SCALE_RANGE 0x3ff
// 0 | 50% 9bit
#define HALF_SCALE_RANGE 0x1ff

static uint8_t get_preset_index(uint8_t row, uint8_t col);
static int is_eeprom_valid(void);
static void defer_eeprom_update_preset(uint8_t preset_index, uint8_t raw_index);
static void update_matrix(uint8_t preset_index);
static uint16_t rescale(uint8_t row, uint8_t col, uint16_t x);
static ec_preset_t* get_preset_key(uint8_t row, uint8_t col);
static ec_preset_t* get_preset(uint8_t preset_index);
static uint32_t send_calibration_data_cb(uint32_t trigger_time, void* cb_arg);
#ifdef EC_DEBUG
static uint32_t debug_send_config_cb(uint32_t trigger_time, void* cb_arg);
#endif /* EC_DEBUG  */

void ec_config_reset(void) {
  // Default values
  for (uint8_t index = 0; index < EC_NUM_PRESETS; index++) {
    ec_preset_t* preset = get_preset(index);
    preset->actuation_mode = EC_ACTUATION_MODE_STATIC;
    preset->release_mode = EC_RELEASE_MODE_STATIC;
    preset->actuation_threshold = EC_ACTUATION_THRESHOLD_DEFAULT;
    preset->release_threshold = EC_RELEASE_THRESHOLD_DEFAULT;
    preset->actuation_travel = EC_ACTUATION_TRAVEL_DEFAULT;
    preset->release_travel = EC_RELEASE_TRAVEL_DEFAULT;
    preset->deadzone = EC_DEADZONE_DEFAULT;
  }
  for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
    for (uint8_t col = 0; col < MATRIX_COLS; col++) {
      // I don't want to lose calibration data for each update firware
#ifdef EC_BOTTOMING_READING_DEFAULT_USER
      eeprom_ec_config.bottoming_reading[row][col] = pgm_read_word(&bottming_reading_default[row][col]);
#else
      eeprom_ec_config.bottoming_reading[row][col] = EC_BOTTOMING_READING_DEFAULT;
#endif
    }
  }
  // Write default value to EEPROM now
  eeprom_update_block(&eeprom_ec_config, (void*)VIA_EEPROM_CUSTOM_CONFIG_USER_ADDR, sizeof(eeprom_ec_config_t));
#ifdef EC_DEBUG
  eeprom_ec_config_reseted = true;
#endif /* EC_DEBUG  */
}

void ec_config_init(void) {
  eeprom_read_block(&eeprom_ec_config, (void*)VIA_EEPROM_CUSTOM_CONFIG_USER_ADDR, sizeof(eeprom_ec_config_t));
  int16_t result = is_eeprom_valid();
#ifdef EC_DEBUG
  eeprom_ec_config_error = result;
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
  uint8_t prev_actuation_mode = ec_config.actuation[row][col].reference.mode;
  uint8_t prev_release_mode = ec_config.release[row][col].reference.mode;
  // actuation
  ec_config.actuation[row][col].reference.mode = preset->actuation_mode;
  switch (preset->actuation_mode) {
    case EC_ACTUATION_MODE_STATIC:
      ec_config.actuation[row][col].reference.value = rescale(row, col, preset->actuation_threshold);
      break;
    case EC_ACTUATION_MODE_DYNAMIC:
      ec_config.actuation[row][col].reference.value = rescale(row, col, preset->actuation_travel);
      break;
  }
  // release
  ec_config.release[row][col].reference.mode = preset->release_mode;
  switch (preset->release_mode) {
    case EC_RELEASE_MODE_STATIC:
      ec_config.release[row][col].reference.value = rescale(row, col, preset->release_threshold);
      break;
    case EC_RELEASE_MODE_DYNAMIC:
      ec_config.release[row][col].reference.value = rescale(row, col, preset->release_travel);
      break;
  }
  // deadzone
  ec_config.deadzone[row][col] = rescale(row, col, preset->deadzone);

  if (prev_actuation_mode != preset->actuation_mode || prev_release_mode != preset->release_mode) {
    ec_config.extremum[row][col] = sw_value[row][col];
  }
  if (ec_config.extremum[row][col] < ec_config.deadzone[row][col]) {
    ec_config.extremum[row][col] = ec_config.deadzone[row][col];
  }
}

void ec_config_set_actuation_mode(uint8_t preset_index, ec_actuation_mode_t actuation_mode) {
  ec_preset_t* preset = get_preset(preset_index);
  if (preset->actuation_mode != actuation_mode) {
    preset->actuation_mode = actuation_mode;
    update_matrix(preset_index);
    defer_eeprom_update_preset(preset_index, 0);
  }
}

void ec_config_set_release_mode(uint8_t preset_index, ec_release_mode_t release_mode) {
  ec_preset_t* preset = get_preset(preset_index);
  if (preset->release_mode != release_mode) {
    preset->release_mode = release_mode;
    update_matrix(preset_index);
    defer_eeprom_update_preset(preset_index, 0);
  }
}
void ec_config_set_actuation_threshold(uint8_t preset_index, uint16_t actuation_threshold) {
  ec_preset_t* preset = get_preset(preset_index);
  if (preset->actuation_threshold != actuation_threshold) {
    eeprom_ec_config.presets[preset_index].preset.actuation_threshold = actuation_threshold;
    update_matrix(preset_index);
    defer_eeprom_update_preset(preset_index, 0);
  }
}

void ec_config_set_release_threshold(uint8_t preset_index, uint16_t release_threshold) {
  ec_preset_t* preset = get_preset(preset_index);
  if (preset->release_threshold != release_threshold) {
    preset->release_threshold = release_threshold;
    update_matrix(preset_index);
    defer_eeprom_update_preset(preset_index, 0);
  }
}

void ec_config_set_actuation_travel(uint8_t preset_index, uint16_t actuation_travel) {
  ec_preset_t* preset = get_preset(preset_index);
  if (preset->actuation_travel != actuation_travel) {
    preset->actuation_travel = actuation_travel;
    update_matrix(preset_index);
    defer_eeprom_update_preset(preset_index, 1);
  }
}

void ec_config_set_release_travel(uint8_t preset_index, uint16_t release_travel) {
  ec_preset_t* preset = get_preset(preset_index);
  if (preset->release_travel != release_travel) {
    preset->release_travel = release_travel;
    update_matrix(preset_index);
    defer_eeprom_update_preset(preset_index, 1);
  }
}
void ec_config_set_deadzone(uint8_t preset_index, uint16_t deadzone) {
  ec_preset_t* preset = get_preset(preset_index);
  if (preset->deadzone != deadzone) {
    preset->deadzone = deadzone;
    update_matrix(preset_index);
    defer_eeprom_update_preset(preset_index, 1);
  }
}
void ec_config_start_calibration(void) {
  for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
    for (uint8_t col = 0; col < MATRIX_COLS; col++) {
      ec_config.bottoming_calibration_starter[row][col] = true;
    }
  }
  ec_config.bottoming_calibration = true;
}
void ec_config_end_calibration(void) {
  ec_config.bottoming_calibration = false;
  for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
    for (uint8_t col = 0; col < MATRIX_COLS; col++) {
      if (ec_config.bottoming_calibration_starter[row][col]) {
        ec_config.bottoming_calibration_starter[row][col] = false;
      } else {
        ec_config_update_key(row, col);
      }
    }
  }
  eeprom_update_block(&eeprom_ec_config.bottoming_reading[0][0], (void*)EC_VIA_EEPROM_BOTTOMING_READING,
                      MATRIX_COLS * MATRIX_ROWS * 2);
}

void ec_config_send_calibration_data(uint32_t delay_ms) {
  send_data_token = defer_exec(delay_ms, &send_calibration_data_cb, NULL);
}

#ifdef EC_DEBUG
void ec_config_debug_send_config(uint32_t delay_ms) {
  send_data_token = defer_exec(delay_ms, &debug_send_config_cb, NULL);
}
#endif

//
// sttatic routines
//-----------------------------------------------------

static uint8_t get_preset_index(uint8_t row, uint8_t col) {
  uint16_t keycode = dynamic_keymap_get_keycode(EC_PRESET_MAP_LAYER, row, col);
  if (keycode >= EC_PRESET_START && keycode <= EC_PRESET_END) {
    return keycode - EC_PRESET_START;
  }
  return 0;
}

static int is_eeprom_valid(void) {
  for (uint8_t index = 0; index < EC_NUM_PRESETS; index++) {
    ec_preset_t* preset = get_preset(index);
    if (preset->actuation_mode > EC_ACTUATION_MODE_DYNAMIC) return -1;
    if (preset->release_mode > EC_RELEASE_MODE_DYNAMIC) return -2;
    if (preset->actuation_threshold > SCALE_RANGE) return -3;
    if (preset->release_threshold > SCALE_RANGE) return -4;
    if (preset->actuation_travel > HALF_SCALE_RANGE) return -5;
    if (preset->release_travel >= HALF_SCALE_RANGE) return -6;
    if (preset->deadzone >= HALF_SCALE_RANGE) return -7;
  }
  for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
    for (uint8_t col = 0; col < MATRIX_COLS; col++) {
      if (eeprom_ec_config.bottoming_reading[row][col] <= ec_config.noise_floor[row][col] ||
          eeprom_ec_config.bottoming_reading[row][col] > 0xfff)
        return -8;
    }
  }
  return 0;
}

static void defer_eeprom_update_preset(uint8_t preset_index, uint8_t raw_index) {
  defer_eeprom_update_dword(EC_VIA_CUSTOM_CHANNEL_ID_START + preset_index, raw_index,
                            (void*)(EC_VIA_EEPROM_PRESETS + sizeof(ec_preset_t) * preset_index),
                            eeprom_ec_config.presets[preset_index].raw[raw_index]);
}

static void update_matrix(uint8_t preset_index) {
  for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
    for (uint8_t col = 0; col < MATRIX_COLS; col++) {
      if (get_preset_index(row, col) == preset_index) {
        ec_config_update_key(row, col);
      }
    }
  }
}

static uint16_t rescale(uint8_t row, uint8_t col, uint16_t x) {
  uint16_t out_max = eeprom_ec_config.bottoming_reading[row][col];
  uint16_t out_min = ec_config.noise_floor[row][col];
  return x * (out_max - out_min) / SCALE_RANGE + out_min;
}

static ec_preset_t* get_preset_key(uint8_t row, uint8_t col) { return get_preset(get_preset_index(row, col)); }

static ec_preset_t* get_preset(uint8_t preset_index) { return &(eeprom_ec_config.presets[preset_index].preset); }

static uint32_t send_calibration_data_cb(uint32_t trigger_time, void* cb_arg) {
  send_data_token = 0;
  send_string("// clang-format off\n#define EC_BOTTOMING_READING_DEFAULT_USER { \\\n");
  wait_ms(100);
  for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
    send_string("{");
    for (uint8_t col = 0; col < MATRIX_COLS; col++) {
      send_string("0x");
      send_word(eeprom_ec_config.bottoming_reading[row][col]);
      if (col < (MATRIX_COLS - 1)) {
        send_string(",");
      }
    }
    send_string("}");
    if (row < (MATRIX_ROWS - 1)) {
      send_string(",");
    }
    send_string(" \\\n");
    wait_ms(100);
  }
  send_string("}\n// clang-format on\n");
  return 0;
}

#ifdef EC_DEBUG

typedef enum {
  _sw_value = 0,
  _bottoming_reading,
  _actuation_mode,
  _actuation_value,
  _release_mode,
  _release_value,
  _deadzone,
  _noise_floor,
  _calibration_starter
} matrix_array_type_t;
// type 0
void _send_matrix_array(matrix_array_type_t type) {
  wait_ms(100);
  send_string("[\n");
  for (int row = 0; row < MATRIX_ROWS; row++) {
    send_string("[");
    for (int col = 0; col < MATRIX_COLS; col++) {
      switch (type) {
        case _sw_value:
          send_string("0x");
          send_word(sw_value[row][col]);
          break;
        case _bottoming_reading:
          send_string("0x");
          send_word(eeprom_ec_config.bottoming_reading[row][col]);
          break;
        case _actuation_mode:
          send_nibble(ec_config.actuation[row][col].reference.mode);
          break;
        case _actuation_value:
          send_string("0x");
          send_word(ec_config.actuation[row][col].reference.value);
          break;
        case _release_mode:
          send_nibble(ec_config.release[row][col].reference.mode);
          break;
        case _release_value:
          send_string("0x");
          send_word(ec_config.release[row][col].reference.value);
          break;
        case _deadzone:
          send_string("0x");
          send_word(ec_config.deadzone[row][col]);
          break;
        case _noise_floor:
          send_string("0x");
          send_word(ec_config.noise_floor[row][col]);
          break;
        case _calibration_starter:
          send_string(ec_config.bottoming_calibration_starter[row][col] ? "true" : "false");
          break;
      }
      if (col < (MATRIX_COLS - 1)) {
        send_string(",");
      }
      wait_ms(100);
    }
    send_string("]");
    if (row < (MATRIX_ROWS - 1)) {
      send_string(",");
    }
    send_string("\n");
  }
  send_string("]");
}

static uint32_t debug_send_config_cb(uint32_t trigger_time, void* cb_arg) {
  send_data_token = 0;
  send_string("const data = {\neeprom_ec_config_reseted: ");
  send_string(eeprom_ec_config_reseted ? "true" : "false");
  send_string(",\neeprom_ec_config_error: 0x");
  send_word(eeprom_ec_config_error);
  send_string(",\ndebug_byte_value: 0x");
  send_byte(debug_byte_value);
  send_string(",\ndebug_word_value: 0x");
  send_word(debug_word_value);
  send_string(",\nsw_value: ");
  _send_matrix_array(_sw_value);
  send_string(",\neeprom_ec_config: {\npresets: [\n");
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
  // end eepro_ec_config
  send_string("\n}");

  // start ec_config
  send_string(",\nec_config: {\n");

  send_string("actuation_mode: ");
  _send_matrix_array(_actuation_mode);
  send_string(",\nactuation_value: ");
  _send_matrix_array(_actuation_value);
  send_string(",\nrelease_mode: ");
  _send_matrix_array(_release_mode);
  send_string(",\nrelease_value: ");
  _send_matrix_array(_release_value);
  send_string(",\ndeadzone: ");
  _send_matrix_array(_deadzone);
  send_string(",\nnoise_floor: ");
  _send_matrix_array(_noise_floor);

  send_string(",\nbottoming_calibration: ");
  send_string(ec_config.bottoming_calibration ? "true" : "false");
  send_string(",\nbottoming_calibration_starter: ");
  _send_matrix_array(_calibration_starter);
  // end eepro_ec_config
  send_string("\n}\n}\n");
  return 0;
}
#endif /* EC_DEBUG */
