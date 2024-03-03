#include "ec_config.h"

#include <eeprom.h>

#include "ec_60.h"
#include "ec_switch_matrix.h"
#include "lib/send_string_macro.h"

ec_eeprom_config_t ec_eeprom_config;
ec_key_config_t ec_config_keys[MATRIX_ROWS][MATRIX_COLS];

#ifdef EC_DEBUG_ENABLE
bool ec_matrix_scan_test_enable;
ec_test_result_t ec_test_result[EC_TEST_CHARGE_PLOT_COUNT][EC_TEST_DISCHARGE_PLOT_COUNT];

static bool ec_eeprom_config_reseted;
static int8_t ec_eeprom_config_error;
#endif

#define KEY_TRAVEL(noise_floor, bottoming, x) \
  ((uint32_t)x * (bottoming - noise_floor) / EC_SCALE_RANGE)
#define KEY_THRESHOLD(noise_floor, bottoming, x) \
  (KEY_TRAVEL(noise_floor, bottoming, x) + noise_floor)

#define IMPLEMENT_PRESET_PARAM_SETTER(member, param_type)               \
  void ec_config_set_##member(uint8_t preset_index, param_type value) { \
    ec_preset_t *preset = get_preset(preset_index);                     \
    if (preset->member != value) {                                      \
      preset->member = value;                                           \
      update_matrix(preset_index);                                      \
      defer_eeprom_update_preset(preset_index);                         \
    }                                                                   \
  }

static uint8_t get_key_preset_index(uint8_t row, uint8_t col);

//  inline functions
// -----------------------------------------------------------------------------------
static inline ec_preset_t *get_preset(uint8_t preset_index) {
  return &(ec_eeprom_config.presets[preset_index]);
}

static inline ec_preset_t *get_key_preset(uint8_t row, uint8_t col) {
  return get_preset(get_key_preset_index(row, col));
}

static inline void defer_eeprom_update_preset(uint8_t preset_index) {
  defer_eeprom_update_block(
    EC_VIA_CUSTOM_CHANNEL_ID_START + preset_index, 0, &ec_eeprom_config.presets[preset_index],
    (void *)(EC_VIA_EEPROM_PRESETS + sizeof(ec_preset_t) * preset_index), sizeof(ec_preset_t));
}

//  static routine
// -----------------------------------------------------------------------------------
static uint8_t get_key_preset_index(uint8_t row, uint8_t col) {
  uint16_t keycode =
    dynamic_keymap_get_keycode(EC_PRESET_MAP(ec_eeprom_config.selected_preset_map_index), row, col);
  if (keycode >= EC_PRESET_START && keycode <= EC_PRESET_END) {
    return keycode - EC_PRESET_START;
  }
  return 0;
}

static int is_preset_valid(ec_preset_t *preset) {
  if (preset->actuation_mode > EC_ACTUATION_MODE_DYNAMIC) return -1;
  if (preset->release_mode > EC_RELEASE_MODE_DYNAMIC) return -2;
  if (!EC_WITHIN_SCALE_RANGE(preset->actuation_threshold)) return -3;
  if (!EC_WITHIN_SCALE_RANGE(preset->release_threshold)) return -4;
  if (!EC_WITHIN_HALF_SCALE_RANGE(preset->actuation_travel)) return -5;
  if (!EC_WITHIN_HALF_SCALE_RANGE(preset->release_travel)) return -5;
  if (!EC_WITHIN_HALF_SCALE_RANGE(preset->deadzone)) return -5;
  if (!EC_WITHIN_SCALE_RANGE(preset->sub_action_actuation_threshold)) return -8;
  if (!EC_WITHIN_SCALE_RANGE(preset->sub_action_release_threshold)) return -9;
  return 0;
}

static int is_eeprom_valid(void) {
  for (uint8_t index = 0; index < EC_NUM_PRESETS; index++) {
    ec_preset_t *preset = get_preset(index);
    int result = is_preset_valid(preset);
    if (result != 0) return result;
  }
  MATRIX_LOOP(
    if (ec_eeprom_config.bottoming_reading[row][col] <= ec_eeprom_config.noise_floor[row][col] ||
        ec_eeprom_config.bottoming_reading[row][col] > 0x3ff) return -9;)
  if (ec_eeprom_config.selected_preset_map_index >= EC_NUM_PRESET_MAPS) return -10;
  return 0;
}

static void update_key(ec_key_config_t *key, ec_preset_t *preset, uint16_t floor,
                       uint16_t bottoming) {
  ec_actuation_mode_t prev_actuation_mode = key->modes.actuation_mode;
  ec_release_mode_t prev_release_mode = key->modes.release_mode;

  //
  // actuation
  key->modes.actuation_mode = preset->actuation_mode;
  switch (preset->actuation_mode) {
    case EC_ACTUATION_MODE_STATIC_EDGE ... EC_ACTUATION_MODE_STATIC_LEVEL:
      key->actuation_reference = KEY_THRESHOLD(floor, bottoming, preset->actuation_threshold);
      break;
    case EC_ACTUATION_MODE_DYNAMIC:
      key->actuation_reference = KEY_TRAVEL(floor, bottoming, preset->actuation_travel);
      break;
  }
  // release
  key->modes.release_mode = preset->release_mode;
  switch (preset->release_mode) {
    case EC_RELEASE_MODE_STATIC_EDGE ... EC_RELEASE_MODE_STATIC_LEVEL:
      key->release_reference = KEY_THRESHOLD(floor, bottoming, preset->release_threshold);
      break;
    case EC_RELEASE_MODE_DYNAMIC:
      key->release_reference = KEY_TRAVEL(floor, bottoming, preset->release_travel);
      break;
  }
  // deadzone
  key->deadzone = KEY_THRESHOLD(floor, bottoming, preset->deadzone);

  // sub action
  key->sub_action_keycode = preset->sub_action_enable ? preset->sub_action_keycode : KC_NO;
  key->sub_action_actuation_threshold =
    KEY_THRESHOLD(floor, bottoming, preset->sub_action_actuation_threshold);
  key->modes.sub_action_release_mode = preset->sub_action_release_mode;
  key->sub_action_release_threshold =
    KEY_THRESHOLD(floor, bottoming, preset->sub_action_release_threshold);

  // reset extremum
  if (prev_actuation_mode != preset->actuation_mode || prev_release_mode != preset->release_mode) {
    key->extremum = floor;
  }
  if (key->extremum < key->deadzone) {
    key->extremum = key->deadzone;
  }
}

static void update_matrix(uint8_t preset_index) {
  MATRIX_LOOP_WITH_KEY(                                         //
    uint8_t key_preset_index = get_key_preset_index(row, col);  //
    if (preset_index == key_preset_index) {
      update_key(key, get_preset(preset_index), ec_eeprom_config.noise_floor[row][col],
                 ec_eeprom_config.bottoming_reading[row][col]);
    }  //
  )
}

//  export functions
// -----------------------------------------------------------------------------------
void ec_config_reset(void) {
  ec_preset_t preset_default = EC_STATIC_PRESET_DEFAULT;
  // Default values
  for (uint8_t preset_index = 0; preset_index < EC_NUM_PRESETS; preset_index++) {
    ec_preset_t *preset = get_preset(preset_index);
    // I don't want to lose presets for each update firmware
    memcpy_P(preset, &ec_presets_default[preset_index], sizeof(ec_preset_t));
    if (is_preset_valid(preset) != 0) {
      ec_eeprom_config.presets[preset_index] = preset_default;
    }
  }
  memcpy_P(&ec_eeprom_config.bottoming_reading[0][0], &ec_bottoming_reading_default[0][0],
           MATRIX_ROWS * MATRIX_COLS * 2);
  memcpy_P(&ec_eeprom_config.noise_floor[0][0], &ec_noise_floor_default[0][0],
           MATRIX_ROWS * MATRIX_COLS * 2);

  ec_eeprom_config.selected_preset_map_index = 0;

  // Write default value to EEPROM now
  eeprom_update_block(&ec_eeprom_config, (void *)VIA_EEPROM_CUSTOM_CONFIG_USER_ADDR,
                      sizeof(ec_eeprom_config_t));
#ifdef EC_DEBUG_ENABLE
  ec_eeprom_config_reseted = true;
#endif /* EC_DEBUG  */
}

void ec_config_init(void) {
  eeprom_read_block(&ec_eeprom_config, (void *)VIA_EEPROM_CUSTOM_CONFIG_USER_ADDR,
                    sizeof(ec_eeprom_config_t));
  int16_t result = is_eeprom_valid();
#ifdef EC_DEBUG_ENABLE
  ec_eeprom_config_error = result;
#endif
  if (result != 0) {
    ec_config_reset();
  }
  MATRIX_LOOP(ec_config_update_key(row, col);)
}

void ec_config_update_key(uint8_t row, uint8_t col) {
  update_key(&ec_config_keys[row][col], get_key_preset(row, col),
             ec_eeprom_config.noise_floor[row][col], ec_eeprom_config.bottoming_reading[row][col]);
}

IMPLEMENT_PRESET_PARAM_SETTER(actuation_mode, ec_actuation_mode_t)
IMPLEMENT_PRESET_PARAM_SETTER(release_mode, ec_release_mode_t)
IMPLEMENT_PRESET_PARAM_SETTER(actuation_threshold, uint16_t)
IMPLEMENT_PRESET_PARAM_SETTER(release_threshold, uint16_t)
IMPLEMENT_PRESET_PARAM_SETTER(actuation_travel, uint16_t)
IMPLEMENT_PRESET_PARAM_SETTER(release_travel, uint16_t)
IMPLEMENT_PRESET_PARAM_SETTER(deadzone, uint16_t)
IMPLEMENT_PRESET_PARAM_SETTER(sub_action_enable, bool)
IMPLEMENT_PRESET_PARAM_SETTER(sub_action_keycode, uint16_t)
IMPLEMENT_PRESET_PARAM_SETTER(sub_action_actuation_threshold, uint16_t)
IMPLEMENT_PRESET_PARAM_SETTER(sub_action_release_mode, ec_sub_action_release_mode_t)
IMPLEMENT_PRESET_PARAM_SETTER(sub_action_release_threshold, uint16_t)

void ec_config_set_preset_map(uint8_t preset_map_index) {
  if (ec_eeprom_config.selected_preset_map_index != preset_map_index) {
    ec_eeprom_config.selected_preset_map_index = preset_map_index;
    MATRIX_LOOP(ec_config_update_key(row, col);)
    eeprom_update_word((void *)EC_VIA_EEPROM_PRESET_MAP,
                       ec_eeprom_config.selected_preset_map_index);
  }
}

void ec_config_save_calibration_data(void) {
  eeprom_update_block(&ec_eeprom_config.bottoming_reading[0][0],
                      (void *)EC_VIA_EEPROM_BOTTOMING_READING, MATRIX_COLS * MATRIX_ROWS * 2);
  eeprom_update_block(&ec_eeprom_config.noise_floor[0][0], (void *)EC_VIA_EEPROM_NOISE_FLOOR,
                      MATRIX_COLS * MATRIX_ROWS * 2);
}

void ec_config_send_calibration_data(void) {
  send_string("// clang-format off\n");
  send_string("const uint16_t PROGMEM ec_bottoming_reading_default[MATRIX_ROWS][MATRIX_COLS] = ");
  SEND_C_2D_ARRAY(ec_eeprom_config.bottoming_reading, MATRIX_ROWS, MATRIX_COLS, WORD, ";\n");
  send_string("const uint16_t PROGMEM ec_noise_floor_default[MATRIX_ROWS][MATRIX_COLS] = ");
  SEND_C_2D_ARRAY(ec_eeprom_config.noise_floor, MATRIX_ROWS, MATRIX_COLS, WORD, ";\n");
  send_string("// clang-format on\n");
}

void ec_config_send_presets(void) {
  send_string("// clang-format off\n");
  send_string("const ec_preset_t PROGMEM ec_presets_default[EC_NUM_PRESETS] = ");
  SEND_C_INDEXED_ARRAY_CODE(
    EC_NUM_PRESETS, ";\n",
    (
      ec_preset_t *preset = get_preset(i);  //
      switch (preset->actuation_mode) {
        case EC_ACTUATION_MODE_STATIC_EDGE:
          SEND_C_SYMBOL_PROP_SYMBOL_VALUE(actuation_mode, EC_ACTUATION_MODE_STATIC_EDGE, ",\n");
          break;
        case EC_ACTUATION_MODE_STATIC_LEVEL:
          SEND_C_SYMBOL_PROP_SYMBOL_VALUE(actuation_mode, EC_ACTUATION_MODE_STATIC_LEVEL, ",\n");
          break;
        case EC_ACTUATION_MODE_DYNAMIC:
          SEND_C_SYMBOL_PROP_SYMBOL_VALUE(actuation_mode, EC_ACTUATION_MODE_DYNAMIC, ",\n");
          break;
      }  //
      SEND_C_PROP_ARROW_VALUE(preset, actuation_threshold, WORD, ",\n");
      SEND_C_PROP_ARROW_VALUE(preset, actuation_travel, WORD, ",\n");  //
      switch (preset->release_mode) {
        case EC_ACTUATION_MODE_STATIC_EDGE:
          SEND_C_SYMBOL_PROP_SYMBOL_VALUE(release_mode, EC_ACTUATION_MODE_STATIC_EDGE, ",\n");
          break;
        case EC_ACTUATION_MODE_STATIC_LEVEL:
          SEND_C_SYMBOL_PROP_SYMBOL_VALUE(release_mode, EC_ACTUATION_MODE_STATIC_LEVEL, ",\n");
          break;
        case EC_ACTUATION_MODE_DYNAMIC:
          SEND_C_SYMBOL_PROP_SYMBOL_VALUE(release_mod, EC_ACTUATION_MODE_DYNAMIC, ",\n");
          break;
      }  //
      SEND_C_PROP_ARROW_VALUE(preset, release_threshold, WORD, ",\n");
      SEND_C_PROP_ARROW_VALUE(preset, release_travel, WORD, ",\n");
      SEND_C_PROP_ARROW_VALUE(preset, deadzone, WORD, ",\n");
      SEND_C_PROP_ARROW_VALUE(preset, sub_action_keycode, WORD, ",\n");
      SEND_C_PROP_ARROW_VALUE(preset, sub_action_actuation_threshold, WORD, ",\n");
      SEND_C_PROP_ARROW_VALUE(preset, sub_action_release_threshold, WORD, "\n");
      //
      ));
  send_string("// clang-format on\n");
}

#ifdef EC_DEBUG_ENABLE
#  define SEND_EC_CONFIG_KEY_MATRIX(prop, type, term)                      \
    SEND_JS_NAME_PROP_2D_ARRAY_CODE(#prop, MATRIX_ROWS, MATRIX_COLS, term, \
                                    (SEND_##type(ec_config_keys[i][j].prop);));
#  define SEND_EC_CONFIG_KEY_MATRIX_MODE(prop, term)                       \
    SEND_JS_NAME_PROP_2D_ARRAY_CODE(#prop, MATRIX_ROWS, MATRIX_COLS, term, \
                                    (SEND_DEC1(ec_config_keys[i][j].modes.prop);));
#  define SEND_JS_TEST_RESULT(prop, type, term)                         \
    SEND_JS_NAME_PROP_2D_ARRAY_CODE(#prop, EC_TEST_CHARGE_PLOT_COUNT,   \
                                    EC_TEST_DISCHARGE_PLOT_COUNT, term, \
                                    (SEND_##type(ec_test_result[i][j].prop);));

void ec_config_debug_send_debug_values(void) {
  send_string("const misc_state = {\n");
#  ifdef DEBUG_MATRIX_SCAN_RATE
  uint32_t matrix_scan_rate = get_matrix_scan_rate();
  SEND_JS_PROP_VALUE(matrix_scan_rate, WORD, ",\n");
#  endif
  SEND_JS_PROP_VALUE(ec_eeprom_config_reseted, BOOL, ",\n");
  SEND_JS_PROP_VALUE(ec_eeprom_config_error, WORD, ",\n");
  SEND_EC_CONFIG_KEY_MATRIX(extremum, WORD, ",\n");
  send_string("scan_test_result: {\n");
  SEND_JS_TEST_RESULT(floor_min, WORD, ",\n");
  SEND_JS_TEST_RESULT(floor_max, WORD, ",\n");
  SEND_JS_NAME_PROP_2D_ARRAY_CODE(
    "floor_noise", EC_TEST_CHARGE_PLOT_COUNT, EC_TEST_DISCHARGE_PLOT_COUNT, ",\n",
    (SEND_WORD(ec_test_result[i][j].floor_max - ec_test_result[i][j].floor_min)))
  SEND_JS_TEST_RESULT(bottom_max, WORD, "\n");
  send_string("}\n");
  send_string("}\n");
}

void ec_config_debug_send_calibration(void) {
  send_string("const calibrtion = {\n");
  SEND_EC_CONFIG_KEY_MATRIX(noise, BYTE, ",\n");
  SEND_EC_CONFIG_KEY_MATRIX(actuation_count, WORD, ",\n");
  SEND_EC_CONFIG_KEY_MATRIX(bottoming_max, WORD, ",\n");
  SEND_JS_NAME_PROP_2D_ARRAY("noise_floor", ec_eeprom_config.noise_floor, MATRIX_ROWS, MATRIX_COLS,
                             WORD, ",\n");
  SEND_JS_NAME_PROP_2D_ARRAY("bottoming_readiong", ec_eeprom_config.bottoming_reading, MATRIX_ROWS,
                             MATRIX_COLS, WORD, ",\n");
  SEND_JS_NAME_PROP_2D_ARRAY_CODE(
    "range", MATRIX_ROWS, MATRIX_COLS, ",\n",
    (SEND_WORD(ec_eeprom_config.bottoming_reading[i][j] - ec_eeprom_config.noise_floor[i][j])););
  SEND_JS_NAME_PROP_2D_ARRAY_CODE(
    "SNR_percentage", MATRIX_ROWS, MATRIX_COLS, "\n",
    (SEND_WORD(ec_config_keys[i][j].noise * 100 /
               (ec_eeprom_config.bottoming_reading[i][j] - ec_eeprom_config.noise_floor[i][j]));));
  send_string("}\n");
}

void ec_config_debug_send_config_keys(void) {
  send_string("const key_config = {\n");
  SEND_EC_CONFIG_KEY_MATRIX_MODE(actuation_mode, ",\n");
  SEND_EC_CONFIG_KEY_MATRIX(actuation_reference, WORD, ",\n");
  SEND_EC_CONFIG_KEY_MATRIX_MODE(release_mode, ",\n");
  SEND_EC_CONFIG_KEY_MATRIX(release_reference, WORD, ",\n");
  SEND_EC_CONFIG_KEY_MATRIX(deadzone, WORD, ",\n");
  SEND_EC_CONFIG_KEY_MATRIX(sub_action_keycode, WORD, ",\n");
  SEND_EC_CONFIG_KEY_MATRIX(sub_action_actuation_threshold, WORD, ",\n");
  SEND_EC_CONFIG_KEY_MATRIX_MODE(sub_action_release_mode, ",\n");
  SEND_EC_CONFIG_KEY_MATRIX(sub_action_release_threshold, WORD, "\n");
  send_string("}\n");
}

void ec_config_debug_send_all(void) {
  ec_config_debug_send_debug_values();
  ec_config_debug_send_calibration();
  ec_config_debug_send_config_keys();
}
#endif  // EC_DEBUG_ENABLE
