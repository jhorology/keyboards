/* SPDX-License-Identifier: GPL-2.0-or-later */

#pragma once

#define VIAL_KEYBOARD_UID {0xC3, 0xB7, 0xF6, 0xE8, 0x92, 0xC7, 0x5F, 0x5F}
#define VIAL_UNLOCK_COMBO_ROWS { 0, 2 }
#define VIAL_UNLOCK_COMBO_COLS { 0, 13 }

// reducing  firmware size
// see https://get.vial.today/docs/firmware-size.html

// #define VIAL_TAP_DANCE_ENTRIES 4
// #define VIAL_COMBO_ENTRIES 4
// #define VIAL_KEY_OVERRIDE_ENTRIES 4

// polling rate
// see https://github.com/qmk/qmk_firmware/blob/master/docs/config_options.md

// #define USB_POLLING_INTERVAL_MS 4
// #define QMK_KEYS_PER_SCAN 4
