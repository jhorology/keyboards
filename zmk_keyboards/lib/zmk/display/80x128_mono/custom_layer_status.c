/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/display.h>
#include <zmk/display/widgets/layer_status.h>
#include <zmk/events/layer_state_changed.h>
#include <zmk/event_manager.h>
#include <zmk/endpoints.h>
#include <zmk/keymap.h>

LV_FONT_DECLARE(pixel_mplus_bold_ascii_10);
LV_FONT_DECLARE(pixel_mplus_bold_ascii_12);

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

struct layer_status_state {
  zmk_keymap_layer_index_t index;
  const char *label;
};

static void set_layer_symbol(lv_obj_t *spangroup, struct layer_status_state state) {
  char span_0_text[3] = {};
  char span_1_text[CONFIG_ZMK_KEYMAP_LAYER_NAME_MAX_LEN + 1] = {};

  snprintf(span_0_text, sizeof(span_0_text), "%i ", state.index);
  snprintf(span_1_text, sizeof(span_1_text), "%s", state.label);

  lv_span_t *span = lv_spangroup_get_child(spangroup, 0);
  lv_span_set_text(span, span_0_text);

  span = lv_spangroup_get_child(spangroup, 1);
  lv_span_set_text(span, span_1_text);
}

static void layer_status_update_cb(struct layer_status_state state) {
  struct zmk_widget_layer_status *widget;
  SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) { set_layer_symbol(widget->obj, state); }
}

static struct layer_status_state layer_status_get_state(const zmk_event_t *eh) {
  zmk_keymap_layer_index_t index = zmk_keymap_highest_layer_active();
  return (struct layer_status_state){
    .index = index, .label = zmk_keymap_layer_name(zmk_keymap_layer_index_to_id(index))};
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_layer_status, struct layer_status_state, layer_status_update_cb,
                            layer_status_get_state)

ZMK_SUBSCRIPTION(widget_layer_status, zmk_layer_state_changed);

int zmk_widget_layer_status_init(struct zmk_widget_layer_status *widget, lv_obj_t *parent) {
  widget->obj = lv_spangroup_create(parent);

  // span_0_text
  lv_span_t *span = lv_spangroup_new_span(widget->obj);
  lv_style_set_text_font(&span->style, &pixel_mplus_bold_ascii_10);

  // span_1_text
  span = lv_spangroup_new_span(widget->obj);
  lv_style_set_text_font(&span->style, &pixel_mplus_bold_ascii_12);

  sys_slist_append(&widgets, &widget->node);

  widget_layer_status_init();
  return 0;
}

lv_obj_t *zmk_widget_layer_status_obj(struct zmk_widget_layer_status *widget) {
  return widget->obj;
}
