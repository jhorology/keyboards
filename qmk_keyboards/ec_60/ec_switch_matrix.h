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

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "eeconfig.h"
#include "matrix.h"

typedef struct {
  /*
    0: normal board-wide APC
    1: Rapid trigger from specific board-wide actuation point
    2: Rapid trigger from resting point
  */
  uint8_t actuation_mode;

  uint8_t _resereved;

  /* threshold for key press in mode 0 */
  uint16_t mode_0_actuation_threshold;

  /* threshold for key release in mode 0 */
  uint16_t mode_0_release_threshold;

  /* threshold for key press in mode 1 (initial deadzone) */
  uint16_t mode_1_initial_deadzone_offset;

  /* sensitivity for key press in mode 1 (1-255) */
  uint8_t mode_1_actuation_sensitivity;

  /* sensitivity for key release in mode 1 (1-255) */
  uint8_t mode_1_release_sensitivity;

  /* bottoming reading */
  uint16_t bottoming_reading[MATRIX_ROWS][MATRIX_COLS];
} __attribute__((packed)) eeprom_ec_config_t;

/* eeprom address */
#define EECONFIG_USER_EC_ACTUATION_MODE EECONFIG_USER_DATABLOCK
#define EECONFIG_USER_EC_MODE_0_ACTUATION_THRESHOLD (EECONFIG_USER_DATABLOCK + 2)
#define EECONFIG_USER_EC_MODE_0_RELEASE_THRESHOLD (EECONFIG_USER_DATABLOCK + 4)
#define EECONFIG_USER_EC_MODE_1_INITIAL_DEADZONE_OFFSET (EECONFIG_USER_DATABLOCK + 6)
#define EECONFIG_USER_EC_MODE_1_ACTUATION_SENSITIVITY (EECONFIG_USER_DATABLOCK + 8)
#define EECONFIG_USER_EC_MODE_1_RELEASE_SENSITIVITY (EECONFIG_USER_DATABLOCK + 9)
#define EECONFIG_USER_EC_BOTTOMING_READING (EECONFIG_USER_DATABLOCK + 10)

typedef struct {
  /*
    0: normal board-wide APC
    1: Rapid trigger from specific board-wide actuation point (it can be very near that baseline noise and be "full
    travel")
  */
  uint8_t actuation_mode;

  /* threshold for key press in mode 0 */
  uint16_t mode_0_actuation_threshold;

  /* threshold for key release in mode 0 */
  uint16_t mode_0_release_threshold;

  /* threshold for key press in mode 1 (initial deadzone) */
  uint16_t mode_1_initial_deadzone_offset;

  /* threshold for key press in mode 0 rescaled to actual scale */
  uint16_t rescaled_mode_0_actuation_threshold[MATRIX_ROWS][MATRIX_COLS];

  /* threshold for key release in mode 0 rescaled to actual scale */
  uint16_t rescaled_mode_0_release_threshold[MATRIX_ROWS][MATRIX_COLS];

  /* threshold for key press in mode 1 (initial deadzone) rescaled to actual scale */
  uint16_t rescaled_mode_1_initial_deadzone_offset[MATRIX_ROWS][MATRIX_COLS];

  /* sensitivity for key press in mode 1 (1-255) */
  uint8_t mode_1_actuation_sensitivity;

  /* sensitivity for key release in mode 1 (1-255) */
  uint8_t mode_1_release_sensitivity;

  /* extremum values for mode 1 */
  uint16_t extremum[MATRIX_ROWS][MATRIX_COLS];

  /* noise floor detected during startup */
  uint16_t noise_floor[MATRIX_ROWS][MATRIX_COLS];

  /* calibration mode for bottoming out values (true: calibration mode, false: normal mode) */
  bool bottoming_calibration;

  /* calibration mode for bottoming out values (true: calibration mode, false: normal mode) */
  bool bottoming_calibration_starter[MATRIX_ROWS][MATRIX_COLS];

  /* bottoming reading */
  uint16_t bottoming_reading[MATRIX_ROWS][MATRIX_COLS];
} ec_config_t;

// Check if the size of the reserved persistent memory is the same as the size of struct eeprom_ec_config_t
// _Static_assert(sizeof(eeprom_ec_config_t) == EECONFIG_KB_DATA_SIZE, "Mismatch in keyboard EECONFIG stored data");
// I use KB scoope for common library
_Static_assert(sizeof(eeprom_ec_config_t) == EECONFIG_USER_DATA_SIZE, "Mismatch in keyboard EECONFIG stored data");

extern eeprom_ec_config_t eeprom_ec_config;
extern ec_config_t ec_config;
extern uint16_t sw_value[MATRIX_ROWS][MATRIX_COLS];

void init_row(void);
void init_amux_sel(void);
void select_amux_channel(uint8_t channel, uint8_t col);
void disable_unused_amux(uint8_t channel);
void discharge_capacitor(void);
void charge_capacitor(uint8_t row);

int ec_init(void);
void ec_noise_floor(void);
bool ec_matrix_scan(matrix_row_t current_matrix[]);
uint16_t ec_readkey_raw(uint8_t channel, uint8_t row, uint8_t col);
bool ec_update_key(matrix_row_t* current_row, uint8_t row, uint8_t col, uint16_t sw_value);
#ifdef CONSOLE_ENABLE
void ec_print_matrix(void);
#endif

uint16_t rescale(uint16_t x, uint16_t in_min, uint16_t in_max, uint16_t out_min, uint16_t out_max);

#ifdef SEND_STRING_ENABLE
void ec_debug_send_matrix(void);
#endif
