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
#include "font/lv_font.h"
#include "misc/lv_area.h"
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#if IS_ENABLED(CONFIG_ZMK_WIDGET_BATTERY_STATUS_80X128_MONO)
static struct zmk_widget_battery_status battery_status_widget;
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_OUTPUT_STATUS_80X128_MONO)
static struct zmk_widget_output_status output_status_widget;
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_PERIPHERAL_STATUS_80X128_MONO)
static struct zmk_widget_peripheral_status peripheral_status_widget;
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_LAYER_STATUS_80X128_MONO)
static struct zmk_widget_layer_status layer_status_widget;
#endif

#define MARGIN_BOTTOM 2
#define MARGIN_LEFT 2
#define MARGIN_RIGHT 2

#if !IS_ENABLED(CONFIG_ZMK_SPLIT) || IS_ENABLED(CONFIG_ZMK_SPLIT_ROLE_CENTRAL)

LV_FONT_DECLARE(pixel_mplus_bold_ascii_12);
LV_FONT_DECLARE(cozetta_icons_13);

/* 0x0f03a  nf-fa-list */
#  define NF_FA_LIST "\xEF\x80\xBA"

/* 0x0f831=>0xf0ab7 =>󰪷 nf-md-text_box_multiple */
#  define NF_MDI_TEXT_BOX_MULTIPLE "\xEF\xA0\xB1"
#  define NF_MD_TEXT_BOX_MULTIPLE "\xF3\xB0\xAA\xB7"

static void create_static_label(lv_obj_t *parent, const char *text, lv_align_t align, lv_coord_t x,
                                lv_coord_t y) {
  lv_obj_t *label = lv_label_create(parent);

  lv_obj_set_style_text_font(label, &pixel_mplus_bold_ascii_12, 0);
  lv_label_set_text_static(label, text);
  lv_obj_align(label, align, x, y);
}
static void create_static_icon(lv_obj_t *parent, const char *text, lv_align_t align, lv_coord_t x,
                               lv_coord_t y) {
  lv_obj_t *label = lv_label_create(parent);

  lv_obj_set_style_text_font(label, &cozetta_icons_13, 0);
  lv_label_set_text_static(label, text);
  lv_obj_align(label, align, x, y);
}
#endif

lv_obj_t *zmk_display_status_screen() {
  lv_obj_t *screen;
  screen = lv_obj_create(NULL);
  int y = MARGIN_BOTTOM;
#if IS_ENABLED(CONFIG_ZMK_WIDGET_OUTPUT_STATUS_80X128_MONO)
  zmk_widget_output_status_init(&output_status_widget, screen);
  lv_obj_align(zmk_widget_output_status_obj(&output_status_widget), LV_ALIGN_TOP_LEFT, MARGIN_LEFT,
               y);
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_PERIPHERAL_STATUS_80X128_MONO)
  zmk_widget_peripheral_status_init(&peripheral_status_widget, screen);
  lv_obj_align(zmk_widget_peripheral_status_obj(&peripheral_status_widget), LV_ALIGN_TOP_LEFT,
               MARGIN_LEFT, y);
#endif
#if IS_ENABLED(CONFIG_ZMK_WIDGET_BATTERY_STATUS_80X128_MONO)
  zmk_widget_battery_status_init(&battery_status_widget, screen);
  lv_obj_align(zmk_widget_battery_status_obj(&battery_status_widget), LV_ALIGN_TOP_RIGHT,
               -MARGIN_RIGHT, y);
#endif

  y += 13 + MARGIN_BOTTOM;

#if IS_ENABLED(CONFIG_ZMK_WIDGET_LAYER_STATUS_80X128_MONO)

  // create_static_icon(screen, NF_MDI_TEXT_BOX_MULTIPLE, LV_ALIGN_TOP_LEFT, MARGIN_LEFT, y);
  create_static_icon(screen, NF_FA_LIST, LV_ALIGN_TOP_LEFT, MARGIN_LEFT, y);

  zmk_widget_layer_status_init(&layer_status_widget, screen);
  lv_obj_align(zmk_widget_layer_status_obj(&layer_status_widget), LV_ALIGN_TOP_LEFT,
               MARGIN_LEFT + 9, y - 2);
  y += 13 + MARGIN_BOTTOM;
#endif

  // TODO do something fun

#if !IS_ENABLED(CONFIG_ZMK_SPLIT) || IS_ENABLED(CONFIG_ZMK_SPLIT_ROLE_CENTRAL)
  create_static_label(screen, "Powered By", LV_ALIGN_BOTTOM_MID, 0, -40 - MARGIN_BOTTOM);
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
  return screen;
}
