
#pragma once
#include <quantum.h>

// mode max 4 modes
typedef enum ec_actuation_mode { EC_ACTUATION_MODE_STATIC = 0, EC_ACTUATION_MODE_DYNAMIC } ec_actuation_mode_t;

// mode max 4 modes
typedef enum ec_release_mode { EC_RELEASE_MODE_STATIC = 0, EC_RELEASE_MODE_DYNAMIC } ec_release_mode_t;

typedef struct {
  ec_actuation_mode_t actuation_mode : 2;
  ec_release_mode_t release_mode : 2;
  uint8_t reserved_0 : 4;
  // 0 | 100% (1023) of Total Travel
  uint16_t actuation_threshold : 10;
  // 0 | 100% (1023) of Total Travel
  uint16_t release_threshold : 10;

  ec_release_mode_t reserved_1 : 4;

  // 0 | 50% (511) of Total Travel
  uint16_t actuation_travel : 9;
  // 0 | 50% (511) of Total Travel
  uint16_t release_travel : 9;
  // 0 | 50% (511) of Total Travel
  uint16_t deadzone : 9;
} __attribute__((packed)) ec_preset_t;

typedef struct {
  ec_preset_t presets[EC_NUM_PRESETS];
  uint16_t bottoming_reading[MATRIX_ROWS][MATRIX_COLS];
  uint8_t preset_map : 3;  // 0 - 7
  uint16_t reserved_0 : 13;
} __attribute__((packed)) ec_eeprom_config_t;

/* eeprom address */
#define EC_VIA_EEPROM_PRESETS VIA_EEPROM_CUSTOM_CONFIG_USER_ADDR
#define EC_VIA_EEPROM_BOTTOMING_READING (EC_VIA_EEPROM_PRESETS + sizeof(ec_preset_t) * EC_NUM_PRESETS)
#define EC_VIA_EEPROM_PRESET_MAP (EC_VIA_EEPROM_BOTTOMING_READING + 2 * MATRIX_ROWS * MATRIX_COLS)

// Check if the size of the reserved persistent memory is the same as the size of struct eeprom_ec_config_t
_Static_assert(sizeof(ec_preset_t) == 8, "Mismatch in keyboard EECONFIG stored data");
_Static_assert(sizeof(ec_eeprom_config_t) == (VIA_EEPROM_CUSTOM_CONFIG_SIZE - VIA_EEPROM_CUSTOM_CONFIG_COMMON_SIZE),
               "Mismatch in keyboard EECONFIG stored data");

typedef struct {
  ec_actuation_mode_t actuation_mode;
  uint16_t actuation_reference;  // threshold or travel
  ec_release_mode_t release_mode;
  uint16_t release_reference;  // threshold or travel
  uint16_t deadzone;
  uint16_t extremum;
  uint16_t noise_floor;
  uint16_t noise;
#ifdef EC_DEBUG
  uint16_t sw_value;
#endif
  bool bottoming_calibration_starter;
} ec_key_config_t;

extern ec_key_config_t ec_config_keys[MATRIX_ROWS][MATRIX_COLS];
extern ec_eeprom_config_t ec_eeprom_config;
extern bool bottoming_calibration;

void ec_config_reset(void);
void ec_config_init(void);
void ec_config_update_key(uint8_t row, uint8_t col);
void ec_config_set_actuation_mode(uint8_t preset_index, ec_actuation_mode_t actuation_mode);
void ec_config_set_release_mode(uint8_t preset_index, ec_release_mode_t release_mode);
void ec_config_set_actuation_threshold(uint8_t preset_index, uint16_t actuation_threshold);
void ec_config_set_release_threshold(uint8_t preset_index, uint16_t release_threshold);
void ec_config_set_actuation_travel(uint8_t preset_index, uint16_t actuation_travel);
void ec_config_set_release_travel(uint8_t preset_index, uint16_t release_travel);
void ec_config_set_deadzone(uint8_t preset_index, uint16_t deadzone);
void ec_config_set_preset_map(uint8_t preset_map_index);
void ec_config_start_calibration(void);
void ec_config_end_calibration(void);
void ec_config_send_calibration_data(uint32_t delay_ms);
void ec_config_send_presets(uint32_t delay_ms);
void ec_config_send_preset_map(uint32_t delay_ms);
#ifdef EC_DEBUG
void ec_config_debug_send_config(uint32_t delay_ms);
#endif

#define MATRIX_LOOP(...)                              \
  for (uint8_t row = 0; row < MATRIX_ROWS; row++) {   \
    for (uint8_t col = 0; col < MATRIX_COLS; col++) { \
      __VA_ARGS__                                     \
    }                                                 \
  }

#define MATRIX_LOOP_WITH_KEY(...)                       \
  for (uint8_t row = 0; row < MATRIX_ROWS; row++) {     \
    for (uint8_t col = 0; col < MATRIX_COLS; col++) {   \
      ec_key_config_t *key = &ec_config_keys[row][col]; \
      __VA_ARGS__                                       \
    }                                                   \
  }
