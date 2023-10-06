/* Copyright 2023 Cipulot
 * Modified 2023 masafumi
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

#include "ec_switch_matrix.h"

#include <analog.h>
#include <atomic_util.h>
#include <math.h>
#include <wait.h>

#include "ec_config.h"

// Pin and port array
const uint32_t row_pins[] = MATRIX_ROW_PINS;
const uint32_t amux_sel_pins[] = AMUX_SEL_PINS;
const uint32_t amux_en_pins[] = AMUX_EN_PINS;
const uint8_t matrix_col_channels[] = MATRIX_COL_CHANNELS;
static rtcnt_t key_scan_time;

#define AMUX_SEL_PINS_COUNT (sizeof(amux_sel_pins) / sizeof(amux_sel_pins[0]))
#define EXPECTED_AMUX_SEL_PINS_COUNT ceil(log2(AMUX_MAX_COLS_COUNT)
// Checks for the correctness of the configuration
_Static_assert(
  (sizeof(amux_en_pins) / sizeof(amux_en_pins[0])) == AMUX_COUNT,
  "AMUX_EN_PINS doesn't have the minimum number of bits required to enable all the multiplexers available");
// Check that number of select pins is enough to select all the channels
_Static_assert(AMUX_SEL_PINS_COUNT == EXPECTED_AMUX_SEL_PINS_COUNT), "AMUX_SEL_PINS doesn't have the minimum number of bits required address all the channels");
// Check that number of elements in AMUX_COL_CHANNELS_SIZES is enough to specify the number of channels for all the
// multiplexers available

#ifdef EC_DEBUG
uint16_t sw_value[MATRIX_ROWS][MATRIX_COLS];
static uint32_t matrix_timer = 0;
static uint32_t matrix_scan_count = 0;
uint32_t last_matrix_scan_count = 0;
#endif

static adc_mux adcMux;

static void ec_bootoming_reading(void);
static void init_row(void);
static void init_amux(void);
static inline void select_col(uint8_t amux_col_ch);
static uint16_t ec_readkey_raw(uint8_t row);
static inline bool ec_update_key(matrix_row_t* current_row, uint8_t row, uint8_t col, uint16_t sw_value);

#define MATRIX_READ_LOOP(...)                     \
  for (int col = 0; col < MATRIX_COLS; col++) {   \
    select_col(matrix_col_channels[col]);         \
    for (int row = 0; row < MATRIX_ROWS; row++) { \
      uint16_t value = ec_readkey_raw(row);       \
      __VA_ARGS__                                 \
    }                                             \
  }

// QMK hook functions
// -----------------------------------------------------------------------------------

// Initialize the peripherals pins
void matrix_init_custom(void) {
  // Initialize ADC
  palSetLineMode(ANALOG_PORT, PAL_MODE_INPUT_ANALOG);
  adcMux = pinToMux(ANALOG_PORT);

  // Dummy call to make sure that adcStart() has been called in the appropriate state
  adc_read(adcMux);

  // Initialize discharge pin as discharge mode
  writePinLow(DISCHARGE_PIN);
  setPinOutputOpenDrain(DISCHARGE_PIN);

  // Initialize drive lines
  init_row();

  // Initialize AMUXs
  init_amux();

  key_scan_time = chSysGetRealtimeCounterX();
  ec_calibrate_noise_floor();
}

bool matrix_scan_custom(matrix_row_t current_matrix[]) {
  bool updated = false;

  // Bottoming calibration mode: update bottoming out values and avoid keycode state change
  // IF statement is higher in the function to avoid the overhead of the execution of normal operation mode
  if (ec_config.bottoming_calibration) {
    ec_bootoming_reading();
    // Return false to avoid keycode state change
    return false;
  }

  // Normal operation mode: update key state
  MATRIX_READ_LOOP(
#ifdef EC_DEBUG
    sw_value[row][col] = value;
#endif
    updated |= ec_update_key(&current_matrix[row], row, col, value);)

#ifdef EC_DEBUG
  matrix_scan_count++;

  uint32_t timer_now = timer_read32();
  if (TIMER_DIFF_32(timer_now, matrix_timer) >= 1000) {
    last_matrix_scan_count = matrix_scan_count;
    matrix_timer = timer_now;
    matrix_scan_count = 0;
  }
#endif
  return updated;
}

// static routines
// -----------------------------------------------------------------------------------

// Get the noise floor
void ec_calibrate_noise_floor(void) {
  // Initialize the noise floor to 0
  for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
    for (uint8_t col = 0; col < MATRIX_COLS; col++) {
      ec_config.noise_floor[row][col] = 0;
      ec_config.extremum[row][col] = 0;
    }
  }

  // Get the noise floor
  // max: ec_config.noise_floor[row][col]
  // min: ec_config.extremum[row][col]
  for (uint8_t i = 0; i < NOISE_FLOOR_SAMPLING_COUNT; i++) {
    wait_ms(5);
    MATRIX_READ_LOOP(
      // max value
      if (value > ec_config.noise_floor[row][col]) { ec_config.noise_floor[row][col] = value; }
      // min value
      if (ec_config.extremum[row][col] == 0 || value < ec_config.extremum[row][col]) {
        ec_config.extremum[row][col] = value;
      })
  }

  // Average the noise floor
  for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
    for (uint8_t col = 0; col < MATRIX_COLS; col++) {
      ec_config.noise[row][col] = ec_config.noise_floor[row][col] - ec_config.extremum[row][col];
      ec_config.noise_floor[row][col] = (ec_config.noise_floor[row][col] + ec_config.extremum[row][col]) / 2;
      ec_config.extremum[row][col] = ec_config.noise_floor[row][col];
    }
  }
}

static void ec_bootoming_reading(void) {
  MATRIX_READ_LOOP(
#ifdef EC_DEBUG
    sw_value[row][col] = value;
#endif
    if (value > (ec_config.noise_floor[row][col] + 32)) {
      if (ec_config.bottoming_calibration_starter[row][col]) {
        eeprom_ec_config.bottoming_reading[row][col] = value;
        ec_config.bottoming_calibration_starter[row][col] = false;
      } else if (value > eeprom_ec_config.bottoming_reading[row][col]) {
        eeprom_ec_config.bottoming_reading[row][col] = value;
      }
    })
}

// Initialize the row pins
static void init_row(void) {
  // Set all row pins as output and low
  for (uint8_t idx = 0; idx < MATRIX_ROWS; idx++) {
    setPinOutput(row_pins[idx]);
    writePinLow(row_pins[idx]);
  }
}

// Initialize the multiplexers
static void init_amux(void) {
  for (uint8_t idx = 0; idx < AMUX_COUNT; idx++) {
    setPinOutput(amux_en_pins[idx]);
    writePinHigh(amux_en_pins[idx]);
  }
  for (uint8_t idx = 0; idx < AMUX_SEL_PINS_COUNT; idx++) {
    setPinOutput(amux_sel_pins[idx]);
  }
}

static inline void select_col(uint8_t amux_col_ch) {
  if ((amux_col_ch & 0xf) == 0) {
#if AMUX_COUNT >= 1
    writePin(amux_en_pins[0], amux_col_ch & 0x10);
#endif
#if AMUX_COUNT >= 2
    writePin(amux_en_pins[1], amux_col_ch & 0x20);
#endif
#if AMUX_COUNT >= 3
    writePin(amux_en_pins[2], amux_col_ch & 0x40);
#endif
#if AMUX_COUNT >= 4
    writePin(amux_en_pins[3], amux_col_ch & 0x88);
#endif
#if AMUX_COUNT >= 5
#  error Unsupported AMUX_COUNT, maximum is 4
#endif
  }
  writePin(amux_sel_pins[0], amux_col_ch & 1);
  writePin(amux_sel_pins[1], amux_col_ch & 2);
  writePin(amux_sel_pins[2], amux_col_ch & 4);
}

// Read the capacitive sensor value
static uint16_t ec_readkey_raw(uint8_t row) {
  uint16_t sw_value;

  // wait_us(DISCHARGE_TIME)
  while (chSysIsCounterWithinX(chSysGetRealtimeCounterX(), key_scan_time,
                               key_scan_time + US2RTC(REALTIME_COUNTER_CLOCK, DISCHARGE_TIME))) {
  }
  writePinHigh(DISCHARGE_PIN);

  ATOMIC_BLOCK_FORCEON {
    writePinHigh(row_pins[row]);
    // Read the ADC value
    sw_value = adc_read(adcMux);
    writePinLow(row_pins[row]);
  }
  // Discharge peak hold capacitor
  writePinLow(DISCHARGE_PIN);
  key_scan_time = chSysGetRealtimeCounterX();
  return sw_value;
}

// Update press/release state of key
static inline bool ec_update_key(matrix_row_t* current_row, uint8_t row, uint8_t col, uint16_t sw_value) {
  uint16_t extremum = sw_value < ec_config.deadzone[row][col] ? ec_config.deadzone[row][col] : sw_value;
  if ((*current_row >> col) & 1) {
    // key pressed
    switch (ec_config.release[row][col].reference.mode) {
      case EC_RELEASE_MODE_STATIC:
        if (sw_value < ec_config.release[row][col].reference.value) {
          ec_config.extremum[row][col] = extremum;
          *current_row &= ~(1 << col);
          return true;
        }
        break;
      case EC_RELEASE_MODE_DYNAMIC:
        if (sw_value < ec_config.deadzone[row][col] ||
            (sw_value < ec_config.extremum[row][col] &&
             ec_config.extremum[row][col] - sw_value > ec_config.release[row][col].reference.value)) {
          ec_config.extremum[row][col] = extremum;
          *current_row &= ~(1 << col);
          return true;
        }
        break;
    }
    // Is key still moving down?
    if (extremum > ec_config.extremum[row][col]) {
      ec_config.extremum[row][col] = extremum;
    }

  } else {
    // key released

    // 14-15 bit: actuation_mode
    switch (ec_config.actuation[row][col].reference.mode) {
      case EC_ACTUATION_MODE_STATIC:
        if (sw_value > ec_config.actuation[row][col].reference.value) {
          *current_row |= (1 << col);
          ec_config.extremum[row][col] = extremum;
          return true;
        }
        break;
      case EC_ACTUATION_MODE_DYNAMIC:
        if (sw_value > ec_config.extremum[row][col] &&
            sw_value - ec_config.extremum[row][col] > ec_config.actuation[row][col].reference.value) {
          // Has key moved down enough to be pressed?
          ec_config.extremum[row][col] = extremum;
          *current_row |= (1 << col);
          return true;
        }
        break;
    }
    // Is key still moving up
    if (extremum < ec_config.extremum[row][col]) {
      ec_config.extremum[row][col] = extremum;
    }
  }
  return false;
}
