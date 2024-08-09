/*
 *
 * Copyright (c) 2021 Darryl deHaan
 * SPDX-License-Identifier: MIT
 *
 */

#include <zephyr/logging/log.h>
#include <zmk/display/status_screen.h>

#include "widgets/battery_status.h"
#include "widgets/peripheral_status.h"
#include "widgets/output_status.h"
#include "widgets/layer_status.h"

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

LV_IMG_DECLARE(layers2);

#if IS_ENABLED(CONFIG_ZMK_DISPLAY_CUSTOM_WIDGETS_80X128_MONO_BATTERY)
static struct zmk_widget_battery_status battery_status_widget;
#endif

#if IS_ENABLED(CONFIG_ZMK_DISPLAY_CUSTOM_WIDGETS_80X128_MONO_OUTPUT)
static struct zmk_widget_output_status output_status_widget;
#endif

#if IS_ENABLED(CONFIG_ZMK_DISPLAY_CUSTOM_WIDGETS_80X128_MONO_PERIPHERAL)
static struct zmk_widget_peripheral_status peripheral_status_widget;
#endif

#if IS_ENABLED(CONFIG_ZMK_DISPLAY_CUSTOM_WIDGETS_80X128_MONO_LAYER)
static struct zmk_widget_layer_status layer_status_widget;
#endif

lv_obj_t *zmk_display_status_screen() {
  lv_obj_t *screen;
  screen = lv_obj_create(NULL);

#if IS_ENABLED(CONFIG_ZMK_DISPLAY_CUSTOM_WIDGETS_80X128_MONO_BATTERY)
  zmk_widget_battery_status_init(&battery_status_widget, screen);
  lv_obj_align(zmk_widget_battery_status_obj(&battery_status_widget), LV_ALIGN_TOP_MID, 0, 2);
#endif

#if IS_ENABLED(CONFIG_ZMK_DISPLAY_CUSTOM_WIDGETS_80X128_MONO_OUTPUT)
  zmk_widget_output_status_init(&output_status_widget, screen);
  lv_obj_align(zmk_widget_output_status_obj(&output_status_widget), LV_ALIGN_TOP_MID, 0, 41);
#endif

#if IS_ENABLED(CONFIG_ZMK_DISPLAY_CUSTOM_WIDGETS_80X128_MONO_PERIPHERAL)
  zmk_widget_peripheral_status_init(&peripheral_status_widget, screen);
  lv_obj_align(zmk_widget_peripheral_status_obj(&peripheral_status_widget), LV_ALIGN_TOP_MID, 0,
               41);
#endif

#if IS_ENABLED(CONFIG_ZMK_DISPLAY_CUSTOM_WIDGETS_80X128_MONO_LAYER)
  lv_obj_t *LayersHeading;
  LayersHeading = lv_img_create(screen);
  lv_obj_align(LayersHeading, LV_ALIGN_BOTTOM_MID, 0, -30);
  lv_img_set_src(LayersHeading, &layers2);

  zmk_widget_layer_status_init(&layer_status_widget, screen);
  lv_obj_set_style_text_font(zmk_widget_layer_status_obj(&layer_status_widget),
                             &lv_font_montserrat_16, LV_PART_MAIN);
  lv_obj_align(zmk_widget_layer_status_obj(&layer_status_widget), LV_ALIGN_BOTTOM_MID, 0, -5);
#endif

#if IS_ENABLED(CONFIG_ZMK_DISPLAY_CUSTOM_WIDGETS_80X128_MONO_LOGO)
  lv_obj_t *logo_icon;
  logo_icon = lv_img_create(screen);
#  if IS_ENABLED(CONFIG_ZMK_DISPLAY_CUSTOM_WIDGETS_80X128_MONO_LOGO_IMAGE_ZEN)
  LV_IMG_DECLARE(zenlogo);
  lv_img_set_src(logo_icon, &zenlogo);
#  elif IS_ENABLED(CONFIG_ZMK_DISPLAY_CUSTOM_WIDGETS_80X128_MONO_LOGO_IMAGE_IMAGE_LPKB)
  LV_IMG_DECLARE(lpkblogo);
  lv_img_set_src(logo_icon, &lpkblogo);
#  elif IS_ENABLED(CONFIG_ZMK_DISPLAY_CUSTOM_WIDGETS_80X128_MONO_LOGO_IMAGE_ZMK)
  LV_IMG_DECLARE(zmklogo);
  lv_img_set_src(logo_icon, &zmklogo);
#  elif IS_ENABLED(CONFIG_ZMK_DISPLAY_CUSTOM_WIDGETS_80X128_MONO_LOGO_IMAGE_MIRYOKU)
  LV_IMG_DECLARE(miryokulogo);
  lv_img_set_src(logo_icon, &miryokulogo);
#  endif
  lv_obj_align(logo_icon, LV_ALIGN_BOTTOM_MID, 0, -5);
#endif  // CONFIG_CUSTOM_WIDGET_LOGO

  return screen;
}
