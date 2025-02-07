/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <lvgl.h>
#include <zmk/display/lv_zmk_event.h>
#include <zmk/display/lv_zmk_status.h>
#include <zmk/display/custom_widgets/layer_indicators.h>

#include <zmk/keymap.h>

#include <zephyr/logging/log.h>
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

  // normal color
  lv_color_t bg_color = lv_obj_get_style_bg_color(container, LV_PART_MAIN);
  lv_color_t text_color = lv_obj_get_style_text_color(container, LV_PART_MAIN);

  for (uint8_t i = 0; i < NUM_LAYERS; i++) {
    lv_obj_t *indicator = lv_obj_get_child(container, i);
    bool active = (state->layers_state & BIT(i)) != 0;
    lv_obj_set_style_bg_color(indicator, active ? text_color : bg_color, LV_PART_MAIN);
    lv_obj_set_style_text_color(indicator, active ? bg_color : text_color, LV_PART_MAIN);
  }
}

lv_obj_t *lv_layer_indicators_create(lv_obj_t *parent, lv_style_t *container_style,
                                     lv_coord_t width) {
  lv_obj_t *container = lv_obj_create(parent);
  if (container_style != NULL) {
    lv_obj_add_style(container, container_style, LV_PART_MAIN);
  }
  lv_obj_set_width(container, width);
  lv_obj_set_flex_flow(container, LV_FLEX_FLOW_ROW_WRAP);
  lv_obj_set_style_pad_column(container, MARGIN, LV_PART_MAIN);
  lv_obj_set_style_pad_row(container, MARGIN, LV_PART_MAIN);
  lv_obj_set_flex_align(container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_START);

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
