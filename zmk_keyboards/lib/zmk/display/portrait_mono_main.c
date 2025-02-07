/*
 * Copyright (c) 2024 The ZMK Contributors
 * SPDX-License-Identifier: MIT
 */
#include <zmk/display/status_screen.h>

#include <zmk/display/status_presenter.h>
#include <zmk/display/custom_widgets/usb_status.h>
#include <zmk/display/custom_widgets/ble_status.h>
#include <zmk/display/custom_widgets/battery_status.h>
#include <zmk/display/custom_widgets/layer_status.h>
#include <zmk/display/custom_widgets/layer_indicators.h>
#include <zmk/display/custom_widgets/hid_indicators.h>

#include <app_version.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

// 7digits SHA same as 'git rev-parse --short'
#define _UINT64_0(a) _UINT64_1(a)
#define _UINT64_1(a) 0x##a##UL
#define APP_BUILD_VERSION_SHORT_REV (uint32_t)((_UINT64_0(APP_BUILD_VERSION) >> 20) & 0xfffffff)

#define MARGIN 2

LV_FONT_DECLARE(micro5_10);
LV_IMG_DECLARE(zmk_logo_40x40);

struct portrait_mono_styles {
  lv_style_t container;
};

static struct portrait_mono_styles styles;

static void style_init(struct portrait_mono_styles *styles) {
  lv_style_init(&styles->container);
  lv_style_set_pad_all(&styles->container, 0);
  lv_style_set_pad_gap(&styles->container, 0);
  lv_style_set_border_width(&styles->container, 0);
  lv_style_set_size(&styles->container, LV_SIZE_CONTENT);
}

static lv_obj_t *container_create(lv_obj_t *parent) {
  lv_obj_t *container = lv_obj_create(parent);
  lv_obj_add_style(container, &styles.container, LV_PART_MAIN);
  return container;
}

#if IS_ENABLED(CONFIG_ZMK_WIDGET_CUSTOM_BLE_STATUS) && \
  IS_ENABLED(CONFIG_ZMK_WIDGET_CUSTOM_USB_STATUS)
static inline lv_obj_t *endpoint_status_create(lv_obj_t *parent) {
  style_init(&styles);

  lv_obj_t *container = container_create(parent);
  lv_obj_set_flex_flow(container, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_START);
  lv_obj_set_style_pad_gap(container, MARGIN, LV_PART_MAIN);

  lv_ble_status_create(container, &styles.container);

  // normal color
  lv_color_t text_color = lv_obj_get_style_text_color(container, LV_PART_MAIN);

  // separator
  lv_obj_t *separator = lv_obj_create(container);
  lv_obj_set_style_opa(separator, LV_OPA_COVER, LV_PART_MAIN);
  lv_obj_set_style_bg_color(separator, text_color, LV_PART_MAIN);
  lv_obj_set_size(separator, 1, 12);

  lv_usb_status_create(container, &styles.container);

  return container;
}
#endif

static inline lv_obj_t *status_header_create(lv_obj_t *parent) {
  lv_obj_t *container = container_create(parent);
  lv_obj_set_flex_flow(container, LV_FLEX_FLOW_ROW_WRAP);
  lv_obj_set_width(container, lv_pct(100));
  lv_obj_set_flex_align(container, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER,
                        LV_FLEX_ALIGN_START);
  lv_obj_set_style_pad_gap(container, MARGIN, LV_PART_MAIN);

#if IS_ENABLED(CONFIG_ZMK_WIDGET_CUSTOM_BLE_STATUS) && \
  IS_ENABLED(CONFIG_ZMK_WIDGET_CUSTOM_USB_STATUS)
  endpoint_status_create(container);
#elif IS_ENABLED(CONFIG_ZMK_WIDGET_CUSTOM_BLE_STATUS)
  lv_ble_status_create(container, &styles.container);
#elif IS_ENABLED(CONFIG_ZMK_WIDGET_CUSTOM_USB_STATUS)
  lv_usb_status_create(container, &styles.container);
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_CUSTOM_BATTERY_STATUS)
  lv_battery_status_create(container, &styles.container);
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_CUSTOM_HID_INDICATORS)
  lv_hid_indicators_create(container, &styles.container);
#endif

  return container;
}

static inline lv_obj_t *content_create(lv_obj_t *parent) {
  lv_obj_t *container = container_create(parent);
  lv_obj_set_width(container, lv_pct(100));
  lv_obj_set_flex_grow(container, 1);
  lv_obj_set_style_border_width(container, 1, LV_PART_MAIN);

  lv_obj_t *label = lv_label_create(container);
  lv_obj_set_style_text_font(label, &micro5_10, LV_PART_MAIN);
  lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
  lv_obj_set_align(label, LV_ALIGN_CENTER);
  lv_label_set_text(label, "UNDER\nCONSTRUCTION");

  return container;
}

static inline lv_obj_t *zmk_logo_create(lv_obj_t *parent) {
  lv_obj_t *container = container_create(parent);
  lv_obj_set_flex_flow(container, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_flex_align(container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
  lv_obj_set_style_pad_row(container, MARGIN, LV_PART_MAIN);

  lv_obj_t *zmk_desc = lv_label_create(container);
  lv_obj_set_style_text_font(zmk_desc, &micro5_10, 0);
  lv_label_set_text_static(zmk_desc, "POWERED BY");

  lv_obj_t *zmk_logo = lv_img_create(container);
  lv_img_set_src(zmk_logo, &zmk_logo_40x40);

#if IS_ENABLED(CONFIG_ZMK_DISPLAY_INVERT)
  lv_color_t img_color = lv_obj_get_style_text_color(container, LV_PART_MAIN);
  lv_obj_set_style_img_recolor_opa(zmk_logo, LV_OPA_COVER, LV_PART_MAIN);
  lv_obj_set_style_img_recolor(zmk_logo, img_color, LV_PART_MAIN);
#endif

  lv_obj_t *ver = lv_label_create(container);
  lv_obj_set_style_text_font(ver, &micro5_10, 0);
  // version + 7digits SHA
  lv_label_set_text_fmt(ver, "v%s (%07X)", APP_VERSION_STRING, APP_BUILD_VERSION_SHORT_REV);
  return container;
}

lv_obj_t *zmk_display_status_screen() {
  zmk_status_presenter_init();
  style_init(&styles);

  lv_obj_t *screen = lv_obj_create(NULL);
  lv_obj_set_scrollbar_mode(screen, LV_SCROLLBAR_MODE_OFF);
  lv_obj_set_style_pad_all(screen, 0, LV_PART_MAIN);
  lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_flex_align(screen, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  lv_obj_set_style_pad_row(screen, MARGIN, LV_PART_MAIN);

  status_header_create(screen);

#if IS_ENABLED(CONFIG_ZMK_WIDGET_CUSTOM_LAYER_STATUS)
  lv_layer_status_create(screen, &styles.container, lv_pct(100));
#endif

  content_create(screen);

  zmk_logo_create(screen);

  zmk_status_presenter_dispatch(screen, 3);

  return screen;
}
