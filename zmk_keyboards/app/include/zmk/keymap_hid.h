/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <zephyr/usb/usb_device.h>
#include <zephyr/usb/class/usb_hid.h>

#include <zmk/keys.h>
#include <zmk/hid.h>
#include <dt-bindings/zmk/hid_usage.h>
#include <dt-bindings/zmk/hid_usage_pages.h>

#ifndef HID_USAGE_PAGE16
#define HID_USAGE_PAGE16(page, page2)                                                              \
    HID_ITEM(HID_ITEM_TAG_USAGE_PAGE, HID_ITEM_TYPE_GLOBAL, 2), page, page2
#endif

#ifndef HID_USAGE6
#define HID_USAGE16(page, page2) HID_ITEM(HID_ITEM_TAG_USAGE, HID_ITEM_TYPE_LOCAL, 2), page, page2
#endif

#define KEYMAP_HID_MAX_BYTES 1

#define KEYMAP_HID_REPORT_ID 0x42

static const uint8_t zmk_keymap_hid_report_desc[] = {
    HID_USAGE_PAGE16(0x0B, 0xFF),
    HID_USAGE(0x01),
    HID_COLLECTION(HID_COLLECTION_APPLICATION),
    HID_REPORT_ID(KEYMAP_HID_REPORT_ID),
    HID_USAGE_PAGE16(0x0B, 0xFF),
    HID_USAGE_MIN8(0x00),
    HID_USAGE_MAX16(0xFF, 0x00),
    HID_LOGICAL_MIN8(0x00),
    HID_LOGICAL_MAX16(0xFF, 0x00),

    HID_REPORT_SIZE(0x08),
    HID_REPORT_COUNT(KEYMAP_HID_MAX_BYTES),
    /* INPUT (Data,Var,Abs) */
    HID_INPUT(ZMK_HID_MAIN_VAL_DATA | ZMK_HID_MAIN_VAL_VAR | ZMK_HID_MAIN_VAL_ABS),

    HID_REPORT_ID(KEYMAP_HID_REPORT_ID),
    HID_USAGE_PAGE16(0x0B, 0xFF),
    HID_USAGE_MIN8(0x00),
    HID_USAGE_MAX16(0xFF, 0x00),
    HID_LOGICAL_MIN8(0x00),
    HID_LOGICAL_MAX16(0xFF, 0x00),

    HID_REPORT_SIZE(0x08),
    HID_REPORT_COUNT(KEYMAP_HID_MAX_BYTES),
    /* INPUT (Data,Var,Abs) */
    HID_OUTPUT(ZMK_HID_MAIN_VAL_DATA | ZMK_HID_MAIN_VAL_VAR | ZMK_HID_MAIN_VAL_ABS),
    HID_END_COLLECTION,
};

struct zmk_keymap_hid_sample_report_body {
    uint8_t data[KEYMAP_HID_MAX_BYTES];
} __packed;

struct zmk_keymap_hid_sample_report {
    uint8_t report_id;
    struct zmk_keymap_hid_sample_report_body body;
} __packed;
