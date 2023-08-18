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
#include <send_string.h>
#include <stdint.h>

#include "ec_60/config.h"
#include "ec_switch_matrix.h"

// Declaring enums for VIA config menu
enum via_apc_enums {
  // clang-format off
  id_ec_actuation_mode = 1,
  id_ec_mode_0_actuation_threshold,
  id_ec_mode_0_release_threshold,
  id_ec_mode_1_initial_deadzone_offset,
  id_ec_mode_1_actuation_sensitivity,
  id_ec_mode_1_release_sensitivity,
  id_ec_bottoming_calibration,
  id_ec_show_calibration_data
  // clang-format on
};

#define NO_KEY_THRESHOLD 0x60

static deferred_token show_calibration_data_token;  // defer_exec token
static bool ec_config_initilized;

#ifdef ENABLE_CALIBRATED_BOTTOMING_READING
const uint16_t PROGMEM caliblated_bottming_reading[MATRIX_ROWS][MATRIX_COLS] = {
  {0x0248, 0x025b, 0x0256, 0x02f1, 0x0251, 0x026d, 0x020d, 0x023f, 0x0299, 0x01ed, 0x027c, 0x0262, 0x02ce, 0x01bd,
   0x023e},
  {0x0292, 0x02c1, 0x0308, 0x0348, 0x02c3, 0x0302, 0x02ce, 0x02a5, 0x0344, 0x02ed, 0x02b3, 0x02a8, 0x0224, 0x01d1,
   0x03ff},
  {0x01e2, 0x02da, 0x02e5, 0x0340, 0x02bd, 0x02e6, 0x0292, 0x030c, 0x031e, 0x02a1, 0x02b7, 0x02ef, 0x03ff, 0x029d,
   0x03ff},
  {0x01e8, 0x03ff, 0x028f, 0x0319, 0x0326, 0x02e0, 0x02a1, 0x034a, 0x037a, 0x02c2, 0x02c5, 0x02a8, 0x03ff, 0x0179,
   0x01ec},
  {0x01ef, 0x01f1, 0x01bc, 0x03ff, 0x03ff, 0x03ff, 0x021d, 0x03ff, 0x03ff, 0x03ff, 0x017d, 0x01a3, 0x01d9, 0x03ff,
   0x03ff}};
#endif

static bool is_eeprom_ec_config_valid(void);
static void defer_show_calibration_data(uint32_t delay_ms);
static void ec_config_rescale(void);
static void ec_config_set_value(uint8_t *data);
static void ec_config_get_value(uint8_t *data);
static void ec_config_save_value(uint8_t *data);

// QMK hook functions
// -----------------------------------------------------------------------------------

void eeconfig_init_user(void) {
  // Default values
  eeprom_ec_config.actuation_mode = DEFAULT_ACTUATION_MODE;
  eeprom_ec_config.mode_0_actuation_threshold = DEFAULT_MODE_0_ACTUATION_LEVEL;
  eeprom_ec_config.mode_0_release_threshold = DEFAULT_MODE_0_RELEASE_LEVEL;
  eeprom_ec_config.mode_1_initial_deadzone_offset = DEFAULT_MODE_1_INITIAL_DEADZONE_OFFSET;
  eeprom_ec_config.mode_1_actuation_sensitivity = DEFAULT_MODE_1_ACTUATION_SENSITIVITY;
  eeprom_ec_config.mode_1_release_sensitivity = DEFAULT_MODE_1_RELEASE_SENSITIVITY;

  for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
    for (uint8_t col = 0; col < MATRIX_COLS; col++) {
      // I don't want to lose calibration data for each update firware
#ifdef ENABLE_CALIBRATED_BOTTOMING_READING
      eeprom_ec_config.bottoming_reading[row][col] = pgm_read_word(&caliblated_bottming_reading[row][col]);
#else
      eeprom_ec_config.bottoming_reading[row][col] = DEFAULT_BOTTOMING_READING;
#endif
    }
  }
  // Write default value to EEPROM now
  eeconfig_update_user_datablock(&eeprom_ec_config);
  ec_config_initilized = true;
}

// On Keyboard startup
void keyboard_post_init_user(void) {
  // Read custom menu variables from memory
  eeconfig_read_user_datablock(&eeprom_ec_config);

  if (!is_eeprom_ec_config_valid()) {
    eeconfig_init_user();
  }

  // Set runtime values to EEPROM values
  ec_config.actuation_mode = eeprom_ec_config.actuation_mode;
  ec_config.mode_0_actuation_threshold = eeprom_ec_config.mode_0_actuation_threshold;
  ec_config.mode_0_release_threshold = eeprom_ec_config.mode_0_release_threshold;
  ec_config.mode_1_initial_deadzone_offset = eeprom_ec_config.mode_1_initial_deadzone_offset;
  ec_config.mode_1_actuation_sensitivity = eeprom_ec_config.mode_1_actuation_sensitivity;
  ec_config.mode_1_release_sensitivity = eeprom_ec_config.mode_1_release_sensitivity;
  ec_config.bottoming_calibration = false;
  for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
    for (uint8_t col = 0; col < MATRIX_COLS; col++) {
      ec_config.bottoming_calibration_starter[row][col] = true;
      ec_config.bottoming_reading[row][col] = eeprom_ec_config.bottoming_reading[row][col];
    }
  }
  ec_config_rescale();
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  switch (keycode) {
    case EC_DBG:
      // release after 1 scond.
      if (!record->event.pressed) {
        defer_show_calibration_data(1000UL);
        return false;
      }
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
      case id_custom_set_value: {
        ec_config_set_value(value_id_and_data);
        return false;
      }
      case id_custom_get_value: {
        ec_config_get_value(value_id_and_data);
        return false;
      }
      case id_custom_save: {
        ec_config_save_value(value_id_and_data);
        return false;
      }
    }
  }
  return true;
}

// static routines
// -----------------------------------------------------------------------------------

// TODO more strict validation
static bool is_eeprom_ec_config_valid(void) {
  if (eeprom_ec_config.actuation_mode > 1) return false;
  if (eeprom_ec_config.mode_0_actuation_threshold > DEFAULT_EXTREMUM || eeprom_ec_config.mode_0_actuation_threshold < 1)
    return false;
  if (eeprom_ec_config.mode_0_release_threshold > DEFAULT_EXTREMUM || eeprom_ec_config.mode_0_release_threshold < 1)
    return false;
  if (eeprom_ec_config.mode_1_initial_deadzone_offset > DEFAULT_EXTREMUM ||
      eeprom_ec_config.mode_1_initial_deadzone_offset < 1)
    return false;

  if (eeprom_ec_config.mode_1_actuation_sensitivity == 0) return false;
  if (eeprom_ec_config.mode_1_release_sensitivity == 0) return false;

  for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
    for (uint8_t col = 0; col < MATRIX_COLS; col++) {
      // ADC 12bit EXTRENUM 10bit
      if (eeprom_ec_config.bottoming_reading[row][col] <= ec_config.noise_floor[row][col] ||
          eeprom_ec_config.bottoming_reading[row][col] > 0xfff)
        return false;
    }
  }
  return true;
}

static void ec_config_rescale_mode_0_actuation_threshold(void) {
  for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
    for (uint8_t col = 0; col < MATRIX_COLS; col++) {
      ec_config.rescaled_mode_0_actuation_threshold[row][col] =
        rescale(ec_config.mode_0_actuation_threshold, 0, 1023, ec_config.noise_floor[row][col],
                eeprom_ec_config.bottoming_reading[row][col]);
    }
  }
}

static void ec_config_rescale_mode_0_release_threshold(void) {
  for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
    for (uint8_t col = 0; col < MATRIX_COLS; col++) {
      ec_config.rescaled_mode_0_release_threshold[row][col] =
        rescale(ec_config.mode_0_release_threshold, 0, 1023, ec_config.noise_floor[row][col],
                eeprom_ec_config.bottoming_reading[row][col]);
    }
  }
}

static void ec_config_rescale_mode_1_initial_deadzone_offset(void) {
  for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
    for (uint8_t col = 0; col < MATRIX_COLS; col++) {
      ec_config.rescaled_mode_1_initial_deadzone_offset[row][col] =
        rescale(ec_config.mode_1_initial_deadzone_offset, 0, 1023, ec_config.noise_floor[row][col],
                eeprom_ec_config.bottoming_reading[row][col]);
    }
  }
}

static void ec_config_rescale(void) {
  ec_config_rescale_mode_0_actuation_threshold();
  ec_config_rescale_mode_0_release_threshold();
  ec_config_rescale_mode_1_initial_deadzone_offset();
}

// Handle the data received by the keyboard from the VIA menus
static void ec_config_set_value(uint8_t *data) {
  // data = [ value_id, value_data ]
  uint8_t *value_id = &(data[0]);
  uint8_t *value_data = &(data[1]);

  switch (*value_id) {
    case id_ec_actuation_mode:
      // dropdown
      ec_config.actuation_mode = value_data[0];
      eeprom_ec_config.actuation_mode = ec_config.actuation_mode;
      break;
    case id_ec_mode_0_actuation_threshold:
      // range
      ec_config.mode_0_actuation_threshold = (value_data[0] << 8) + value_data[1];
      eeprom_ec_config.mode_0_actuation_threshold = ec_config.mode_0_actuation_threshold;
      break;
    case id_ec_mode_0_release_threshold:
      // range
      ec_config.mode_0_release_threshold = (value_data[0] << 8) + value_data[1];
      eeprom_ec_config.mode_0_release_threshold = ec_config.mode_0_release_threshold;
      ec_config_rescale_mode_0_release_threshold();
      break;
    case id_ec_mode_1_initial_deadzone_offset:
      // range
      ec_config.mode_1_initial_deadzone_offset = (value_data[0] << 8) + value_data[1];
      eeprom_ec_config.mode_1_initial_deadzone_offset = ec_config.mode_1_initial_deadzone_offset;
      ec_config_rescale_mode_1_initial_deadzone_offset();
      break;
    case id_ec_mode_1_actuation_sensitivity:
      // range
      ec_config.mode_1_actuation_sensitivity = value_data[0];
      eeprom_ec_config.mode_1_actuation_sensitivity = ec_config.mode_1_actuation_sensitivity;
      break;
    case id_ec_mode_1_release_sensitivity:
      // range
      ec_config.mode_1_release_sensitivity = value_data[0];
      eeprom_ec_config.mode_1_release_sensitivity = ec_config.mode_1_release_sensitivity;
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
        for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
          for (uint8_t col = 0; col < MATRIX_COLS; col++) {
            if (!ec_config.bottoming_calibration_starter[row][col]) {
              eeprom_ec_config.bottoming_reading[row][col] = ec_config.bottoming_reading[row][col];
            };
          }
        }
        ec_config_rescale();
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
      value_data[0] = ec_config.actuation_mode;
      break;
    case id_ec_mode_0_actuation_threshold:
      // range
      value_data[0] = ec_config.mode_0_actuation_threshold >> 8;
      value_data[1] = ec_config.mode_0_actuation_threshold & 0xff;
      break;
    case id_ec_mode_0_release_threshold:
      // range
      value_data[0] = ec_config.mode_0_release_threshold >> 8;
      value_data[1] = ec_config.mode_0_release_threshold & 0xff;
      break;
    case id_ec_mode_1_initial_deadzone_offset:
      // range
      value_data[0] = ec_config.mode_1_initial_deadzone_offset >> 8;
      value_data[1] = ec_config.mode_1_initial_deadzone_offset & 0xff;
      break;
    case id_ec_mode_1_actuation_sensitivity:
      // range
      value_data[0] = ec_config.mode_1_actuation_sensitivity;
      break;
    case id_ec_mode_1_release_sensitivity:
      // range
      value_data[0] = ec_config.mode_1_release_sensitivity;
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

// Save the data to persistent memory after changes are made
void ec_config_save_value(uint8_t *data) {
  uint8_t *value_id = &(data[0]);
  switch (*value_id) {
    case id_ec_actuation_mode:
      // dropdown
      eeprom_update_byte(EECONFIG_USER_EC_ACTUATION_MODE, eeprom_ec_config.actuation_mode);
      break;
    case id_ec_mode_0_actuation_threshold:
      // range
      defer_eeprom_update_word(VIA_EC_CUSTOM_CHANNEL_ID, id_ec_mode_0_actuation_threshold,
                               EECONFIG_USER_EC_MODE_0_ACTUATION_THRESHOLD,
                               eeprom_ec_config.mode_0_actuation_threshold);
      break;
    case id_ec_mode_0_release_threshold:
      // range
      defer_eeprom_update_word(VIA_EC_CUSTOM_CHANNEL_ID, id_ec_mode_0_release_threshold,
                               EECONFIG_USER_EC_MODE_0_RELEASE_THRESHOLD, eeprom_ec_config.mode_0_release_threshold);
      break;
    case id_ec_mode_1_initial_deadzone_offset:
      // range
      defer_eeprom_update_word(VIA_EC_CUSTOM_CHANNEL_ID, id_ec_mode_1_initial_deadzone_offset,
                               EECONFIG_USER_EC_MODE_1_INITIAL_DEADZONE_OFFSET,
                               eeprom_ec_config.mode_1_initial_deadzone_offset);
      break;
    case id_ec_mode_1_actuation_sensitivity:
      // range
      defer_eeprom_update_byte(VIA_EC_CUSTOM_CHANNEL_ID, id_ec_mode_1_actuation_sensitivity,
                               EECONFIG_USER_EC_MODE_1_ACTUATION_SENSITIVITY,
                               eeprom_ec_config.mode_1_actuation_sensitivity);
      break;
    case id_ec_mode_1_release_sensitivity:
      // range
      defer_eeprom_update_byte(VIA_EC_CUSTOM_CHANNEL_ID, id_ec_mode_1_release_sensitivity,
                               EECONFIG_USER_EC_MODE_1_RELEASE_SENSITIVITY,
                               eeprom_ec_config.mode_1_release_sensitivity);
      break;
    case id_ec_bottoming_calibration:
      // toggle
      if (ec_config.bottoming_calibration == 0) {
        eeprom_update_block(&eeprom_ec_config.bottoming_reading[0][0], EECONFIG_USER_EC_BOTTOMING_READING,
                            MATRIX_COLS * MATRIX_ROWS * 2);
      }
      break;
    case id_ec_show_calibration_data:
      // toggle
      // nothing to do
      break;
  }
}

static void _send_matrix_array(void *matrix_array, uint8_t size, bool is_bool, bool is_c) {
  send_string(is_c ? "{\n" : "[\n");
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
      wait_ms(20);
    }
    send_string(is_c ? "}" : "]");
    if (row < (MATRIX_ROWS - 1)) {
      send_string(",");
    }
    send_string("\n");
  }
  send_string(is_c ? "}" : "]");
  wait_ms(40);
}

static void ec_send_config(void) {
  send_string("const data = {\nsw_value: ");
  _send_matrix_array(&sw_value[0][0], 2, false, false);
  send_string(",\nec_config: {\nactuation_mode: 0x");
  send_byte(ec_config.actuation_mode);
  send_string(",\nmode_0_actuation_threshold: 0x");
  send_word(ec_config.mode_0_actuation_threshold);
  send_string(",\nmode_0_release_threshold: 0x");
  send_word(ec_config.mode_0_release_threshold);
  send_string(",\nmode_1_initial_deadzone_offset: 0x");
  send_word(ec_config.mode_1_initial_deadzone_offset);

  send_string(",\nrescaled_mode_0_actuation_threshold: ");
  _send_matrix_array(&ec_config.rescaled_mode_0_actuation_threshold[0][0], 2, false, false);
  send_string(",\nrescaled_mode_0_release_threshold: ");
  _send_matrix_array(&ec_config.rescaled_mode_0_release_threshold[0][0], 2, false, false);

  send_string(",\nrescaled_mode_1_initial_deadzone_offset: ");
  _send_matrix_array(&ec_config.rescaled_mode_1_initial_deadzone_offset[0][0], 2, false, false);
  send_string(",\nmode_1_actuation_sensitivity: 0x");
  send_byte(ec_config.mode_1_actuation_sensitivity);
  send_string(",\nmode_1_release_sensitivity: 0x");
  send_byte(ec_config.mode_1_release_sensitivity);
  send_string(",\nextremum: ");
  _send_matrix_array(&ec_config.extremum[0][0], 2, false, false);
  send_string(",\nnoise_floor: ");
  _send_matrix_array(&ec_config.noise_floor[0][0], 2, false, false);
  send_string(",\nbottoming_calibration: ");
  send_string(ec_config.bottoming_calibration ? "true" : "false");
  send_string(",\nbottoming_calibration_starter: ");
  _send_matrix_array(&ec_config.bottoming_calibration_starter[0][0], sizeof(bool), true, false);
  send_string(",\nbottoming_reading: ");
  _send_matrix_array(&ec_config.bottoming_reading[0][0], 2, false, false);

  send_string("\n}\n}\n/*\nconst uint16_t PROGMEM caliblated_bottming_reading[MATRIX_ROWS][MATRIX_COLS] = ");
  _send_matrix_array(&ec_config.bottoming_reading[0][0], 2, false, true);
  send_string(";\n*/\n");
}

static uint32_t defer_show_calibration_data_cb(uint32_t trigger_time, void *cb_arg) {
  ec_send_config();
  show_calibration_data_token = 0;
  return 0;
}

static void defer_show_calibration_data(uint32_t delay_ms) {
  show_calibration_data_token = defer_exec(delay_ms, &defer_show_calibration_data_cb, NULL);
}
