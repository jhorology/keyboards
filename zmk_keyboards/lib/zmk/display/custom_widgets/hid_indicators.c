/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <lvgl.h>
#include <zmk/display/lv_zmk_event.h>
#include <zmk/display/lv_zmk_status.h>
#include <zmk/display/custom_widgets/hid_indicators.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#define INDICATOR_WIDTH 11
#define INDICATOR_HEIGHT 11
#define NUM_INDICATORS 5

const char indicator_letters[] = {
  /* NUM Lock */
  '9',
  /* Caps Lock */
  'A',
  /* Scroll Lock */
  'S',
  /* Compose  */
  'C',
  /* Kana Lock  */
  'K',
};

static const lv_zmk_event_interests zmk_event_interests = LV_ZMK_EVENT_INTERESTS(hid_indicators);
LV_FONT_DECLARE(pixel_mplus_bold_10);

static void hid_indicators_cb(lv_event_t *event) {
  lv_obj_t *container = lv_event_get_current_target(event);
  struct lv_zmk_status *state = lv_event_get_param(event);
  if (state->indicators_state) {
    for (uint8_t i = 0; i < NUM_INDICATORS; i++) {
      lv_obj_t *indicator = lv_obj_get_child(container, i);
      if (state->indicators_state & BIT(i)) {
        lv_obj_clear_flag(indicator, LV_OBJ_FLAG_HIDDEN);
      } else {
        lv_obj_add_flag(indicator, LV_OBJ_FLAG_HIDDEN);
      }
    }
    lv_obj_clear_flag(container, LV_OBJ_FLAG_HIDDEN);
  } else {
    lv_obj_add_flag(container, LV_OBJ_FLAG_HIDDEN);
  }
}

lv_obj_t *lv_hid_indicators_create(lv_obj_t *parent, lv_obj_t *(*container_default)(lv_obj_t *)) {
  lv_obj_t *container =
    container_default != NULL ? container_default(parent) : lv_obj_create(parent);
  lv_obj_set_size(container, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
  lv_obj_set_flex_flow(container, LV_FLEX_FLOW_ROW);
  lv_obj_set_style_pad_column(container, 1, LV_PART_MAIN);
  lv_obj_set_flex_align(container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_START);

  lv_color_t border_color = lv_obj_get_style_text_color(container, LV_PART_MAIN);

  for (uint8_t i = 0; i < NUM_INDICATORS; i++) {
    lv_obj_t *indicator = lv_label_create(container);
    lv_obj_set_size(indicator, INDICATOR_WIDTH, INDICATOR_HEIGHT);
    lv_obj_set_style_border_width(indicator, 1, LV_PART_MAIN);
    lv_obj_set_style_border_color(indicator, border_color, LV_PART_MAIN);

    lv_obj_set_style_text_font(indicator, &pixel_mplus_bold_10, LV_PART_MAIN);

    lv_obj_set_style_pad_left(indicator, 2, LV_PART_MAIN);
    lv_label_set_text_fmt(indicator, "%c", indicator_letters[i]);
  }

  lv_obj_add_event_cb(container, hid_indicators_cb, LV_ZMK_EVENT_CODE(hid_indicators), NULL);
  lv_obj_set_user_data(container, (void *)&zmk_event_interests);

  return container;
}
