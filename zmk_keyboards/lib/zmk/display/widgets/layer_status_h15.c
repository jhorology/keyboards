/*
 * Copyright (c) 2024 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <zmk/display/lv_zmk_event.h>
#include <zmk/display/lv_zmk_status.h>
#include <zmk/display/status_presenter.h>
#include <zmk/display/util_macros.h>
#include <zmk/display/widgets/layer_status_h15.h>
#include <zephyr/logging/log.h>
#include "core/lv_event.h"
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#define HEIGHT 15

LV_FONT_DECLARE(teko_bold_15);
LV_FONT_DECLARE(pixel_mplus_bold_10);

static void index_cb(lv_event_t *event) {
  lv_obj_t *index_label = lv_event_get_current_target(event);
  struct lv_zmk_status *state = lv_event_get_param(event);

  lv_label_set_text_fmt(index_label, "%d", state->layer_index);
}

static void name_cb(lv_event_t *event) {
  lv_obj_t *name_label = lv_event_get_current_target(event);
  struct lv_zmk_status *state = lv_event_get_param(event);

  lv_label_set_text_fmt(name_label, "%s", state->layer_name);
}

lv_obj_t *lv_layer_status_create(lv_obj_t *parent, lv_obj_t *(*container_default)(lv_obj_t *),
                                 lv_align_t align, lv_coord_t width) {
  lv_obj_t *container =
    container_default != NULL ? container_default(parent) : lv_obj_create(parent);
  /* WIDTH is not defined in herer  */
  lv_obj_set_size(container, width, HEIGHT);
  lv_obj_set_flex_flow(container, LV_FLEX_FLOW_ROW);
  lv_obj_set_style_pad_gap(container, 1, LV_PART_MAIN);
  ALIGN_FLEX_FLOW_ROW_COMPOSITE_WIDGET(container, align, LV_FLEX_ALIGN_CENTER);

  lv_obj_set_style_pad_gap(container, 2, LV_PART_MAIN);

  lv_obj_t *index_label = lv_label_create(container);
  lv_obj_set_size(index_label, 11, 13);

  lv_obj_set_style_bg_opa(index_label, LV_OPA_COVER, LV_PART_MAIN);
  lv_obj_set_style_bg_color(index_label, lv_color_black(), LV_PART_MAIN);
  lv_obj_set_style_text_font(index_label, &pixel_mplus_bold_10, LV_PART_MAIN);
  lv_obj_set_style_text_color(index_label, lv_color_white(), LV_PART_MAIN);
  lv_obj_set_style_text_align(index_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
  lv_obj_set_style_pad_top(index_label, 2, LV_PART_MAIN);
  lv_obj_set_style_radius(index_label, 2, LV_PART_MAIN);
  lv_obj_add_event_cb(index_label, index_cb, LV_ZMK_EVENT_CODE(layer_state), NULL);
  /* TODO auto register */
  zmk_status_presenter_register(index_label, LV_ZMK_EVENT_CODE(layer_state));

  lv_obj_t *name_label = lv_label_create(container);
  lv_obj_set_style_text_font(name_label, &teko_bold_15, LV_PART_MAIN);
  lv_label_set_long_mode(name_label, LV_LABEL_LONG_DOT);
  lv_obj_set_flex_grow(name_label, 1);
  lv_label_set_long_mode(name_label, LV_LABEL_LONG_CLIP);

  lv_obj_add_event_cb(name_label, name_cb, LV_ZMK_EVENT_CODE(layer_state), NULL);
  /* TODO auto register */
  zmk_status_presenter_register(name_label, LV_ZMK_EVENT_CODE(layer_state));

  return container;
}
