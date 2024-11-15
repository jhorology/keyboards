/*
 * Copyright (c) 2024 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <lvgl.h>

#define IS_TOP_ALIGN(align) \
  (align == LV_ALIGN_TOP_LEFT || align == LV_ALIGN_TOP_MID || align == LV_ALIGN_TOP_MID)
#define IS_V_MID_ALIGN(align) \
  (align == LV_ALIGN_LEFT_MID || align == LV_ALIGN_CENTER || align == LV_ALIGN_RIGHT_MID)
#define IS_BOTTOM_ALIGN(align) \
  (align == LV_ALIGN_BOTTOM_LEFT || align == LV_ALIGN_BOTTOM_MID || align == LV_ALIGN_BOTTOM_RIGHT)
#define IS_LEFT_ALIGN(align) \
  (align == LV_ALIGN_TOP_LEFT || align == LV_ALIGN_LEFT_MID || align == LV_ALIGN_BOTTOM_LEFT)
#define IS_H_MID_ALIGN(align) \
  (align == LV_ALIGN_TOP_MID || align == LV_ALIGN_CENTER || align == LV_ALIGN_BOTTOM_MID)
#define IS_RIGHT_ALIGN(align) \
  (align == LV_ALIGN_TOP_RIGHT || align == LV_ALIGN_RIGHT_MID || align == LV_ALIGN_BOTTOM_RIGHT)

#define FLEX_FLOW_ROW_MAIN_PLACE(align)           \
  (IS_H_MID_ALIGN(align)   ? LV_FLEX_ALIGN_CENTER \
   : IS_RIGHT_ALIGN(align) ? LV_FLEX_ALIGN_END    \
                           : LV_FLEX_ALIGN_START)

#define FLEX_FLOW_ROW_CROSS_PLACE(align)           \
  (IS_V_MID_ALIGN(align)    ? LV_FLEX_ALIGN_CENTER \
   : IS_BOTTOM_ALIGN(align) ? LV_FLEX_ALIGN_END    \
                            : LV_FLEX_ALIGN_START)

#define FLEX_FLOW_ROW_TRACK_CROSS_PLACE(align) FLEX_FLOW_ROW_CROSS_PLACE(align)

#define ALIGN_FLEX_FLOW_ROW_COMPOSITE_WIDGET(container, align, cross_align)      \
  lv_obj_set_flex_align(container, FLEX_FLOW_ROW_MAIN_PLACE(align), cross_align, \
                        FLEX_FLOW_ROW_TRACK_CROSS_PLACE(align))
