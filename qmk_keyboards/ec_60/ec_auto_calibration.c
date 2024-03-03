#include "ec_auto_calibration.h"

#include "ec_config.h"
#include "lib/qsort.h"

#define NUM_NOISE_FLOOR_SAMPLES 31
#define IDLE_TIME 500
#define NUM_CALIBRATION_KEYS 10
#define ACTUATION_COUNT 32
#define BOTTOMING_UPDATE_COUNT 256

static uint16_t noise_floor_samples[NUM_NOISE_FLOOR_SAMPLES];
static uint32_t idle_timer;

static uint8_t sample_count;
static uint8_t key_count;
static uint8_t start_row;
static uint8_t start_col;
static uint8_t row;
static uint8_t col;

uint16_t bottoming_update_count;

static bool sample_noise_floor(void);
static void update_keys(void);

static uint16_t _tmp;
#define LESS(i, j) noise_floor_samples[i] < noise_floor_samples[j]
#define SWAP(i, j)                                                                \
  _tmp = noise_floor_samples[i], noise_floor_samples[i] = noise_floor_samples[j], \
  noise_floor_samples[j] = _tmp

#define KEY_NEXT                     \
  col++;                             \
  if (col >= MATRIX_COLS) {          \
    col = 0;                         \
    row++;                           \
    if (row >= MATRIX_ROWS) row = 0; \
  }

void ec_auto_calibration_init(void) { idle_timer = timer_read32(); }

void ec_auto_calibration_task(bool all_released) {
  static bool in_sampling = false;
  static bool end_sampling = false;
  uint32_t timer_now = timer_read32();

  //  idle time = IDLE_TIME ms
  //  sampling = matrix_scan_time (roughly 1ms) * NUM_CALIBRATION_KEYS * NUM_NOISE_FLOOR_SAMPLES
  //
  //  |------idle time-----|----sampling-----|------idle time-----|---- sampling
  //                        -----------------
  //                                |---------------------------->|
  //                                                              |
  //                                                              update_keys()
  // -----------------------------------------------------------------------------
  //                         |
  // <---------------------- | press any key
  //
  if (!all_released) {
    in_sampling = false;
    idle_timer = timer_now;
    row = start_row;
    col = start_col;
    sample_count = 0;
    key_count = 0;
    end_sampling = false;
    return;
  }

  if (in_sampling) {
    if (sample_noise_floor()) {
      end_sampling = true;
      idle_timer = timer_now;
      in_sampling = false;
    }
  } else {
    if (TIMER_DIFF_32(timer_now, idle_timer) >= IDLE_TIME) {
      if (end_sampling) {
        end_sampling = false;
        row = start_row;
        col = start_col;
        update_keys();
        start_row = row;
        start_col = col;
      }
      sample_count = 0;
      key_count = 0;
      in_sampling = true;
    }
  }
}

static bool sample_noise_floor() {
  ec_key_config_t *key = &ec_config_keys[row][col];
  noise_floor_samples[sample_count] = key->sw_value;
  sample_count++;
  if (sample_count >= NUM_NOISE_FLOOR_SAMPLES) {
    // calculate median value
    // https://github.com/svpv/qsort
    QSORT(NUM_NOISE_FLOOR_SAMPLES, LESS, SWAP);
    ec_eeprom_config.noise_floor[row][col] = noise_floor_samples[NUM_NOISE_FLOOR_SAMPLES >> 1];
    // noise = max - min
    key->noise = noise_floor_samples[NUM_NOISE_FLOOR_SAMPLES - 1] - noise_floor_samples[0];
    sample_count = 0;
    key_count++;
    KEY_NEXT;
  }
  return key_count >= NUM_CALIBRATION_KEYS;
}

static void update_keys() {
  for (uint8_t i = 0; i < NUM_CALIBRATION_KEYS; i++) {
    ec_key_config_t *key = &ec_config_keys[row][col];
    if (key->actuation_count >= ACTUATION_COUNT) {
      ec_eeprom_config.bottoming_reading[row][col] = key->bottoming_max - (key->noise >> 1);
      key->actuation_count = 0;
      key->bottoming_max = 0;
      bottoming_update_count++;
    }
    ec_config_update_key(row, col);
    KEY_NEXT;
  }
  // TODO how often should save
  if (bottoming_update_count >= BOTTOMING_UPDATE_COUNT) {
    ec_config_save_calibration_data();
    // TODO temporay comment out for monitoring counter.
    // only once
    // update_count = 0;
  }
}
