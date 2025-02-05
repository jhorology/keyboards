/*
 * Copyright (c) 2024 The ZMK Contributors
 * SPDX-License-Identifier: MIT
 */
#include <zmk/display/status_screen.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/display/status_presenter.h>
#include <zmk/display/widgets/usb_status_16x12.h>
#include <zmk/display/widgets/ble_status_16x12.h>
#include <zmk/display/widgets/battery_status_23x12.h>
#include <zmk/display/widgets/layer_status_h15.h>
#include <zmk/display/widgets/layer_indicators_h9.h>
#include <zmk/display/widgets/hid_indicators_h11.h>

#define WIDTH 68
#define HEIGHT 160
#define MARGIN 2
#define CONTENT_WIDTH (WIDTH - MARGIN * 2)

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

// LV_FONT_DECLARE(pixel_mplus_10);
LV_FONT_DECLARE(micro5_10);
LV_IMG_DECLARE(zmk_logo_40x40);
#if IS_ENABLED(CONFIG_ZMK_WIDGET_BLE_STATUS_16X12) && IS_ENABLED(CONFIG_ZMK_WIDGET_USB_STATUS_16X12)
LV_FONT_DECLARE(cozetta_icons_13);
#endif

static inline lv_obj_t *top_bar_create(lv_obj_t *parent) {
  lv_obj_t *container = container_default(parent);
  lv_obj_set_size(container, WIDTH, 12);
  lv_obj_set_flex_flow(container, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(container, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER,
                        LV_FLEX_ALIGN_CENTER);
  lv_obj_set_height(container, 12);

#if IS_ENABLED(CONFIG_ZMK_WIDGET_HID_INDICATORS_H11)
  lv_obj_t *indicators = lv_hid_indicators_create(container, container_default, LV_ALIGN_LEFT_MID);
  lv_obj_set_flex_grow(indicators, 1);
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_BATTERY_STATUS_23X12)
  lv_battery_status_create(container, container_default, LV_ALIGN_RIGHT_MID);
#endif

  return container;
}

static inline lv_obj_t *endpoint_status_create(lv_obj_t *parent) {
  lv_obj_t *container = container_default(parent);
  lv_obj_set_flex_flow(container, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(container, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER,
                        LV_FLEX_ALIGN_CENTER);
  lv_obj_set_style_pad_top(container, 1, LV_PART_MAIN);
  lv_obj_set_style_pad_column(container, 4, LV_PART_MAIN);
  lv_obj_set_height(container, 12);

#if IS_ENABLED(CONFIG_ZMK_WIDGET_BLE_STATUS_16X12)
  lv_ble_status_create(container, container_default, LV_ALIGN_TOP_MID);
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_BLE_STATUS_16X12) && IS_ENABLED(CONFIG_ZMK_WIDGET_USB_STATUS_16X12)
  // separator
  lv_obj_t *separator = lv_obj_create(container);
  lv_obj_set_style_opa(separator, LV_OPA_COVER, LV_PART_MAIN);
  lv_obj_set_style_bg_color(separator, lv_color_black(), LV_PART_MAIN);
  lv_obj_set_height(separator, 12);
  lv_obj_set_width(separator, 1);
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_USB_STATUS_16X12)
  lv_usb_status_create(container, container_default, LV_ALIGN_TOP_MID);
#endif

  return container;
}

static inline lv_obj_t *content_create(lv_obj_t *parent) {
  lv_obj_t *container = container_default(parent);
  lv_obj_set_style_border_width(container, 1, LV_PART_MAIN);

  lv_obj_t *label = lv_label_create(container);
  // lv_obj_set_style_text_font(label, &pixel_mplus_10, LV_PART_MAIN);
  lv_obj_set_style_text_font(label, &micro5_10, LV_PART_MAIN);
  lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
  lv_obj_set_align(label, LV_ALIGN_CENTER);
  lv_label_set_text(label, "UNDER\nCONSTRUCTION\n01234567890");

  return container;
}

lv_obj_t *zmk_display_status_screen() {
  lv_obj_t *container = container_default(NULL);

  zmk_status_presenter_init();

  lv_obj_set_size(container, WIDTH, HEIGHT);
  lv_obj_set_flex_flow(container, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_flex_align(container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  lv_obj_set_style_pad_gap(container, 1, LV_PART_MAIN);

#if IS_ENABLED(CONFIG_ZMK_WIDGET_HID_INDICATORS_H11) || \
  IS_ENABLED(CONFIG_ZMK_WIDGET_BATTERY_STATUS_23X12)
  top_bar_create(container);
#endif

  endpoint_status_create(container);

#if IS_ENABLED(CONFIG_ZMK_WIDGET_LAYER_STATUS_H15)
  lv_layer_status_create(container, container_default, LV_ALIGN_TOP_LEFT, CONTENT_WIDTH);
#endif
#if IS_ENABLED(CONFIG_ZMK_WIDGET_LAYER_INDICATORS_H9)
  lv_layer_indicators_create(container, container_default, LV_ALIGN_CENTER, 49);
#endif

  lv_obj_t *content = content_create(container);
  lv_obj_set_width(content, CONTENT_WIDTH);
  lv_obj_set_flex_grow(content, 1);

  lv_obj_t *zmk_desc = lv_label_create(container);
  // lv_obj_set_style_text_font(zmk_desc, &pixel_mplus_10, 0);
  lv_obj_set_style_text_font(zmk_desc, &micro5_10, 0);
  lv_label_set_text_static(zmk_desc, "Powered By");

  lv_obj_t *zmk_logo = lv_img_create(container);
  lv_img_set_src(zmk_logo, &zmk_logo_40x40);
  lv_obj_set_style_pad_bottom(zmk_logo, MARGIN, 0);

  zmk_status_presenter_dispatch(container, 2);

  return container;
}
