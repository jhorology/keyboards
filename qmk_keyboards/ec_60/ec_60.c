/* Copyright 2023 Cipulot
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "ec_60.h"

#include <eeprom.h>

#include "ec_switch_matrix.h"

#define RESCALE_ACTUATION_THRESHOLD 1
#define RESCALE_RELEASE_THRESHOLD (1 << 1)

#define RESCALE_ACTUATION_MOVING_DISTANCE 1
#define RESCALE_RELEASE_MOVING_DISTANCE (1 << 1)
#define RESCALE_INITIAL_DEAD_ZONE (1 << 2)

#define RESCALE_ALL 0xff

// Declaring enums for VIA config menu
enum via_apc_enums {
  // clang-format off
  id_ec_actuation_mode = 1,
  id_ec_mode_0_actuation_threshold,
  id_ec_mode_0_release_threshold,
  id_ec_mode_1_initial_deadzone_offset,
  id_ec_mode_1_actuation_moving_distance,
  id_ec_mode_1_release_moving_distance,
  id_ec_bottoming_calibration,
  id_ec_show_calibration_data
  // clang-format on
};

#define NO_KEY_THRESHOLD 0x60

static deferred_token show_calibration_data_token;  // defer_exec token
static bool ec_config_initialized;
static int ec_config_error;

// if defined in ec_60/config.h or ec_60/keymaps/<keymap name>/config.h
#ifdef DEFAULT_BOTTOMING_READING_USER
const uint16_t PROGMEM bottming_reading_default[MATRIX_ROWS][MATRIX_COLS] = DEFAULT_BOTTOMING_READING_USER;
#endif

static int is_eeprom_ec_config_valid(void);
static void defer_show_calibration_data(uint32_t delay_ms);
static void ec_config_clear_extremum(void);
static void ec_config_rescale(uint8_t actuation_mode, uint8_t flags);
static void ec_config_set_value(uint8_t *data);
static void ec_config_get_value(uint8_t *data);
static void ec_config_set_actuation_mode(uint8_t mode);

// QMK hook functions
// -----------------------------------------------------------------------------------

void eeconfig_init_user(void) {
  // Default values
  eeprom_ec_config.actuation_mode = DEFAULT_ACTUATION_MODE;
  eeprom_ec_config.mode_0_actuation_threshold = DEFAULT_MODE_0_ACTUATION_LEVEL;
  eeprom_ec_config.mode_0_release_threshold = DEFAULT_MODE_0_RELEASE_LEVEL;
  eeprom_ec_config.mode_1_initial_deadzone_offset = DEFAULT_MODE_1_INITIAL_DEADZONE_OFFSET;
  eeprom_ec_config.mode_1_actuation_moving_distance = DEFAULT_MODE_1_ACTUATION_MOVING_DISTANCE;
  eeprom_ec_config.mode_1_release_moving_distance = DEFAULT_MODE_1_RELEASE_MOVING_DISTANCE;

  for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
    for (uint8_t col = 0; col < MATRIX_COLS; col++) {
      // I don't want to lose calibration data for each update firware
#ifdef DEFAULT_BOTTOMING_READING_USER
      eeprom_ec_config.bottoming_reading[row][col] = pgm_read_word(&bottming_reading_default[row][col]);
#else
      eeprom_ec_config.bottoming_reading[row][col] = DEFAULT_BOTTOMING_READING;
#endif
    }
  }
  // Write default value to EEPROM now
  eeprom_update_block(&eeprom_ec_config, (void *)VIA_EEPROM_CUSTOM_CONFIG_USER_ADDR, sizeof(eeprom_ec_config_t));
  ec_config_initialized = true;
}

// On Keyboard startup
void keyboard_post_init_user(void) {
  // Read custom menu variables from memory
  eeprom_read_block(&eeprom_ec_config, (void *)VIA_EEPROM_CUSTOM_CONFIG_USER_ADDR, sizeof(eeprom_ec_config_t));

  ec_config_error = is_eeprom_ec_config_valid();
  if (ec_config_error != 0) {
    eeconfig_init_user();
  }

  ec_config.bottoming_calibration = false;
  for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
    for (uint8_t col = 0; col < MATRIX_COLS; col++) {
      ec_config.bottoming_calibration_starter[row][col] = true;
    }
  }
  ec_config_rescale(eeprom_ec_config.actuation_mode, RESCALE_ALL);
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  switch (keycode) {
    case EC_DBG:
      // release after 1 scond.
      if (!record->event.pressed) {
        defer_show_calibration_data(1000UL);
        return false;
      }
      break;
    case RT_TOGG:
      if (record->event.pressed) {
        ec_config_set_actuation_mode(eeprom_ec_config.actuation_mode == 0 ? 1 : 0);
        return false;
      }
      break;
    case RT_ON:
      if (record->event.pressed) {
        ec_config_set_actuation_mode(get_mods() & MOD_MASK_SHIFT ? 0 : 1);
        return false;
      }
      break;
    case RT_OFF:
      if (record->event.pressed) {
        ec_config_set_actuation_mode(get_mods() & MOD_MASK_SHIFT ? 1 : 0);
        return false;
      }
      break;
  }
  return true;
}

// common lib hook functions
// -----------------------------------------------------------------------------------

bool via_custom_value_command_user(uint8_t *data, uint8_t length) {
  // data = [ command_id, channel_id, value_id, value_data ]
  uint8_t *command_id = &(data[0]);
  uint8_t *channel_id = &(data[1]);
  uint8_t *value_id_and_data = &(data[2]);

  if (*channel_id == VIA_EC_CUSTOM_CHANNEL_ID) {
    switch (*command_id) {
      case id_custom_set_value:
        ec_config_set_value(value_id_and_data);
        return false;
      case id_custom_get_value:
        ec_config_get_value(value_id_and_data);
        return false;
      case id_custom_save:
        // nothing to do
        return false;
    }
  }
  return true;
}

// static routines
// -----------------------------------------------------------------------------------

// TODO more strict validation
static int is_eeprom_ec_config_valid(void) {
  if (eeprom_ec_config.actuation_mode > 1) return false;
  if (eeprom_ec_config.mode_0_actuation_threshold > 1023 || eeprom_ec_config.mode_0_actuation_threshold < 1) return -1;
  if (eeprom_ec_config.mode_0_release_threshold > 1023 || eeprom_ec_config.mode_0_release_threshold < 1) return -2;
  if (eeprom_ec_config.mode_1_initial_deadzone_offset > 1023 || eeprom_ec_config.mode_1_initial_deadzone_offset < 1)
    return -3;
  if (eeprom_ec_config.mode_1_actuation_moving_distance > 1023 || eeprom_ec_config.mode_1_actuation_moving_distance < 1)
    return -4;
  if (eeprom_ec_config.mode_1_release_moving_distance > 1023 || eeprom_ec_config.mode_1_release_moving_distance < 1)
    return -5;

  for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
    for (uint8_t col = 0; col < MATRIX_COLS; col++) {
      if (eeprom_ec_config.bottoming_reading[row][col] <= ec_config.noise_floor[row][col] ||
          eeprom_ec_config.bottoming_reading[row][col] > 0xfff)
        return -7;
    }
  }
  return 0;
}

static void ec_config_clear_extremum() {
  for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
    for (uint8_t col = 0; col < MATRIX_COLS; col++) {
      ec_config.extremum[row][col] = 0;
    }
  }
}

static void ec_config_rescale(uint8_t actuation_mode, uint8_t flags) {
  if (actuation_mode == 0) {
    // APC mode
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
      for (uint8_t col = 0; col < MATRIX_COLS; col++) {
        if (flags & RESCALE_ACTUATION_THRESHOLD) {
          ec_config.rescaled.mode_0.actuation_threshold[row][col] =
            rescale(eeprom_ec_config.mode_0_actuation_threshold, 0, 1023, ec_config.noise_floor[row][col],
                    eeprom_ec_config.bottoming_reading[row][col]);
        }
        if (flags & RESCALE_RELEASE_THRESHOLD) {
          ec_config.rescaled.mode_0.release_threshold[row][col] =
            rescale(eeprom_ec_config.mode_0_release_threshold, 0, 1023, ec_config.noise_floor[row][col],
                    eeprom_ec_config.bottoming_reading[row][col]);
        }
      }
    }
  } else if (actuation_mode == 1) {
    // Rapid Trigger mode
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
      for (uint8_t col = 0; col < MATRIX_COLS; col++) {
        if (flags & RESCALE_ACTUATION_MOVING_DISTANCE) {
          ec_config.rescaled.mode_1.actuation_moving_distance[row][col] =
            rescale(eeprom_ec_config.mode_1_actuation_moving_distance, 0, 1023, ec_config.noise_floor[row][col],
                    eeprom_ec_config.bottoming_reading[row][col]);
        }
        if (flags & RESCALE_RELEASE_MOVING_DISTANCE) {
          ec_config.rescaled.mode_1.release_moving_distance[row][col] =
            rescale(eeprom_ec_config.mode_1_release_moving_distance, 0, 1023, ec_config.noise_floor[row][col],
                    eeprom_ec_config.bottoming_reading[row][col]);
        }
        if (flags & RESCALE_INITIAL_DEAD_ZONE) {
          ec_config.rescaled.mode_1.initial_deadzone_offset[row][col] =
            rescale(eeprom_ec_config.mode_1_initial_deadzone_offset, 0, 1023, ec_config.noise_floor[row][col],
                    eeprom_ec_config.bottoming_reading[row][col]);
        }
      }
    }
  }
}

static void ec_config_set_actuation_mode(uint8_t mode) {
  eeprom_ec_config.actuation_mode = mode;
  ec_config_rescale(eeprom_ec_config.actuation_mode, RESCALE_ALL);
  if (eeprom_ec_config.actuation_mode == 1) {
    ec_config_clear_extremum();
  }
  eeprom_update_byte((void *)EC_CONFIG_ACTUATION_MODE, eeprom_ec_config.actuation_mode);
}

// Handle the data received by the keyboard from the VIA menus
static void ec_config_set_value(uint8_t *data) {
  // data = [ value_id, value_data ]
  uint8_t *value_id = &(data[0]);
  uint8_t *value_data = &(data[1]);

  switch (*value_id) {
    case id_ec_actuation_mode:
      // dropdown
      ec_config_set_actuation_mode(value_data[0]);
      break;
    case id_ec_mode_0_actuation_threshold:
      // range
      eeprom_ec_config.mode_0_actuation_threshold = (value_data[0] << 8) + value_data[1];
      ec_config_rescale(0, RESCALE_ACTUATION_THRESHOLD);
      defer_eeprom_update_word(VIA_EC_CUSTOM_CHANNEL_ID, id_ec_mode_0_actuation_threshold,
                               (void *)EC_CONFIG_MODE_0_ACTUATION_THRESHOLD,
                               eeprom_ec_config.mode_0_actuation_threshold);
      break;
    case id_ec_mode_0_release_threshold:
      // range
      eeprom_ec_config.mode_0_release_threshold = (value_data[0] << 8) + value_data[1];
      ec_config_rescale(0, RESCALE_RELEASE_THRESHOLD);
      defer_eeprom_update_word(VIA_EC_CUSTOM_CHANNEL_ID, id_ec_mode_0_release_threshold,
                               (void *)EC_CONFIG_MODE_0_RELEASE_THRESHOLD, eeprom_ec_config.mode_0_release_threshold);
      break;
    case id_ec_mode_1_initial_deadzone_offset:
      // range
      eeprom_ec_config.mode_1_initial_deadzone_offset = (value_data[0] << 8) + value_data[1];
      ec_config_rescale(1, RESCALE_INITIAL_DEAD_ZONE);
      defer_eeprom_update_word(VIA_EC_CUSTOM_CHANNEL_ID, id_ec_mode_1_initial_deadzone_offset,
                               (void *)EC_CONFIG_MODE_1_INITIAL_DEADZONE_OFFSET,
                               eeprom_ec_config.mode_1_initial_deadzone_offset);
      break;
    case id_ec_mode_1_actuation_moving_distance:
      // range
      eeprom_ec_config.mode_1_actuation_moving_distance = (value_data[0] << 8) + value_data[1];
      ec_config_rescale(1, RESCALE_ACTUATION_MOVING_DISTANCE);
      defer_eeprom_update_word(VIA_EC_CUSTOM_CHANNEL_ID, id_ec_mode_1_actuation_moving_distance,
                               (void *)EC_CONFIG_MODE_1_ACTUATION_MOVING_DISTANCE,
                               eeprom_ec_config.mode_1_actuation_moving_distance);
      break;
    case id_ec_mode_1_release_moving_distance:
      // range
      eeprom_ec_config.mode_1_release_moving_distance = (value_data[0] << 8) + value_data[1];
      ec_config_rescale(1, RESCALE_RELEASE_MOVING_DISTANCE);
      defer_eeprom_update_word(VIA_EC_CUSTOM_CHANNEL_ID, id_ec_mode_1_release_moving_distance,
                               (void *)EC_CONFIG_MODE_1_RELEASE_MOVING_DISTANCE,
                               eeprom_ec_config.mode_1_release_moving_distance);
      break;
    case id_ec_bottoming_calibration:
      ec_config.bottoming_calibration = value_data[0];
      if (ec_config.bottoming_calibration) {
        // start calibration
        for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
          for (uint8_t col = 0; col < MATRIX_COLS; col++) {
            ec_config.bottoming_calibration_starter[row][col] = true;
          }
        }
      } else {
        // end calibration
        ec_config_rescale(eeprom_ec_config.actuation_mode, RESCALE_ALL);
        eeprom_update_block(&eeprom_ec_config.bottoming_reading[0][0], (void *)EC_CONFIG_BOTTOMING_READING,
                            MATRIX_COLS * MATRIX_ROWS * 2);
      }
      break;
      // toggle
    case id_ec_show_calibration_data:
      if (value_data[0] != 0) {
        defer_show_calibration_data(3000UL);
      }
      break;
  }
}

// Handle the data sent by the keyboard to the VIA menus
static void ec_config_get_value(uint8_t *data) {
  // data = [ value_id, value_data ]
  uint8_t *value_id = &(data[0]);
  uint8_t *value_data = &(data[1]);

  switch (*value_id) {
    case id_ec_actuation_mode:
      // dropdown
      value_data[0] = eeprom_ec_config.actuation_mode;
      break;
    case id_ec_mode_0_actuation_threshold:
      // range
      value_data[0] = eeprom_ec_config.mode_0_actuation_threshold >> 8;
      value_data[1] = eeprom_ec_config.mode_0_actuation_threshold & 0xff;
      break;
    case id_ec_mode_0_release_threshold:
      // range
      value_data[0] = eeprom_ec_config.mode_0_release_threshold >> 8;
      value_data[1] = eeprom_ec_config.mode_0_release_threshold & 0xff;
      break;
    case id_ec_mode_1_initial_deadzone_offset:
      // range
      value_data[0] = eeprom_ec_config.mode_1_initial_deadzone_offset >> 8;
      value_data[1] = eeprom_ec_config.mode_1_initial_deadzone_offset & 0xff;
      break;
    case id_ec_mode_1_actuation_moving_distance:
      // range
      value_data[0] = eeprom_ec_config.mode_1_actuation_moving_distance >> 8;
      value_data[1] = eeprom_ec_config.mode_1_actuation_moving_distance & 0xff;
      break;
    case id_ec_mode_1_release_moving_distance:
      // range
      value_data[0] = eeprom_ec_config.mode_1_release_moving_distance >> 8;
      value_data[1] = eeprom_ec_config.mode_1_release_moving_distance & 0xff;
      break;
    case id_ec_bottoming_calibration:
      // toggle
      value_data[0] = ec_config.bottoming_calibration;
      break;
    case id_ec_show_calibration_data:
      // toggle
      value_data[0] = 0;
      break;
  }
}

static void _send_matrix_array(void *matrix_array, uint8_t size, bool is_bool, bool is_c) {
  send_string(is_c ? "{ \\\n" : "[\n");
  for (int row = 0; row < MATRIX_ROWS; row++) {
    send_string(is_c ? "{" : "[");
    for (int col = 0; col < MATRIX_COLS; col++) {
      if (is_bool) {
        send_string(*(bool *)matrix_array ? "true" : "false");
      } else {
        send_string("0x");
        if (size == 1) {
          send_byte(*(uint8_t *)matrix_array);
        } else if (size == 2) {
          send_word(*(uint16_t *)matrix_array);
        }
      }
      if (col < (MATRIX_COLS - 1)) {
        send_string(",");
      }
      matrix_array += size;
      wait_ms(50);
    }
    send_string(is_c ? "}" : "]");
    if (row < (MATRIX_ROWS - 1)) {
      send_string(",");
    }
    send_string(is_c ? " \\\n" : "\n");
  }
  send_string(is_c ? "}" : "]");
  wait_ms(100);
}

static void ec_send_config(void) {
  send_string("const data = {\nec_config_initialized: ");
  send_string(ec_config_initialized ? "true" : "false");
  send_string(",\nec_config_error: 0x");
  send_word(ec_config_error);
  send_string(",\nsw_value: ");
  _send_matrix_array(&sw_value[0][0], 2, false, false);
  send_string(",\nec_config: {\nactuation_mode: 0x");
  send_byte(eeprom_ec_config.actuation_mode);
  if (eeprom_ec_config.actuation_mode == 0) {
    send_string(",\nactuation_threshold: 0x");
    send_word(eeprom_ec_config.mode_0_actuation_threshold);
    send_string(",\nrelease_threshold: 0x");
    send_word(eeprom_ec_config.mode_0_release_threshold);
    send_string(",\nrescaled_actuation_threshold: ");
    _send_matrix_array(ec_config.rescaled.mode_0.actuation_threshold, 2, false, false);
    send_string(",\nrescaled_release_threshold: ");
    _send_matrix_array(ec_config.rescaled.mode_0.release_threshold, 2, false, false);
  } else if (eeprom_ec_config.actuation_mode == 1) {
    send_string(",\ninitial_deadzone_offset: 0x");
    send_word(eeprom_ec_config.mode_1_initial_deadzone_offset);
    send_string(",\nactuation_moving_distance: 0x");
    send_word(eeprom_ec_config.mode_1_actuation_moving_distance);
    send_string(",\nrelease_moving_distance: 0x");
    send_word(eeprom_ec_config.mode_1_release_moving_distance);
    send_string(",\nrescaled_mode_1_initial_deadzone_offset: ");
    _send_matrix_array(ec_config.rescaled.mode_1.initial_deadzone_offset, 2, false, false);
    send_string(",\nrescaled_mode_1_actuation_moving_distance: ");
    _send_matrix_array(ec_config.rescaled.mode_1.actuation_moving_distance, 2, false, false);
    send_string(",\nrescaled_mode_1_release_moving_distance: ");
    _send_matrix_array(ec_config.rescaled.mode_1.release_moving_distance, 2, false, false);
    send_string(",\nextremum: ");
    _send_matrix_array(ec_config.extremum, 2, false, false);
  }
  send_string(",\nnoise_floor: ");
  _send_matrix_array(&ec_config.noise_floor[0][0], 2, false, false);
  send_string(",\nbottoming_calibration: ");
  send_string(ec_config.bottoming_calibration ? "true" : "false");
  send_string(",\nbottoming_calibration_starter: ");
  _send_matrix_array(&ec_config.bottoming_calibration_starter[0][0], sizeof(bool), true, false);
  send_string(",\nbottoming_reading: ");
  _send_matrix_array(eeprom_ec_config.bottoming_reading, 2, false, false);

  send_string("\n}\n}\n/*\n// clang-format off\n#define DEFAULT_BOTTOMING_READING_USER ");
  _send_matrix_array(eeprom_ec_config.bottoming_reading, 2, false, true);
  send_string("\n// clang-format on\n*/\n");
}

static uint32_t defer_show_calibration_data_cb(uint32_t trigger_time, void *cb_arg) {
  ec_send_config();
  show_calibration_data_token = 0;
  return 0;
}

static void defer_show_calibration_data(uint32_t delay_ms) {
  show_calibration_data_token = defer_exec(delay_ms, &defer_show_calibration_data_cb, NULL);
}
