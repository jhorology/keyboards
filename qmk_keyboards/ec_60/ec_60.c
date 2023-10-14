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

#include "ec_60.h"

#include "ec_config.h"
#include "ec_switch_matrix.h"

#if EC_SCALE_RANGE < 0x100
#  define VIA_READ_EC_RANGE_VALUE(command) via_read_range_byte_value(command)
#  define VIA_WRITE_EC_RANGE_VALUE(command, value) via_write_range_byte_value(command, value)
#else
#  define VIA_READ_EC_RANGE_VALUE(command) via_read_range_word_value(command)
#  define VIA_WRITE_EC_RANGE_VALUE(command, value) via_write_range_word_value(command, value)
#endif

#if EC_SCALE_RANGE < 0x200
#  define VIA_READ_EC_HALF_RANGE_VALUE(command) via_read_range_byte_value(command)
#  define VIA_WRITE_EC_HALF_RANGE_VALUE(command, value) via_write_range_byte_value(command, value)
#else
#  define VIA_READ_EC_HALF_RANGE_VALUE(command) via_read_range_word_value(command)
#  define VIA_WRITE_EC_HALF_RANGE_VALUE(command, value) via_write_range_word_value(command, value)
#endif

enum {
  id_ec_tools_channel = id_custom_channel_user_range,
  id_ec_preset_channel_start = id_ec_tools_channel + 1,
  id_ec_preset_channel_end = id_ec_preset_channel_start + EC_NUM_PRESETS - 1,
};

_Static_assert(id_ec_tools_channel == EC_VIA_CUSTOM_CHANNEL_ID_START,
               "Mismatch in via custom menu channel");

enum via_ec_tools_value_id {
  id_ec_tools_bottoming_calibration = 1,
  id_ec_tools_show_calibration_data,
  id_ec_tools_test_discharge,
  id_ec_tools_debug_send_config,
  id_ec_tools_bootloader_jump
};

// Declaring enums for VIA config menu
enum via_ec_preset_value_id {
  // clang-format off
  id_ec_preset_actuation_mode = 1,
  id_ec_preset_actuation_threshold,
  id_ec_preset_actuation_travel,
  id_ec_preset_release_mode,
  id_ec_preset_release_threshold,
  id_ec_preset_release_travel,
  id_ec_preset_deadzone,
  id_ec_preset_sub_action_enable,
  id_ec_preset_sub_action_keycode,
  id_ec_preset_sub_action_actuation_threshold,
  id_ec_preset_sub_action_release_mode,
  id_ec_preset_sub_action_release_threshold
  // clang-format on
};

static deferred_token send_data_token;  // defer_exec token

typedef void (*send_data_func)(void);

// static routines
// -----------------------------------------------------------------------------------

uint32_t send_data_cb(uint32_t trigger_time, send_data_func fn) {
  fn();
  send_data_token = 0;
  return 0;
}

static void cancel_send_data(void) {
  if (send_data_token) {
    cancel_deferred_exec(send_data_token);
  }
}
static void send_data(uint32_t delay, void (*send_data_func)(void)) {
  cancel_send_data();
  if (send_data_token) {
    cancel_deferred_exec(send_data_token);
  }
  send_data_token = defer_exec(delay, (uint32_t(*)(uint32_t, void *))send_data_cb, send_data_func);
}

// QMK hook functions
// -----------------------------------------------------------------------------------

// Bootmagic overriden to avoid conflicts with EC
void bootmagic_lite(void) {
#ifdef EC_BOOTMAGIC_LITE_THRESHOLD
  if (ec_config_keys[BOOTMAGIC_LITE_ROW][BOOTMAGIC_LITE_COLUMN].noise_floor >
      EC_BOOTMAGIC_LITE_THRESHOLD) {
    eeconfig_disable();
    // Jump to bootloader.
    bootloader_jump();
  }
#endif
}

void eeconfig_init_user(void) {  // Default values
  ec_config_reset();
}

// On Keyboard startup
void keyboard_post_init_user(void) { ec_config_init(); }

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  switch (keycode) {
    case EC_CALD:
      if (!record->event.pressed) {
        // 2 secods after release
        send_data(2000, ec_config_send_calibration_data);
      }
      return false;
    case EC_PSET:
      if (!record->event.pressed) {
        // 2 secods after release
        send_data(2000, ec_config_send_presets);
      }
      return false;
    case EC_PRESET_MAP_START ... EC_PRESET_MAP_END:
      if (record->event.pressed) {
        uint8_t preset_map_index = keycode - EC_PRESET_MAP_START;
        ec_config_set_preset_map(preset_map_index);
      }
      return false;
    case EC_PRESET_START ... EC_PRESET_END:
      // not keycode
      return false;
#ifdef EC_DEBUG_ENABLE
    case EC_DBG0:
      if (!record->event.pressed) {
        // 2 secods after release
        send_data(2000, ec_config_debug_send_debug_values);
      }
      return false;
    case EC_DBG1:
      if (!record->event.pressed) {
        // 2 secods after release
        send_data(2000, ec_config_debug_send_calibration);
      }
      return false;
    case EC_DBG2:
      if (!record->event.pressed) {
        // 2 secods after release
        send_data(2000, ec_config_debug_send_config_keys);
      }
      return false;
#endif
  }
  return true;
}

// common lib hook functions
// -----------------------------------------------------------------------------------
void via_raw_hid_post_receive_user(uint8_t *data, uint8_t length) {
  via_custom_command_t command = VIA_CUSTOM_COMMAND(data);
  switch (command.command_id) {
    case id_dynamic_keymap_set_keycode: {
      uint8_t layer = command.data[0];
      uint8_t row = command.data[1];
      uint8_t col = command.data[2];
      if ((layer - EC_PRESET_MAP_START) == ec_eeprom_config.selected_preset_map_index) {
        ec_config_update_key(row, col);
      }
      break;
    }
    case id_dynamic_keymap_reset: {
      ec_config_init();
      break;
    }
  }
}

bool via_custom_value_command_user(via_custom_command_t *command) {
  switch (command->channel_id) {
    case id_ec_tools_channel:
      switch (command->command_id) {
        case id_custom_set_value:
          switch (command->value_id) {
            case id_ec_tools_bottoming_calibration:
              if (via_read_toggle_value(command)) {
                ec_config_calibration_start();
              } else {
                ec_config_calibration_end();
              }
              return false;
            case id_ec_tools_show_calibration_data:
              if (via_read_toggle_value(command)) {
                send_data(3000, ec_config_send_calibration_data);
              }
              return false;
#ifdef EC_DEBUG_ENABLE
            case id_ec_tools_test_discharge: {
              if (via_read_toggle_value(command)) {
                for (uint8_t i = 0; i <= EC_TEST_DISCHARGE_MAX_TIME_US; i++) {
                  ec_test_discharge_floor_min[i] = 0x3ff;
                  ec_test_discharge_floor_max[i] = 0;
                  ec_test_discharge_bottom_max[i] = 0;
                }
                ec_test_discharge_enable = true;
              } else {
                ec_test_discharge_enable = false;
              }
              return false;
            }
            case id_ec_tools_debug_send_config:
              if (via_read_toggle_value(command)) {
                send_data(3000, ec_config_debug_send_all);
              }
              return false;
            case id_ec_tools_bootloader_jump:
              if (via_read_toggle_value(command)) {
                bootloader_jump();
              }
              return false;
#endif
          }
          break;

        case id_custom_get_value:
          via_write_toggle_value(command, false);
          return false;

        case id_custom_save:
          // nothing to do
          return false;
      }
      // EC Presets
    case id_ec_preset_channel_start ... id_ec_preset_channel_end: {
      uint8_t preset_index = command->channel_id - id_ec_preset_channel_start;
      switch (command->command_id) {
          // EC Presets set value
        case id_custom_set_value:
          switch (command->value_id) {
            case id_ec_preset_actuation_mode:
              SET_PRESET_PARAM(actuation_mode, preset_index, via_read_dropdown_value(command));
              return false;
            case id_ec_preset_actuation_threshold:
              SET_PRESET_PARAM(actuation_threshold, preset_index, VIA_READ_EC_RANGE_VALUE(command));
              return false;
            case id_ec_preset_actuation_travel:
              SET_PRESET_PARAM(actuation_travel, preset_index,
                               VIA_READ_EC_HALF_RANGE_VALUE(command));
              return false;
            case id_ec_preset_release_mode:
              SET_PRESET_PARAM(release_mode, preset_index, via_read_dropdown_value(command));
              return false;
            case id_ec_preset_release_threshold:
              SET_PRESET_PARAM(release_threshold, preset_index, VIA_READ_EC_RANGE_VALUE(command));
              return false;
            case id_ec_preset_release_travel:
              SET_PRESET_PARAM(release_travel, preset_index, VIA_READ_EC_HALF_RANGE_VALUE(command));
              return false;
            case id_ec_preset_deadzone:
              SET_PRESET_PARAM(deadzone, preset_index, VIA_READ_EC_HALF_RANGE_VALUE(command));
              return false;
            case id_ec_preset_sub_action_enable:
              SET_PRESET_PARAM(sub_action_enable, preset_index, via_read_toggle_value(command));
              return false;
            case id_ec_preset_sub_action_keycode:
              SET_PRESET_PARAM(sub_action_keycode, preset_index, via_read_keycode_value(command));
              return false;
            case id_ec_preset_sub_action_actuation_threshold:
              SET_PRESET_PARAM(sub_action_actuation_threshold, preset_index,
                               VIA_READ_EC_RANGE_VALUE(command));
              return false;
            case id_ec_preset_sub_action_release_mode:
              SET_PRESET_PARAM(sub_action_release_threshold, preset_index,
                               via_read_dropdown_value(command));
              return false;
            case id_ec_preset_sub_action_release_threshold:
              SET_PRESET_PARAM(sub_action_release_threshold, preset_index,
                               VIA_READ_EC_RANGE_VALUE(command));
              return false;
          }
          break;

        case id_custom_get_value: {
          ec_preset_t *preset = &ec_eeprom_config.presets[preset_index];
          switch (command->value_id) {
            case id_ec_preset_actuation_mode:
              via_write_dropdown_value(command, preset->actuation_mode);
              return false;
            case id_ec_preset_actuation_threshold:
              VIA_WRITE_EC_RANGE_VALUE(command, preset->actuation_threshold);
              return false;
            case id_ec_preset_actuation_travel:
              VIA_WRITE_EC_HALF_RANGE_VALUE(command, preset->actuation_travel);
              return false;
            case id_ec_preset_release_mode:
              via_write_dropdown_value(command, preset->release_mode);
              return false;
            case id_ec_preset_release_threshold:
              VIA_WRITE_EC_RANGE_VALUE(command, preset->release_threshold);
              return false;
            case id_ec_preset_release_travel:
              VIA_WRITE_EC_HALF_RANGE_VALUE(command, preset->release_travel);
              return false;
            case id_ec_preset_deadzone:
              VIA_WRITE_EC_HALF_RANGE_VALUE(command, preset->deadzone);
              return false;
            case id_ec_preset_sub_action_enable:
              via_write_toggle_value(command, preset->sub_action_enable);
              return false;
            case id_ec_preset_sub_action_keycode:
              via_write_keycode_value(command, preset->sub_action_keycode);
              return false;
            case id_ec_preset_sub_action_actuation_threshold:
              VIA_WRITE_EC_RANGE_VALUE(command, preset->sub_action_actuation_threshold);
              return false;
            case id_ec_preset_sub_action_release_mode:
              via_write_dropdown_value(command, preset->sub_action_release_mode);
              return false;
            case id_ec_preset_sub_action_release_threshold:
              VIA_WRITE_EC_RANGE_VALUE(command, preset->sub_action_release_threshold);
              return false;
          }
          break;
        }
        case id_custom_save:
          // nothing to do
          return false;
      }
      break;
    }
  }
  // unhandled
  return true;
}
