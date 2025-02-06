/*
 * Copyright (c) 2024 The ZMK Contributors
 * SPDX-License-Identifier: MIT
 */
#include <zmk/display/status_screen.h>

#include <zmk/display/status_presenter.h>
#include <zmk/display/custom_widgets/peripheral_status.h>
#include <zmk/display/custom_widgets/battery_status.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#define MARGIN 2

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

LV_FONT_DECLARE(micro5_10);

#if IS_ENABLED(CONFIG_ZMK_CUSTOM_KEYBOARD_LOGO)
LV_IMG_DECLARE(keyboard_logo);
#else
LV_IMG_DECLARE(starman_68x80);
#endif

static inline lv_obj_t *status_header_create(lv_obj_t *parent) {
  lv_obj_t *container = container_default(parent);
  lv_obj_set_flex_flow(container, LV_FLEX_FLOW_ROW_WRAP);
  lv_obj_set_size(container, lv_pct(100), LV_SIZE_CONTENT);
  lv_obj_set_flex_align(container, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER,
                        LV_FLEX_ALIGN_START);
  lv_obj_set_style_pad_gap(container, MARGIN, LV_PART_MAIN);

#if IS_ENABLED(CONFIG_ZMK_WIDGET_CUSTOM_PERIPHERAL_STATUS)
  lv_peripheral_status_create(container, container_default);
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_CUSTOM_BATTERY_STATUS)
  lv_battery_status_create(container, container_default);
#endif

  return container;
}

static inline lv_obj_t *content_create(lv_obj_t *parent) {
  lv_obj_t *container = container_default(parent);
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

static inline lv_obj_t *kbd_logo_create(lv_obj_t *parent) {
  lv_obj_t *kbd_logo = lv_img_create(parent);
#if IS_ENABLED(CONFIG_ZMK_CUSTOM_KEYBOARD_LOGO)
  lv_img_set_src(kbd_logo, &keyboard_logo);
#else
  lv_img_set_src(kbd_logo, &starman_68x80);
#endif
#if IS_ENABLED(CONFIG_ZMK_DISPLAY_INVERT)
  lv_color_t color = lv_obj_get_style_text_color(parent, LV_PART_MAIN);
  lv_obj_set_style_img_recolor_opa(kbd_logo, LV_OPA_COVER, LV_PART_MAIN);
  lv_obj_set_style_img_recolor(kbd_logo, color, LV_PART_MAIN);
#endif
  return kbd_logo;
}

lv_obj_t *zmk_display_status_screen() {
  lv_obj_t *screen = container_default(NULL);

  zmk_status_presenter_init();

  lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_flex_align(screen, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  lv_obj_set_style_pad_row(screen, MARGIN, LV_PART_MAIN);

  status_header_create(screen);
  content_create(screen);
  kbd_logo_create(screen);

  zmk_status_presenter_dispatch(screen, 3);

  return screen;
}
