/*
 * Copyright (c) 2024 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <lvgl.h>
#include <zmk/display/lv_zmk_event.h>
#include <zmk/display/lv_zmk_status.h>
#include <zmk/display/custom_widgets/layer_status.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

static const lv_zmk_event_interests zmk_event_interests = LV_ZMK_EVENT_INTERESTS(layer_state);

LV_FONT_DECLARE(teko_bold_15);
LV_FONT_DECLARE(pixel_mplus_bold_10);

static void layer_state_cb(lv_event_t *event) {
  lv_obj_t *container = lv_event_get_current_target(event);
  struct lv_zmk_status *state = lv_event_get_param(event);
  lv_obj_t *index_label = lv_obj_get_child(container, 0);
  lv_obj_t *name_label = lv_obj_get_child(container, 1);

  lv_label_set_text_fmt(index_label, "%d", state->layer_index);
  lv_label_set_text_fmt(name_label, "%s", state->layer_name);
}

lv_obj_t *lv_layer_status_create(lv_obj_t *parent, lv_style_t *container_style, lv_coord_t width) {
  lv_obj_t *container = lv_obj_create(parent);
  if (container_style != NULL) {
    lv_obj_add_style(container, container_style, LV_PART_MAIN);
  }
  lv_obj_set_width(container, width);
  lv_obj_set_flex_flow(container, LV_FLEX_FLOW_ROW);
  lv_obj_set_style_pad_gap(container, 1, LV_PART_MAIN);
  lv_obj_set_flex_align(container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);

  lv_obj_set_style_pad_gap(container, 2, LV_PART_MAIN);

  lv_obj_t *index_label = lv_label_create(container);

  lv_obj_set_style_bg_opa(index_label, LV_OPA_COVER, LV_PART_MAIN);
  lv_obj_set_style_bg_color(index_label, lv_obj_get_style_text_color(parent, LV_PART_MAIN),
                            LV_PART_MAIN);
  lv_obj_set_style_text_font(index_label, &pixel_mplus_bold_10, LV_PART_MAIN);
  lv_obj_set_style_text_color(index_label, lv_obj_get_style_bg_color(parent, LV_PART_MAIN),
                              LV_PART_MAIN);
  lv_obj_set_size(index_label, 11, 11);
  lv_obj_set_style_text_align(index_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
  lv_obj_set_style_radius(index_label, 2, LV_PART_MAIN);
  lv_obj_set_style_pad_top(index_label, 1, LV_PART_MAIN);

  lv_obj_t *name_label = lv_label_create(container);
  lv_obj_set_style_text_font(name_label, &teko_bold_15, LV_PART_MAIN);
  lv_label_set_long_mode(name_label, LV_LABEL_LONG_DOT);
  lv_obj_set_flex_grow(name_label, 1);
  lv_label_set_long_mode(name_label, LV_LABEL_LONG_CLIP);

  lv_obj_add_event_cb(container, layer_state_cb, LV_ZMK_EVENT_CODE(layer_state), NULL);

  lv_obj_set_user_data(container, (void *)&zmk_event_interests);

  return container;
}
