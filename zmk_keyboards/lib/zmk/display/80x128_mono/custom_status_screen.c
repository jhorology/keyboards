/*
 * Copyright (c) 2020 The ZMK Contributors
 * SPDX-License-Identifier: MIT
 */

#include <zmk/display/widgets/output_status.h>
#include <zmk/display/widgets/peripheral_status.h>
#include <zmk/display/widgets/battery_status.h>
#include <zmk/display/widgets/layer_status.h>
#include <zmk/display/widgets/wpm_status.h>
#include <zmk/display/status_screen.h>

#include <zephyr/logging/log.h>
#include "misc/lv_area.h"
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#if IS_ENABLED(CONFIG_ZMK_WIDGET_BATTERY_STATUS_80X128_MONO)
static struct zmk_widget_battery_status battery_status_widget;
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_OUTPUT_STATUS_80X128_MONO)
static struct zmk_widget_output_status output_status_widget;
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_PERIPHERAL_STATUS)
static struct zmk_widget_peripheral_status peripheral_status_widget;
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_LAYER_STATUS)
static struct zmk_widget_layer_status layer_status_widget;
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_WPM_STATUS)
static struct zmk_widget_wpm_status wpm_status_widget;
#endif

#define MARGIN_BOTTOM 2
#define MARGIN_LEFT 2
#define MARGIN_RIGHT 2

static void create_label(lv_obj_t *parent, const char *text, lv_align_t align, lv_coord_t x,
                         lv_coord_t y) {
  lv_obj_t *label = lv_label_create(parent);
  lv_obj_set_style_text_font(label, lv_theme_get_font_small(parent), LV_PART_MAIN);
  lv_label_set_text(label, text);
  lv_obj_align(label, align, x, y);
}

lv_obj_t *zmk_display_status_screen() {
  lv_obj_t *screen;
  screen = lv_obj_create(NULL);
  int y = MARGIN_BOTTOM;
#if IS_ENABLED(CONFIG_ZMK_WIDGET_LAYER_STATUS)

  zmk_widget_layer_status_init(&layer_status_widget, screen);
  lv_obj_set_style_text_font(zmk_widget_layer_status_obj(&layer_status_widget),
                             lv_theme_get_font_small(screen), LV_PART_MAIN);
  lv_obj_align(zmk_widget_layer_status_obj(&layer_status_widget), LV_ALIGN_TOP_LEFT, MARGIN_LEFT,
               y);
  y += 14 + MARGIN_BOTTOM;
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_BATTERY_STATUS_80X128_MONO)
  // create_label(screen, "BAT", LV_ALIGN_TOP_LEFT, MARGIN_LEFT, y + 2);
  zmk_widget_battery_status_init(&battery_status_widget, screen);
  lv_obj_align(zmk_widget_battery_status_obj(&battery_status_widget), LV_ALIGN_TOP_LEFT,
               MARGIN_LEFT, y);
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_OUTPUT_STATUS_80X128_MONO)
  // create_label(screen, "OUT", LV_ALIGN_TOP_LEFT, MARGIN_LEFT, y + 2);
  zmk_widget_output_status_init(&output_status_widget, screen);
  lv_obj_align(zmk_widget_output_status_obj(&output_status_widget), LV_ALIGN_TOP_RIGHT,
               -MARGIN_RIGHT, y);
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_PERIPHERAL_STATUS)
  // create_label(screen, "LINK", LV_ALIGN_TOP_LEFT, MARGIN_LEFT, y + 2);
  zmk_widget_peripheral_status_init(&peripheral_status_widget, screen);
  lv_obj_align(zmk_widget_peripheral_status_obj(&peripheral_status_widget), LV_ALIGN_TOP_RIGHT,
               -MARGIN_RIGHT, y);
#endif
  y += 14 + MARGIN_BOTTOM;

  // TODO do something fun
#if !IS_ENABLED(CONFIG_ZMK_SPLIT) || IS_ENABLED(CONFIG_ZMK_SPLIT_ROLE_CENTRAL)
  create_label(screen, "Powered By", LV_ALIGN_BOTTOM_MID, 0, -40 - MARGIN_BOTTOM);
  lv_obj_t *zmk = lv_img_create(screen);
  LV_IMG_DECLARE(zmk_logo_40x40);
  lv_img_set_src(zmk, &zmk_logo_40x40);
  lv_obj_align(zmk, LV_ALIGN_BOTTOM_MID, 0, -MARGIN_BOTTOM);
#endif
#if IS_ENABLED(CONFIG_ZMK_SPLIT) && !IS_ENABLED(CONFIG_ZMK_SPLIT_ROLE_CENTRAL)
  lv_obj_t *custom_logo = lv_img_create(screen);
  LV_IMG_DECLARE(custom_logo_80x80);
  lv_img_set_src(custom_logo, &custom_logo_80x80);
  lv_obj_align(custom_logo, LV_ALIGN_BOTTOM_MID, 0, 0);
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_WPM_STATUS)
  zmk_widget_wpm_status_init(&wpm_status_widget, screen);
  lv_obj_align(zmk_widget_wpm_status_obj(&wpm_status_widget), LV_ALIGN_BOTTOM_RIGHT, 0, 0);
#endif

  return screen;
}
