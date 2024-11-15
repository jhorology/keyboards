/*
 * Copyright (c) 2024 The ZMK Contributors
 * SPDX-License-Identifier: MIT
 */
#include <zmk/display/status_screen.h>

#include <zephyr/logging/log.h>
#include "misc/lv_area.h"
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/display/widgets/usb_status_16x12.h>
#include <zmk/display/widgets/ble_status_16x12.h>
#include <zmk/display/widgets/peripheral_status_36x12.h>
#include <zmk/display/widgets/battery_status_32x12.h>
#include <zmk/display/widgets/layer_status_h15.h>

#define WIDTH 80
#define HEIGHT 128
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

#if !IS_ENABLED(CONFIG_ZMK_SPLIT) || IS_ENABLED(CONFIG_ZMK_SPLIT_ROLE_CENTRAL)

LV_IMG_DECLARE(zmk_logo_40x40);
LV_FONT_DECLARE(teko_bold_15);
LV_FONT_DECLARE(cozetta_icons_13);

static inline lv_obj_t *split_left_status_bar(lv_obj_t *parent) {
  lv_obj_t *container = container_default(parent);
  lv_obj_set_flex_flow(container, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
  lv_obj_set_style_pad_top(container, 1, LV_PART_MAIN);

#  if IS_ENABLED(CONFIG_ZMK_WIDGET_BLE_STATUS_16X12)
  zmk_lv_ble_status_create(container, container_default, LV_ALIGN_TOP_MID);
#  endif
#  if IS_ENABLED(CONFIG_ZMK_WIDGET_BLE_STATUS_16X12) && \
    IS_ENABLED(CONFIG_ZMK_WIDGET_USB_STATUS_16X12)
  // separator
  lv_obj_t *separator = lv_label_create(container);
  lv_obj_set_style_text_font(separator, &cozetta_icons_13, LV_PART_MAIN);
  lv_label_set_text_static(separator, "|");
  lv_obj_set_style_pad_right(separator, 1, LV_PART_MAIN);
#  endif

#  if IS_ENABLED(CONFIG_ZMK_WIDGET_USB_STATUS_16X12)
  zmk_lv_usb_status_create(container, container_default, LV_ALIGN_TOP_MID);
#  endif

  // just a placeholder
  lv_obj_t *placeholder = container_default(container);
  lv_obj_set_flex_grow(placeholder, 1);

#  if IS_ENABLED(CONFIG_ZMK_WIDGET_BATTERY_STATUS_32X12)
  zmk_lv_battery_status_create(container, container_default, LV_ALIGN_OUT_RIGHT_TOP);
#  endif

  return container;
}

static inline lv_obj_t *split_left_content(lv_obj_t *parent) {
  lv_obj_t *container = container_default(parent);
  lv_obj_set_style_border_width(container, 1, LV_PART_MAIN);

  lv_obj_t *label = lv_label_create(container);
  lv_obj_set_style_text_font(label, &pixel_mplus_bold_10, LV_PART_MAIN);
  lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
  lv_obj_set_align(label, LV_ALIGN_CENTER);
  lv_label_set_text(label, "UNDER\nCONSTRUCTION");

  return container;
}

static inline lv_obj_t *split_left_body(lv_obj_t *parent) {
  lv_obj_t *container = container_default(parent);
  lv_obj_set_size(container, WIDTH, HEIGHT);
  lv_obj_set_flex_flow(container, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_flex_align(container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  lv_obj_set_style_pad_gap(container, 1, LV_PART_MAIN);

  lv_obj_t *status_bar = split_left_status_bar(container);
  lv_obj_set_size(status_bar, CONTENT_WIDTH, STATUS_BAR_HEIGHT);

#  if IS_ENABLED(CONFIG_ZMK_WIDGET_LAYER_STATUS_H15)
  zmk_lv_layer_status_create(container, container_default, LV_ALIGN_TOP_LEFT, CONTENT_WIDTH);
#  endif

  lv_obj_t *content = split_left_content(container);
  lv_obj_set_width(content, CONTENT_WIDTH);
  lv_obj_set_flex_grow(content, 1);

  // product logo

  lv_obj_t *zmk_desc = lv_label_create(container);
  lv_obj_set_style_text_font(zmk_desc, &teko_bold_15, 0);
  lv_label_set_text_static(zmk_desc, "Powered By");

  lv_obj_t *zmk_logo = lv_img_create(container);
  lv_img_set_src(zmk_logo, &zmk_logo_40x40);
  lv_obj_set_style_pad_bottom(zmk_logo, MARGIN, 0);

  return container;
}
#endif

#if IS_ENABLED(CONFIG_ZMK_SPLIT) && !IS_ENABLED(CONFIG_ZMK_SPLIT_ROLE_CENTRAL)

#  if IS_ENABLED(CONFIG_ZMK_CUSTOM_KEYNOARD_LOGO)
LV_IMG_DECLARE(keyboard_logo_80x80);
#  else
LV_IMG_DECLARE(starman_80x80);
#  endif

static inline lv_obj_t *split_right_status_bar(lv_obj_t *parent) {
  lv_obj_t *container = container_default(parent);
  lv_obj_set_flex_flow(container, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
  lv_obj_set_style_pad_top(container, 1, LV_PART_MAIN);

#  if IS_ENABLED(CONFIG_ZMK_WIDGET_PERIPHERAL_STATUS_36X12)
  zmk_lv_peripheral_status_create(container, container_default, LV_ALIGN_TOP_LEFT);
#  endif

  // just a placeholder
  lv_obj_t *placeholder = container_default(container);
  lv_obj_set_flex_grow(placeholder, 1);

#  if IS_ENABLED(CONFIG_ZMK_WIDGET_BATTERY_STATUS_32X12)
  zmk_lv_battery_status_create(container, container_default, LV_ALIGN_TOP_RIGHT);
#  endif

  return container;
}

static inline lv_obj_t *split_right_content(lv_obj_t *parent) {
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

static inline lv_obj_t *split_right_body(lv_obj_t *parent) {
  lv_obj_t *container = container_default(parent);
  lv_obj_set_size(container, WIDTH, HEIGHT);
  lv_obj_set_flex_flow(container, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_flex_align(container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  lv_obj_set_style_pad_gap(container, 1, LV_PART_MAIN);

  lv_obj_t *status_bar = split_right_status_bar(container);
  lv_obj_set_size(status_bar, CONTENT_WIDTH, STATUS_BAR_HEIGHT);

  lv_obj_t *content = split_right_content(container);
  lv_obj_set_width(content, CONTENT_WIDTH);
  lv_obj_set_flex_grow(content, 1);

  lv_obj_t *kbd_logo = lv_img_create(container);
  lv_obj_set_style_pad_top(kbd_logo, MARGIN, LV_PART_MAIN);
#  if IS_ENABLED(CONFIG_ZMK_CUSTOM_KEYNOARD_LOGO)
  lv_img_set_src(kbd_logo, &keyboard_logo_80x80);
#  else
  lv_img_set_src(kbd_logo, &starman_80x80);
#  endif
  return container;
}
#endif

lv_obj_t *zmk_display_status_screen() {
  lv_obj_t *screen;
#if !IS_ENABLED(CONFIG_ZMK_SPLIT) || IS_ENABLED(CONFIG_ZMK_SPLIT_ROLE_CENTRAL)
  screen = split_left_body(NULL);
#elif IS_ENABLED(CONFIG_ZMK_SPLIT) && !IS_ENABLED(CONFIG_ZMK_SPLIT_ROLE_CENTRAL)
  screen = split_right_body(NULL);
#endif
  return screen;
}
