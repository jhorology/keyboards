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

#if EC_SCALE_RANGE >= 0x100
#  define VIA_READ_EC_RANGE_VALUE(command) via_read_range_word_value(command)
#  define VIA_WRITE_EC_RANGE_VALUE(command, value) via_write_range_word_value(command, value)
#else
#  define VIA_READ_EC_RANGE_VALUE(command) via_read_range_byte_value(command)
#  define VIA_WRITE_EC_RANGE_VALUE(command, value) via_write_range_byte_value(command, value)
#endif

#if EC_SCALE_RANGE >= 0x200
#  define VIA_READ_EC_HALF_RANGE_VALUE(command) via_read_range_word_value(command)
#  define VIA_WRITE_EC_HALF_RANGE_VALUE(command, value) via_write_range_word_value(command, value)
#else
#  define VIA_READ_EC_HALF_RANGE_VALUE(command) via_read_range_byte_value(command)
#  define VIA_WRITE_EC_HALF_RANGE_VALUE(command, value) via_write_range_byte_value(command, value)
#endif

// QMK hook functions
// -----------------------------------------------------------------------------------

// Bootmagic overriden to avoid conflicts with EC
void bootmagic_lite(void) {
#ifdef EC_BOOTMAGIC_LITE_THRESHOLD
  if (ec_config_keys[BOOTMAGIC_LITE_ROW][BOOTMAGIC_LITE_COLUMN].noise_floor > EC_BOOTMAGIC_LITE_THRESHOLD) {
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
        // 3 secods after release
        ec_config_send_calibration_data(3000);
      }
      return false;
    case EC_PSET:
      if (!record->event.pressed) {
        // 3 secods after release
        ec_config_send_presets(3000);
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
#ifdef EC_DEBUG
    case EC_DBG_DT:
      if (!record->event.pressed) {
        // 3 secods after release
        ec_config_debug_send_config(3000);
      }
      return false;
    case EC_DBG_FQ:
      if (record->event.pressed) {
        // 3 secods after release
        uint32_t scan_freq = last_matrix_scan_count;
        send_string("matrix scan frequency: 0x");
        send_dword(scan_freq);
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
                ec_config_start_calibration();
              } else {
                ec_config_end_calibration();
              }
              return false;
            case id_ec_tools_show_calibration_data:
              if (via_read_toggle_value(command)) {
                ec_config_send_calibration_data(3000);
              }
              return false;
#ifdef EC_DEBUG
            case id_ec_tools_debug_send_config:
              if (via_read_toggle_value(command)) {
                ec_config_debug_send_config(3000);
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
              ec_config_set_actuation_mode(preset_index, via_read_dropdown_value(command));
              return false;
            case id_ec_preset_actuation_threshold:
              ec_config_set_actuation_threshold(preset_index, VIA_READ_EC_RANGE_VALUE(command));
              return false;
            case id_ec_preset_actuation_travel:
              ec_config_set_actuation_travel(preset_index, VIA_READ_EC_HALF_RANGE_VALUE(command));
              return false;
            case id_ec_preset_release_mode:
              ec_config_set_release_mode(preset_index, via_read_dropdown_value(command));
              return false;
            case id_ec_preset_release_threshold:
              ec_config_set_release_threshold(preset_index, VIA_READ_EC_RANGE_VALUE(command));
              return false;
            case id_ec_preset_release_travel:
              ec_config_set_release_travel(preset_index, VIA_READ_EC_HALF_RANGE_VALUE(command));
              return false;
            case id_ec_preset_deadzone:
              ec_config_set_deadzone(preset_index, VIA_READ_EC_HALF_RANGE_VALUE(command));
              return false;
            case id_ec_preset_sub_action_enable:
              ec_config_set_sub_action_enable(preset_index, via_read_toggle_value(command));
              return false;
            case id_ec_preset_sub_action_keycode:
              ec_config_set_sub_action_keycode(preset_index, via_read_keycode_value(command));
              return false;
            case id_ec_preset_sub_action_actuation_threshold:
              ec_config_set_sub_action_actuation_threshold(preset_index, VIA_READ_EC_RANGE_VALUE(command));
              return false;
            case id_ec_preset_sub_action_release_threshold:
              ec_config_set_sub_action_release_threshold(preset_index, VIA_READ_EC_RANGE_VALUE(command));
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
              VIA_WRITE_EC_RANGE_VALUE(command, preset->actuation_travel);
              return false;
            case id_ec_preset_release_mode:
              via_write_dropdown_value(command, preset->release_mode);
              return false;
            case id_ec_preset_release_threshold:
              VIA_WRITE_EC_HALF_RANGE_VALUE(command, preset->release_threshold);
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
  return true;
}
