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

#include "ec_auto_calibration.h"
#include "ec_config.h"

// Pin and port array
static const uint32_t row_pins[] = MATRIX_ROW_PINS;
static const uint32_t amux_sel_pins[] = AMUX_SEL_PINS;
static const uint32_t amux_en_pins[] = AMUX_EN_PINS;
static const uint8_t matrix_col_channels[] = MATRIX_COL_CHANNELS;

static rtcnt_t last_key_scan_time;
// 3D matrix
static matrix_row_t matrix[MATRIX_PAGES][MATRIX_ROWS];
static matrix_row_t matrix_changed[MATRIX_PAGES][MATRIX_ROWS];
static matrix_row_t matrix_used[MATRIX_ROWS];
static adc_mux adcMux;

#define PRIMARY_MATRIX_PAGE 0
#define SUB_ACTION_MATRIX_PAGE 1

#define AMUX_SEL_PINS_COUNT (sizeof(amux_sel_pins) / sizeof(amux_sel_pins[0]))
#define EXPECTED_AMUX_SEL_PINS_COUNT ceil(log2(AMUX_MAX_COLS_COUNT)
// Checks for the correctness of the configuration
_Static_assert((sizeof(amux_en_pins) / sizeof(amux_en_pins[0])) == AMUX_COUNT,
               "AMUX_EN_PINS doesn't have the minimum number of bits required to enable all the "
               "multiplexers available");
// Check that number of select pins is enough to select all the channels
_Static_assert(AMUX_SEL_PINS_COUNT == EXPECTED_AMUX_SEL_PINS_COUNT), "AMUX_SEL_PINS doesn't have the minimum number of bits required address all the channels");
// Check that number of elements in AMUX_COL_CHANNELS_SIZES is enough to specify the number of
// chann1els for all the multiplexers available

#define BOTTOMING_READING_THRESHOLD 0xff

// 10us = 850 RTC
#define RTC_DISCHARGE_TIME US2RTC(REALTIME_COUNTER_CLOCK, DISCHARGE_TIME)
#define RTC_CHARGE_TIME 20UL

// quantum/keyboard.c
extern void matrix_scan_perf_task(void);
extern void switch_events(uint8_t row, uint8_t col, bool pressed);

// if row = 0 col =0, dummy reading for equalize discharge time
#define MATRIX_READ_LOOP(...)                                             \
  matrix_row_t col_mask = 1;                                              \
  for (int col = 0; col < MATRIX_COLS; col++, col_mask <<= 1) {           \
    select_col(col);                                                      \
    for (int row = 0; row < MATRIX_ROWS; row++) {                         \
      if (col == 0 && row == 0) ec_readkey(row_pins[row]);                \
      if (matrix_used[row] & col_mask) {                                  \
        uint16_t sw_value = ec_readkey(row_pins[row]);                    \
        ec_key_config_t *key = &ec_config_keys[row][col];                 \
        key->sw_value = sw_value;                                         \
        if (sw_value > key->bottoming_max) key->bottoming_max = sw_value; \
        __VA_ARGS__                                                       \
      }                                                                   \
    }                                                                     \
  }

//  inline functions
// -----------------------------------------------------------------------------------

static inline void select_col(uint8_t col) {
  static uint8_t amux_col_ch = 0;

  uint8_t changes = matrix_col_channels[col] ^ amux_col_ch;
  amux_col_ch = matrix_col_channels[col];
#if AMUX_COUNT >= 1
  if (changes & 0x10) writePin(amux_en_pins[0], amux_col_ch & 0x10);
#endif
#if AMUX_COUNT >= 2
  if (changes & 0x20) writePin(amux_en_pins[1], amux_col_ch & 0x20);
#endif
#if AMUX_COUNT >= 3
  if (changes & 0x40) writePin(amux_en_pins[2], amux_col_ch & 0x40);
#endif
#if AMUX_COUNT >= 4
  if (changes & 0x80) writePin(amux_en_pins[3], amux_col_ch & 0x80);
#endif
#if AMUX_COUNT >= 5
#  error Unsupported AMUX_COUNT, maximum is 4
#endif
  if (changes & 1) writePin(amux_sel_pins[0], amux_col_ch & 1);
  if (changes & 2) writePin(amux_sel_pins[1], amux_col_ch & 2);
  if (changes & 4) writePin(amux_sel_pins[2], amux_col_ch & 4);
}

static inline bool is_actuated(ec_key_config_t *key, uint16_t sw_value) {
  if (sw_value <= key->deadzone) return false;
  switch (key->modes.actuation_mode) {
    case EC_ACTUATION_MODE_STATIC_EDGE:
      // cross the threshold from top to bottom
      return key->extremum <= key->actuation_reference && sw_value > key->actuation_reference;
    case EC_ACTUATION_MODE_STATIC_LEVEL:
      return sw_value > key->actuation_reference;
    case EC_ACTUATION_MODE_DYNAMIC:
      return sw_value > key->extremum && sw_value - key->extremum > key->actuation_reference;
  }
  return false;
}

static inline bool is_released(ec_key_config_t *key, uint16_t sw_value) {
  if (sw_value <= key->deadzone) return true;
  switch (key->modes.release_mode) {
    case EC_RELEASE_MODE_STATIC_EDGE:
      // cross the threshold from bottom to top
      return key->extremum >= key->release_reference && sw_value < key->release_reference;
    case EC_RELEASE_MODE_STATIC_LEVEL:
      return sw_value < key->release_reference;
    case EC_RELEASE_MODE_DYNAMIC:
      return sw_value < key->extremum && key->extremum - sw_value > key->release_reference;
  }
  return false;
}
static inline bool is_sub_action_actuated(ec_key_config_t *key, uint16_t sw_value) {
  if (sw_value <= key->deadzone) return false;
  return sw_value > key->sub_action_actuation_threshold;
}

static inline bool is_sub_action_released(ec_key_config_t *key, uint16_t sw_value,
                                          bool primary_pressed) {
  if (sw_value <= key->deadzone) return true;
  switch (key->modes.sub_action_release_mode) {
    case EC_SUB_ACTION_RELEASE_MODE_SYNC_PRIMARY:
      return !primary_pressed;
    case EC_SUB_ACTION_RELEASE_MODE_USE_THRESHOLD:
      return sw_value < key->sub_action_release_threshold;
  }
  return false;
}

// static routines
// -----------------------------------------------------------------------------------

static uint16_t ec_readkey(uint32_t strobe_pin) {
  uint16_t sw_value;

  ATOMIC_BLOCK_FORCEON {
    // DISCHARGE_TIME 10us = 850 clock count
    while (TIMER_DIFF_32(chSysGetRealtimeCounterX(), last_key_scan_time) <
           (uint32_t)RTC_DISCHARGE_TIME) {
    }
    // charge peak hold capacitor
    writePinHigh(DISCHARGE_PIN);
    writePinHigh(strobe_pin);

    last_key_scan_time = chSysGetRealtimeCounterX();
    while (TIMER_DIFF_32(chSysGetRealtimeCounterX(), last_key_scan_time) <
           (uint32_t)RTC_CHARGE_TIME) {
    }
    // wait_us(CHARGE_TIME);
    // Read the ADC value
    sw_value = adc_read(adcMux);
    writePinLow(strobe_pin);
    // Discharge peak hold capacitor
    writePinLow(DISCHARGE_PIN);
    last_key_scan_time = chSysGetRealtimeCounterX();
  }
  return sw_value;
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

static void ec_initialize_noise_floor(void) {
  // Initialize the noise floor to 0
  MATRIX_LOOP_WITH_KEY(ec_eeprom_config.noise_floor[row][col] = 0; key->extremum = 0;)

  // Get the noise floor
  // max: ec_config.noise_floor[row][col]
  // min: ec_config.extremum[row][col]
  for (uint8_t sample_count = 0; sample_count < 5; sample_count++) {
    MATRIX_READ_LOOP(
      // max value
      if (sw_value > ec_eeprom_config.noise_floor[row][col]) {
        ec_eeprom_config.noise_floor[row][col] = sw_value;
      }
      // min value
      if (key->extremum == 0 || sw_value < key->extremum) { key->extremum = sw_value; });
  }

  // Average the noise floor
  MATRIX_LOOP_WITH_KEY(
    // noise = max - mini
    key->noise = ec_eeprom_config.noise_floor[row][col] - key->extremum;
    // noise_floor = (max + min) / 2
    ec_eeprom_config.noise_floor[row][col] =
      (ec_eeprom_config.noise_floor[row][col] + key->extremum) / 2;
    // initilize extremum
    key->extremum = ec_eeprom_config.noise_floor[row][col];);
}

// debug
// -----------------------------------------------------------------------------------
#ifdef EC_DEBUG_ENABLE
#  define CHARGE_STEP(plot_index) (rtcnt_t)(10UL * (plot_index))
#  define DISCHARGE_STEP(plot_index) US2RTC(REALTIME_COUNTER_CLOCK, plot_index)

static uint16_t ec_test_readkey(uint32_t strobe_pin, uint32_t charge_index,
                                uint32_t discharge_index) {
  uint16_t sw_value;

  ATOMIC_BLOCK_FORCEON {
    // DISCHARGE_TIME 10us = 850 clock count
    while (TIMER_DIFF_32(chSysGetRealtimeCounterX(), last_key_scan_time) <
           DISCHARGE_STEP(discharge_index)) {
    }
    // charge peak hold capacitor
    writePinHigh(DISCHARGE_PIN);
    writePinHigh(strobe_pin);

    if (charge_index) {
      last_key_scan_time = chSysGetRealtimeCounterX();
      while (TIMER_DIFF_32(chSysGetRealtimeCounterX(), last_key_scan_time) <
             CHARGE_STEP(charge_index)) {
      }
    }

    // Read the ADC value
    sw_value = adc_read(adcMux);

    writePinLow(strobe_pin);
    // Discharge peak hold capacitor
    writePinLow(DISCHARGE_PIN);
    last_key_scan_time = chSysGetRealtimeCounterX();
  }
  return sw_value;
}

static void matrix_scan_test_init(void) {
  for (uint8_t i = 0; i < EC_TEST_CHARGE_PLOT_COUNT; i++) {
    for (uint8_t j = 0; j < EC_TEST_DISCHARGE_PLOT_COUNT; j++) {
      ec_test_result_t *result = &ec_test_result[i][j];
      result->floor_min = 0xffff;
      result->floor_max = 0;
      result->bottom_max = 0;
    }
  }
}

static void matrix_scan_test(void) {
  static uint8_t charge_index;
  static uint8_t discharge_index;

  matrix_row_t col_mask = 1;
  for (int col = 0; col < MATRIX_COLS; col++, col_mask <<= 1) {
    select_col(col);
    for (int row = 0; row < MATRIX_ROWS; row++) {
      if (col == 0 && row == 0) ec_readkey(row_pins[row]);
      uint16_t sw_value = ec_test_readkey(row_pins[row], charge_index, discharge_index);
      if (matrix_used[row] & col_mask) {
        ec_test_result_t *result = &ec_test_result[charge_index][discharge_index];
        if (row == EC_TEST_FLOOR_ROW && col == EC_TEST_FLOOR_COL) {
          if (sw_value < result->floor_min) {
            result->floor_min = sw_value;
          }
          if (sw_value > result->floor_max) {
            result->floor_max = sw_value;
          }
        }
        if (row == EC_TEST_BOTTOM_ROW && col == EC_TEST_BOTTOM_COL) {
          if (sw_value > result->bottom_max) {
            result->bottom_max = sw_value;
          }
        }
      }
    }
  }

  charge_index++;
  charge_index %= EC_TEST_CHARGE_PLOT_COUNT;
  if (charge_index == 0) {
    discharge_index++;
    discharge_index %= EC_TEST_DISCHARGE_PLOT_COUNT;
  }
  wait_us(100);
}
#endif

//  implementation of CUSTOM_MATRIX=yes
// -----------------------------------------------------------------------------------

// Initialize the peripherals pins
void matrix_init(void) {
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

  memset(matrix, 0, sizeof(matrix));
  memset(matrix_changed, 0, sizeof(matrix_changed));
  memset(matrix_used, 0, sizeof(matrix_used));

  // scan rate increase 780 -> 950
  // but when layout changed, restart is needed.
  MATRIX_LOOP(if (dynamic_keymap_get_keycode(0, row, col)) { matrix_used[row] |= (1 << col); });

  // initial read for bootmagic
  last_key_scan_time = chSysGetRealtimeCounterX();
  // MATRIX_READ_LOOP(key->extremum = sw_value;)
  ec_initialize_noise_floor();
  ec_auto_calibration_init();
}

inline matrix_row_t matrix_get_row(uint8_t row) { return matrix[PRIMARY_MATRIX_PAGE][row]; }

uint8_t matrix_scan(void) {
  bool changed = false;

#ifdef EC_DEBUG_ENABLE
  static bool test_last_state;
  if (ec_matrix_scan_test_enable) {
    if (!test_last_state) {
      matrix_scan_test_init();
    }
    test_last_state = ec_matrix_scan_test_enable;
    matrix_scan_test();
    return false;
  }
  test_last_state = ec_matrix_scan_test_enable;
#endif

  // Normal operation mode: update key state
  MATRIX_READ_LOOP(
    uint16_t extremum = sw_value < key->deadzone ? key->deadzone : sw_value;  //
    matrix_row_t *primary_matrix_row = &matrix[PRIMARY_MATRIX_PAGE][row];     //
    if (*primary_matrix_row & col_mask) {
      if (is_released(key, sw_value)) {
        *primary_matrix_row &= ~col_mask;
        matrix_changed[PRIMARY_MATRIX_PAGE][row] |= col_mask;
        key->extremum = extremum;
        changed = true;
      } else {
        matrix_changed[PRIMARY_MATRIX_PAGE][row] &= ~col_mask;
        // Is key still moving down
        if (extremum > key->extremum) {
          key->extremum = extremum;
        }
      }
    } else {
      if (is_actuated(key, sw_value)) {
        *primary_matrix_row |= col_mask;
        matrix_changed[PRIMARY_MATRIX_PAGE][row] |= col_mask;
        key->extremum = extremum;
        key->actuation_count++;
        changed = true;
      } else {
        matrix_changed[PRIMARY_MATRIX_PAGE][row] &= ~col_mask;
        // Is key still moving up
        if (extremum < key->extremum) {
          key->extremum = extremum;
        }
      }
    }
    // sub action
    if (key->sub_action_keycode != KC_NO) {
      matrix_row_t *sub_matrix_row = &matrix[SUB_ACTION_MATRIX_PAGE][row];  //
      if (*sub_matrix_row & col_mask) {
        if (is_sub_action_released(key, sw_value, *primary_matrix_row & col_mask)) {
          *sub_matrix_row &= ~col_mask;
          matrix_changed[SUB_ACTION_MATRIX_PAGE][row] |= col_mask;
          changed = true;
        } else {
          matrix_changed[SUB_ACTION_MATRIX_PAGE][row] &= ~col_mask;
        }
      } else {
        if (is_sub_action_actuated(key, sw_value)) {
          *sub_matrix_row |= col_mask;
          matrix_changed[SUB_ACTION_MATRIX_PAGE][row] |= col_mask;
          changed = true;
        } else {
          matrix_changed[SUB_ACTION_MATRIX_PAGE][row] &= ~col_mask;
        }
      }
    } else {
      matrix_changed[SUB_ACTION_MATRIX_PAGE][row] &= ~col_mask;  //
    });
  return changed;
}

bool custom_matrix_task(void) {
  bool changed = matrix_scan();
  static bool all_released = true;

#if defined(DEBUG_MATRIX_SCAN_RATE)
  // in keyboard.c
  matrix_scan_perf_task();
#endif
  // Short-circuit the complete matrix processing if it is not necessary
  if (changed) {
    all_released = true;
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
      const matrix_row_t current_row = matrix[PRIMARY_MATRIX_PAGE][row];
      const matrix_row_t row_changes = matrix_changed[PRIMARY_MATRIX_PAGE][row];
      if (current_row) all_released = false;
      if (!row_changes) continue;

      matrix_row_t col_mask = 1;
      for (uint8_t col = 0; col < MATRIX_COLS; col++, col_mask <<= 1) {
        if (!(row_changes & col_mask)) continue;

        const bool key_pressed = current_row & col_mask;
        action_exec(MAKE_KEYEVENT(row, col, key_pressed));

#if defined(LED_MATRIX_ENABLE) || defined(RGB_MATRIX_ENABLE)
        // in quantum/keyboard.c
        switch_events(row, col, key_pressed);
#endif
      }
    }

    // Sub action
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
      const matrix_row_t current_row = matrix[SUB_ACTION_MATRIX_PAGE][row];
      const matrix_row_t row_changes = matrix_changed[SUB_ACTION_MATRIX_PAGE][row];
      if (current_row) all_released = false;
      if (!row_changes) continue;

      matrix_row_t col_mask = 1;
      for (uint8_t col = 0; col < MATRIX_COLS; col++, col_mask <<= 1) {
        if (!(row_changes & col_mask)) continue;

        const bool key_pressed = current_row & col_mask;
        // TODO experimental
        ec_key_config_t *key = &ec_config_keys[row][col];
        keyrecord_t record = {
          .event = MAKE_KEYEVENT(row, col, key_pressed),
          // requires ACTION_FOR_KEYCODE_ENABLE = yes
          .keycode = key->sub_action_keycode,
        };
        process_record(&record);
      }
    }
  } else {
    // in quantum/keyboard.c
    // static inline void generate_tick_event(void)
    static uint16_t last_tick = 0;
    const uint16_t now = timer_read();
    if (TIMER_DIFF_16(now, last_tick) != 0) {
      action_exec(MAKE_TICK_EVENT);
      last_tick = now;
    }
  }

  ec_auto_calibration_task(all_released);
  return changed;
}
