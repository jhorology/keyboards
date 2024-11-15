/*
 * Copyright (c) 2024 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <lvgl.h>

struct zmk_lv_battery_status_user_data {
  /* icon label  */
  lv_obj_t *icons_label;
  /* percentage label  */
  lv_obj_t *perc_label;
};

lv_obj_t *zmk_lv_battery_status_create(lv_obj_t *parent, lv_obj_t *(*container_default)(lv_obj_t *),
                                       lv_align_t align);
