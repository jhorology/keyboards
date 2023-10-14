
#pragma once
#include <quantum.h>

#include "lib/via_custom_menus.h"

// mode max 4 modes
typedef enum ec_actuation_mode { EC_ACTUATION_MODE_STATIC = 0, EC_ACTUATION_MODE_DYNAMIC } ec_actuation_mode_t;

// mode max 4 modes
typedef enum ec_release_mode { EC_RELEASE_MODE_STATIC = 0, EC_RELEASE_MODE_DYNAMIC } ec_release_mode_t;

typedef struct {
  ec_actuation_mode_t actuation_mode : 2;  // +0
  ec_release_mode_t release_mode : 2;      // +2
  uint8_t _reserved_0 : 1;                 // +4
  uint16_t actuation_travel : 9;           // +5  0 | 50% (511) of Total Travel
  uint16_t release_travel : 9;             // +14 0 | 50% (511) of Total Travel
  uint16_t deadzone : 9;                   // +23 0 | 50% (511) of Total Travel

  uint16_t actuation_threshold : 10;             // +32  0 | 100% (1023) of Total Travel
  uint16_t release_threshold : 10;               // +42 0 | 100% (1023) of Total Travel
  bool sub_action_enable : 1;                    // +52
  uint8_t _reserved_1 : 1;                       // +53
  uint16_t sub_action_actuation_threshold : 10;  // +54 0 | 100% (1023) of Total Travel

  uint16_t sub_action_keycode : 16;            // +64
  uint16_t sub_action_release_threshold : 10;  // +80 0 | 100% (1023) of Total Travel
  uint8_t _reserved_2 : 6;                     // +90
  // total 96bit = dword x 3 = 12bytes
} __attribute__((packed)) ec_preset_t;

typedef struct {
  ec_preset_t presets[EC_NUM_PRESETS];
  uint16_t bottoming_reading[MATRIX_ROWS][MATRIX_COLS];
  uint8_t selected_preset_map_index : 3;  // 0 - 7
  bool debouncing_enable : 1;
  uint16_t reserved_0 : 12;
} __attribute__((packed)) ec_eeprom_config_t;

/* eeprom address */
#define EC_VIA_EEPROM_PRESETS VIA_EEPROM_CUSTOM_CONFIG_USER_ADDR
#define EC_VIA_EEPROM_BOTTOMING_READING (EC_VIA_EEPROM_PRESETS + sizeof(ec_preset_t) * EC_NUM_PRESETS)
#define EC_VIA_EEPROM_PRESET_MAP (EC_VIA_EEPROM_BOTTOMING_READING + 2 * MATRIX_ROWS * MATRIX_COLS)

// Check if the size of the reserved persistent memory is the same as the size of struct eeprom_ec_config_t
_Static_assert(sizeof(ec_preset_t) == VIA_EC_PRESET_SIZE, "Mismatch in keyboard eeprom confiiguration");
_Static_assert(sizeof(ec_eeprom_config_t) == (VIA_EEPROM_CUSTOM_CONFIG_SIZE - VIA_EEPROM_CUSTOM_CONFIG_COMMON_SIZE),
               "Mismatch in keyboard eeprom configuration");

typedef struct {
  ec_actuation_mode_t actuation_mode;
  uint16_t actuation_reference;  // threshold or travel
  ec_release_mode_t release_mode;
  uint16_t release_reference;  // threshold or travel
  uint16_t deadzone;
  uint16_t extremum;
  uint16_t sub_action_keycode;
  uint16_t sub_action_actuation_threshold;
  uint16_t sub_action_release_threshold;
  uint16_t noise_floor;
  uint16_t noise;
#ifdef EC_DEBUG_ENABLE
  uint16_t sw_value;
#endif
  bool bottoming_calibration_starter;
} ec_key_config_t;

extern ec_key_config_t ec_config_keys[MATRIX_ROWS][MATRIX_COLS];
extern ec_eeprom_config_t ec_eeprom_config;
extern bool ec_bottoming_calibration_enable;
#ifdef EC_DEBUG_ENABLE
extern bool ec_test_discharge_enable;
extern uint16_t ec_test_discharge_floor_min[EC_TEST_DISCHARGE_MAX_TIME_US + 1];
extern uint16_t ec_test_discharge_floor_max[EC_TEST_DISCHARGE_MAX_TIME_US + 1];
extern uint16_t ec_test_discharge_bottom_max[EC_TEST_DISCHARGE_MAX_TIME_US + 1];
#endif

void ec_config_reset(void);
void ec_config_init(void);
void ec_config_update_key(uint8_t row, uint8_t col);

void ec_config_set_actuation_mode(uint8_t preset_index, ec_actuation_mode_t value);
void ec_config_set_release_mode(uint8_t preset_index, ec_release_mode_t value);
void ec_config_set_actuation_threshold(uint8_t preset_index, uint16_t value);
void ec_config_set_release_threshold(uint8_t preset_index, uint16_t value);
void ec_config_set_actuation_travel(uint8_t preset_index, uint16_t value);
void ec_config_set_release_travel(uint8_t preset_index, uint16_t value);
void ec_config_set_deadzone(uint8_t preset_index, uint16_t value);
void ec_config_set_sub_action_enable(uint8_t preset_index, bool value);
void ec_config_set_sub_action_keycode(uint8_t preset_index, uint16_t value);
void ec_config_set_sub_action_actuation_threshold(uint8_t preset_index, uint16_t value);
void ec_config_set_sub_action_release_threshold(uint8_t preset_index, uint16_t value);

void ec_config_set_preset_map(uint8_t preset_map_index);
void ec_config_calibration_start(void);
void ec_config_calibration_end(void);
void ec_config_send_calibration_data(void);
void ec_config_send_presets(void);
#ifdef EC_DEBUG_ENABLE
void ec_config_debug_send_debug_values(void);
void ec_config_debug_send_calibration(void);
void ec_config_debug_send_config_keys(void);
void ec_config_debug_send_all(void);
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

#define EC_HALF_SCALE_RANGE (EC_SCALE_RANGE >> 1)
#define EC_SAFETY_MARGIN (EC_SAFETY_MARGIN_PERC * EC_SCALE_RANGE / 100)
#define EC_WITHIN_SCALE_RANGE(v) (v >= EC_SAFETY_MARGIN && v <= EC_SCALE_RANGE)
#define EC_WITHIN_HALF_SCALE_RANGE(v) (v >= EC_SAFETY_MARGIN && v <= EC_HALF_SCALE_RANGE)

// convtert a percentage of tatal travel into preset
#define EC_PERC(perc) ((perc) * EC_SCALE_RANGE / 100)

// clang-format off
#define EC_STATIC_PRESET(a, r) {                 \
  .actuation_mode = EC_ACTUATION_MODE_STATIC,    \
  .release_mode = EC_RELEASE_MODE_STATIC,        \
  .actuation_threshold = a,                      \
  .release_threshold = r,                        \
  .actuation_travel = EC_PERC(15),               \
  .release_travel = EC_PERC(15),                 \
  .deadzone = EC_PERC(15),                       \
  .sub_action_enable = false,                    \
  .sub_action_keycode = KC_NO,                   \
  .sub_action_actuation_threshold = EC_PERC(90), \
  .sub_action_release_threshold = EC_PERC(75)    \
}
#define EC_STATIC_PRESET_PERC(a, r) EC_STATIC_PRESET(EC_PERC(a), EC_PERC(r))
#define EC_STATIC_PRESET_DEFAULT EC_STATIC_PRESET_PERC(45, 30)


#define EC_DYNAMIC_PRESET(a, r, d) {             \
  .actuation_mode = EC_ACTUATION_MODE_DYNAMIC,   \
  .release_mode = EC_RELEASE_MODE_DYNAMIC,       \
  .actuation_threshold = EC_PERC(40),            \
  .release_threshold = EC_PERC(30),              \
  .actuation_travel = a,                         \
  .release_travel = r,                           \
  .deadzone = d,                                 \
  .sub_action_enable = false,                    \
  .sub_action_keycode = KC_NO,                   \
  .sub_action_actuation_threshold = EC_PERC(90), \
  .sub_action_release_threshold = EC_PERC(75)    \
}
#define EC_DYNAMIC_PRESET_PERC(a, r, d) EC_DYNAMIC_PRESET(EC_PERC(a), EC_PERC(r), EC_PERC(d))
#define EC_DYNAMIC_PRESET_DEFAULT EC_DYNAMIC_PRESET_PERC(15, 15, 20)

// clang-format on
