/*
 * Copyright (c) 2024 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <lvgl.h>

struct zmk_lv_peripheral_status_user_data {
  /* layer index label  */
  lv_obj_t *icon_label;
  /* reserved for future use  */
  lv_obj_t *desc_label;
};

lv_obj_t *zmk_lv_peripheral_status_create(lv_obj_t *parent,
                                          lv_obj_t *(*container_default)(lv_obj_t *),
                                          lv_align_t align);
