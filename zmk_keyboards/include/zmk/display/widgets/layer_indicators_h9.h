/*
 * Copyright (c) 2024 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <lvgl.h>

lv_obj_t *lv_layer_indicators_create(lv_obj_t *parent, lv_obj_t *(*container_default)(lv_obj_t *),
                                     lv_align_t align, lv_coord_t width);
