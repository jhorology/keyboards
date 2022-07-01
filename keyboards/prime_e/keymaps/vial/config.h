#pragma once

#define VIAL_KEYBOARD_UID {0xF3, 0xB8, 0xA3, 0x29, 0x37, 0x91, 0xFD, 0xA2}
#define VIAL_UNLOCK_COMBO_ROWS { 0, 1 }
#define VIAL_UNLOCK_COMBO_COLS { 0, 12 }

// reducing  firmware size
// see https://get.vial.today/docs/firmware-size.html

// #define VIAL_TAP_DANCE_ENTRIES 4
// #define VIAL_COMBO_ENTRIES 4
// #define VIAL_KEY_OVERRIDE_ENTRIES 4

// polling rate
// see https://github.com/qmk/qmk_firmware/blob/master/docs/config_options.md

#define USB_POLLING_INTERVAL_MS 1
#define QMK_KEYS_PER_SCAN 6
