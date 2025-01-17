/*
 * Copyright (c) 2024 The ZMK Contributors
 * SPDX-License-Identifier: MIT
 */
#include <zmk/display/status_screen.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/display/widgets/peripheral_status_36x12.h>
#include <zmk/display/widgets/battery_status_32x12.h>

#define WIDTH 68
#define HEIGHT 160
#define MARGIN 2
#define CONTENT_WIDTH (WIDTH - MARGIN * 2)
#define STATUS_BAR_HEIGHT 14

static lv_obj_t *container_default(lv_obj_t *parent) {
  lv_obj_t *container = lv_obj_create(parent);
  lv_obj_set_scrollbar_mode(container, LV_SCROLLBAR_MODE_OFF);
  lv_obj_set_style_pad_all(container, 0, LV_PART_MAIN);
  lv_obj_set_style_pad_column(container, 0, LV_PART_MAIN);
  lv_obj_set_style_pad_row(container, 0, LV_PART_MAIN);
  lv_obj_set_style_pad_gap(container, 0, LV_PART_MAIN);
  lv_obj_set_style_border_width(container, 0, LV_PART_MAIN);
  return container;
}

LV_FONT_DECLARE(pixel_mplus_bold_10);

#if IS_ENABLED(CONFIG_ZMK_CUSTOM_KEYBOARD_LOGO)
LV_IMG_DECLARE(keyboard_logo_68x80);
#else
LV_IMG_DECLARE(starman_68x80);
#endif

static inline lv_obj_t *content_create(lv_obj_t *parent) {
  lv_obj_t *container = container_default(parent);
  lv_obj_set_style_border_width(container, 1, LV_PART_MAIN);
  lv_obj_set_style_pad_bottom(container, MARGIN, LV_PART_MAIN);

  lv_obj_t *label = lv_label_create(container);
  lv_obj_set_style_text_font(label, &pixel_mplus_bold_10, LV_PART_MAIN);
  lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
  lv_obj_set_align(label, LV_ALIGN_CENTER);
  lv_label_set_text(label, "UNDER\nCONSTRUCTION");

  return container;
}

lv_obj_t *zmk_display_status_screen() {
  lv_obj_t *container = container_default(NULL);
  lv_obj_set_size(container, WIDTH, HEIGHT);
  lv_obj_set_flex_flow(container, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_flex_align(container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  lv_obj_set_style_pad_gap(container, 1, LV_PART_MAIN);

#if IS_ENABLED(CONFIG_ZMK_WIDGET_BATTERY_STATUS_32X12)
  zmk_lv_battery_status_create(container, container_default, LV_ALIGN_CENTER);
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_PERIPHERAL_STATUS_36X12)
  zmk_lv_peripheral_status_create(container, container_default, LV_ALIGN_CENTER);
#endif

  lv_obj_t *content = content_create(container);
  lv_obj_set_width(content, CONTENT_WIDTH);
  lv_obj_set_flex_grow(content, 1);

  lv_obj_t *kbd_logo = lv_img_create(container);
  lv_obj_set_style_pad_top(kbd_logo, MARGIN, LV_PART_MAIN);
#if IS_ENABLED(CONFIG_ZMK_CUSTOM_KEYBOARD_LOGO)
  lv_img_set_src(kbd_logo, &keyboard_logo_68x80);
#else
  lv_img_set_src(kbd_logo, &starman_68x80);
#endif
  return container;
}
