/*
 * Copyright (c) 2024 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <lvgl.h>

struct zmk_lv_ble_status_user_data {
  lv_obj_t *icon_label;
  lv_obj_t *index_label;
};

lv_obj_t *zmk_lv_ble_status_create(lv_obj_t *parent, lv_obj_t *(*container_default)(lv_obj_t *),
                                   lv_align_t align);
