/*
 * Copyright (c) 2024 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <lvgl.h>
#include "misc/lv_area.h"

struct zmk_lv_layer_status_user_data {
  /* layer index label  */
  lv_obj_t *index_label;
  /* layer index label  */
  lv_obj_t *name_label;
};

lv_obj_t *zmk_lv_layer_status_create(lv_obj_t *parent, lv_obj_t *(*container_default)(lv_obj_t *),
                                     lv_align_t align, lv_coord_t width);
