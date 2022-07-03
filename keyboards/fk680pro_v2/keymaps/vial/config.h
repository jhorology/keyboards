#pragma once

#define VIAL_KEYBOARD_UID {0x76, 0xDA, 0x74, 0x74, 0x74, 0x9B, 0xDD, 0x0C}
#define VIAL_UNLOCK_COMBO_ROWS { 0, 2 }
#define VIAL_UNLOCK_COMBO_COLS { 0, 13 }

// polling rate
// see https://github.com/qmk/qmk_firmware/blob/master/docs/config_options.md

#define USB_POLLING_INTERVAL_MS 1
#define QMK_KEYS_PER_SCAN 6
