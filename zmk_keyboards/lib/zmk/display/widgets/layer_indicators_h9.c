/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <lvgl.h>
#include <zmk/display/lv_zmk_event.h>
#include <zmk/display/lv_zmk_status.h>
#include <zmk/display/util_macros.h>
#include <zmk/display/widgets/layer_indicators_h9.h>

#include <zmk/keymap.h>

#include <zephyr/logging/log.h>
#include "core/lv_obj_tree.h"
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#define INDICATOR_WIDTH 9
#define INDICATOR_HEIGHT 9
#define MARGIN 1
#define MAX_INDICATORS 16
#define NUM_LAYERS MIN(MAX_INDICATORS, ZMK_KEYMAP_LAYERS_LEN)

static const lv_zmk_event_interests zmk_event_interests = LV_ZMK_EVENT_INTERESTS(layer_state);
LV_FONT_DECLARE(micro5_10);

static void layer_state_cb(lv_event_t *event) {
  lv_obj_t *container = lv_event_get_current_target(event);
  struct lv_zmk_status *state = lv_event_get_param(event);

  for (uint8_t i = 0; i < NUM_LAYERS; i++) {
    lv_obj_t *indicator = lv_obj_get_child(container, i);
    bool active = (state->layers_state & BIT(i)) != 0;
    lv_obj_set_style_bg_color(indicator, active ? lv_color_black() : lv_color_white(),
                              LV_PART_MAIN);
    lv_obj_set_style_text_color(indicator, active ? lv_color_white() : lv_color_black(),
                                LV_PART_MAIN);
  }
}

lv_obj_t *lv_layer_indicators_create(lv_obj_t *parent, lv_obj_t *(*container_default)(lv_obj_t *),
                                     lv_align_t align, lv_coord_t width) {
  uint16_t cols = (width + MARGIN) / (INDICATOR_WIDTH + MARGIN);
  if (cols == 0) {
    cols = 1;
    width = INDICATOR_WIDTH;
  }
  uint16_t rows = DIV_ROUND_UP(NUM_LAYERS, cols);

  lv_obj_t *container =
    container_default != NULL ? container_default(parent) : lv_obj_create(parent);
  lv_obj_set_size(container, width, INDICATOR_HEIGHT * rows + MARGIN * (rows - 1));
  lv_obj_set_flex_flow(container, LV_FLEX_FLOW_ROW_WRAP);
  lv_obj_set_style_pad_column(container, MARGIN, LV_PART_MAIN);
  lv_obj_set_style_pad_row(container, MARGIN, LV_PART_MAIN);
  ALIGN_FLEX_FLOW_ROW_COMPOSITE_WIDGET(container, align, LV_FLEX_ALIGN_CENTER);

  for (uint8_t i = 0; i < NUM_LAYERS; i++) {
    lv_obj_t *indicator = lv_label_create(container);
    lv_obj_set_size(indicator, INDICATOR_WIDTH, INDICATOR_HEIGHT);
    lv_obj_set_style_border_width(indicator, 1, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(indicator, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_text_font(indicator, &micro5_10, LV_PART_MAIN);

    lv_obj_set_style_pad_top(indicator, -1, LV_PART_MAIN);
    lv_obj_set_style_pad_left(indicator, 2, LV_PART_MAIN);
    lv_label_set_text_fmt(indicator, "%c", i < 9 ? ('1' + i) : ('A' + (i - 9)));
  }

  lv_obj_add_event_cb(container, layer_state_cb, LV_ZMK_EVENT_CODE(layer_state), NULL);
  lv_obj_set_user_data(container, (void *)&zmk_event_interests);

  return container;
}
