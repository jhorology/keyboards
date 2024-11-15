/*
 * Copyright (c) 2024 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include "core/lv_obj.h"
#include "core/lv_obj_pos.h"
#include "core/lv_obj_style.h"
#include "misc/lv_area.h"
#include "misc/lv_color.h"
#include "misc/lv_txt.h"
#include "widgets/lv_label.h"
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/display.h>
#include <zmk/events/layer_state_changed.h>
#include <zmk/event_manager.h>
#include <zmk/endpoints.h>
#include <zmk/keymap.h>

#include <zmk/display/util_macros.h>
#include <zmk/display/widgets/layer_status_h15.h>

#define HEIGHT 15

LV_FONT_DECLARE(teko_bold_15);

struct layer_status_state {
  zmk_keymap_layer_index_t index;
  const char *label;
};

static struct zmk_lv_layer_status_user_data user_data;

static void layer_status_update_cb(struct layer_status_state state) {
  lv_label_set_text_fmt(user_data.index_label, "%c", (char)('1' + state.index));
  lv_label_set_text_fmt(user_data.name_label, "%s", state.label);
}

static struct layer_status_state layer_status_get_state(const zmk_event_t *eh) {
  zmk_keymap_layer_index_t index = zmk_keymap_highest_layer_active();
  return (struct layer_status_state){
    .index = index, .label = zmk_keymap_layer_name(zmk_keymap_layer_index_to_id(index))};
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_layer_status, struct layer_status_state, layer_status_update_cb,
                            layer_status_get_state)

ZMK_SUBSCRIPTION(widget_layer_status, zmk_layer_state_changed);

lv_obj_t *zmk_lv_layer_status_create(lv_obj_t *parent, lv_obj_t *(*container_default)(lv_obj_t *),
                                     lv_align_t align, lv_coord_t width) {
  lv_obj_t *container =
    container_default != NULL ? container_default(parent) : lv_obj_create(parent);
  /* WIDTH is not defined in herer  */
  lv_obj_set_size(container, width, HEIGHT);
  lv_obj_set_flex_flow(container, LV_FLEX_FLOW_ROW);
  lv_obj_set_style_pad_gap(container, 1, LV_PART_MAIN);
  ALIGN_FLEX_FLOW_ROW_COMPOSITE_WIDGET(container, align, LV_FLEX_ALIGN_END);

  lv_obj_set_style_pad_gap(container, 2, LV_PART_MAIN);

  lv_obj_t *index_label = lv_label_create(container);
  lv_obj_set_width(index_label, 13);

  lv_obj_set_style_bg_opa(index_label, LV_OPA_COVER, LV_PART_MAIN);
  lv_obj_set_style_bg_color(index_label, lv_color_black(), LV_PART_MAIN);
  lv_obj_set_style_text_font(index_label, &teko_bold_15, LV_PART_MAIN);
  lv_obj_set_style_text_color(index_label, lv_color_white(), LV_PART_MAIN);
  lv_obj_set_style_text_align(index_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
  lv_obj_set_style_radius(index_label, 2, LV_PART_MAIN);

  lv_obj_t *name_label = lv_label_create(container);
  lv_obj_set_style_text_font(name_label, &teko_bold_15, LV_PART_MAIN);
  lv_label_set_long_mode(name_label, LV_LABEL_LONG_DOT);
  lv_obj_set_flex_grow(name_label, 1);
  lv_label_set_long_mode(name_label, LV_LABEL_LONG_CLIP);

  user_data.index_label = index_label;
  user_data.name_label = name_label;
  lv_obj_set_user_data(container, &user_data);

  widget_layer_status_init();
  return container;
}
