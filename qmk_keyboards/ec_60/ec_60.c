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

#define EC_PRESET_CHANNEL_ID_START EC_VIA_CUSTOM_CHANNEL_ID_START
#define EC_PRESET_CHANNEL_ID_END (EC_VIA_CUSTOM_CHANNEL_ID_START + EC_NUM_PRESETS - 1)
#define EC_CALIBRATION_CHANNEL_ID (EC_VIA_CUSTOM_CHANNEL_ID_START + EC_NUM_PRESETS)

// Declaring enums for VIA config menu
enum via_ec_preset_value_id {
  // clang-format off
  id_ec_preset_actuation_mode = 1,
  id_ec_preset_actuation_threshold,
  id_ec_preset_actuation_travel,
  id_ec_preset_release_mode,
  id_ec_preset_release_threshold,
  id_ec_preset_release_travel,
  id_ec_preset_deadzone
  // clang-format on
};

enum via_ec_calibration_value_id {
  id_ec_bottoming_calibration = 1,
  id_ec_show_calibration_data,
  id_ec_debug_send_config,
  id_ec_bootloader_jump
};

static inline void writeUInt16BE(uint8_t *dst, uint16_t value) {
  dst[0] = value >> 8;
  dst[1] = value & 0xff;
}

static inline uint16_t readUInt16BE(uint8_t *src) { return ((uint16_t)src[0] << 8) + src[1]; }

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
    case EC_PRESET_START ... EC_PRESET_END:
      // not keycode
      return false;
    case EC_PRESET_MAP_START ... EC_PRESET_MAP_END:
      if (record->event.pressed) {
        uint8_t preset_map_index = keycode - EC_PRESET_MAP_START;
        ec_config_set_preset_map(preset_map_index);
      }
      return false;
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
    case EC_PMAP:
      if (!record->event.pressed) {
        // 3 secods after release
        ec_config_send_preset_map(3000);
      }
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
  uint8_t command_id = data[0];
  uint8_t *command_data = &(data[1]);
  switch (command_id) {
    case id_dynamic_keymap_set_keycode:
      if ((command_data[0] - EC_PRESET_MAP_START) == ec_eeprom_config.preset_map) {
        ec_config_update_key(command_data[1], command_data[2]);
      }
      break;
    case id_dynamic_keymap_reset: {
      ec_config_init();
      break;
    }
  }
}

bool via_custom_value_command_user(uint8_t *data, uint8_t length) {
  uint8_t command_id = data[0];
  uint8_t channel_id = data[1];
  uint8_t value_id = data[2];
  uint8_t value_byte = data[3];
  uint8_t *value_ptr = &data[3];
  // Big Endian
  uint16_t value_word = readUInt16BE(&data[3]);
  switch (channel_id) {
      // EC Presets
    case EC_PRESET_CHANNEL_ID_START ... EC_PRESET_CHANNEL_ID_END: {
      uint8_t preset_index = data[1] - EC_PRESET_CHANNEL_ID_START;
      switch (command_id) {
          // EC Presets set value
        case id_custom_set_value:
          switch (value_id) {
            case id_ec_preset_actuation_mode:
              ec_config_set_actuation_mode(preset_index, value_byte);
              return false;
            case id_ec_preset_actuation_threshold:
              ec_config_set_actuation_threshold(preset_index, value_word);
              return false;
            case id_ec_preset_actuation_travel:
              ec_config_set_actuation_travel(preset_index, value_word);
              return false;
            case id_ec_preset_release_mode:
              ec_config_set_release_mode(preset_index, value_byte);
              return false;
            case id_ec_preset_release_threshold:
              ec_config_set_release_threshold(preset_index, value_word);
              return false;
            case id_ec_preset_release_travel:
              ec_config_set_release_travel(preset_index, value_word);
              return false;
            case id_ec_preset_deadzone:
              ec_config_set_deadzone(preset_index, value_word);
              return false;
          }
          break;

          // EC Presets get value
        case id_custom_get_value:
          switch (value_id) {
            case id_ec_preset_actuation_mode:
              *value_ptr = ec_eeprom_config.presets[preset_index].actuation_mode;
              return false;
            case id_ec_preset_actuation_threshold:
              writeUInt16BE(value_ptr, ec_eeprom_config.presets[preset_index].actuation_threshold);
              return false;
            case id_ec_preset_actuation_travel:
              writeUInt16BE(value_ptr, ec_eeprom_config.presets[preset_index].actuation_travel);
              return false;
            case id_ec_preset_release_mode:
              *value_ptr = ec_eeprom_config.presets[preset_index].release_mode;
              return false;
            case id_ec_preset_release_threshold:
              writeUInt16BE(value_ptr, ec_eeprom_config.presets[preset_index].release_threshold);
              return false;
            case id_ec_preset_release_travel:
              writeUInt16BE(value_ptr, ec_eeprom_config.presets[preset_index].release_travel);
              return false;
            case id_ec_preset_deadzone:
              writeUInt16BE(value_ptr, ec_eeprom_config.presets[preset_index].deadzone);
              return false;
          }
          break;

        case id_custom_save:
          // nothing to do
          return false;
      }
      break;
    }
      // EC Calibration
    case EC_CALIBRATION_CHANNEL_ID:
      switch (command_id) {
        // EC Calibration set value
        case id_custom_set_value:
          switch (value_id) {
            case id_ec_bottoming_calibration:
              if (value_byte) {
                ec_config_start_calibration();
              } else {
                ec_config_end_calibration();
              }
              return false;
            case id_ec_show_calibration_data:
              if (value_byte) {
                ec_config_send_calibration_data(3000);
              }
              return false;
#ifdef EC_DEBUG
            case id_ec_debug_send_config:
              if (value_byte) {
                ec_config_debug_send_config(3000);
              }
              return false;
            case id_ec_bootloader_jump:
              if (value_byte) {
                bootloader_jump();
              }
              return false;
#endif
          }
          break;

          // EC Calibration get value
        case id_custom_get_value:
          *value_ptr = 0;
          return false;
        case id_custom_save:
          // nothing to do
          return false;
      }
  }
  return true;
}
