/* SPDX-License-Identifier: GPL-2.0-or-later */

#pragma once

#define VIAL_KEYBOARD_UID {0x8B, 0xE3, 0x89, 0x56, 0xA7, 0xC8, 0x61, 0x72}
#define VIAL_UNLOCK_COMBO_ROWS { 0, 2 }
#define VIAL_UNLOCK_COMBO_COLS { 0, 13 }

// polling rate
// see https://github.com/qmk/qmk_firmware/blob/master/docs/config_options.md

#define USB_POLLING_INTERVAL_MS 1
#define QMK_KEYS_PER_SCAN 6
