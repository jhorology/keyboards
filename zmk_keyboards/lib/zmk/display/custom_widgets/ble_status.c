/*
 * Copyright (c) 2024 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */
#include <lvgl.h>
#include <zmk/display/lv_zmk_event.h>
#include <zmk/display/lv_zmk_status.h>
#include <zmk/display/custom_widgets/ble_status.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

/* 0x0e615  nf-seti-config */
#define NF_SETI_CONFIG "\xEE\x98\x95"
/* 0x0f00c  nf-fa-check */
#define NF_FA_CHECK "\xEF\x80\x8C"
/* 0x0f00d  nf-fa-close nf-fa-times nf-fa-xmark */
#define NF_FA_CLOSE "\xEF\x80\x8D"
/* 0x0f293  nf-fa-bluetooth */
#define NF_FA_BLUETOOTH "\xEF\x8A\x93"
/* 0x0f5b1=>0xf00b2 =>󰂲 nf-md-bluetooth_off */
#define NF_MDI_BLUETOOTH_OFF "\xEF\x96\xB1"
#define NF_MD_BLUETOOTH_OFF "\xF3\xB0\x82\xb2"
/* 0x0f5b2=>0xf00b3 =>󰂳 nf-md-bluetooth_settings */
#define NF_MDI_BLUETOOTH_SETTING "\xEF\x96\xB2"
#define NF_MD_BLUETOOTH_OFF_SETTING "\xF3\xB0\x82\xb3"

static const lv_zmk_event_interests zmk_event_interests =
  LV_ZMK_EVENT_INTERESTS(ble_active_profile, endpoint);

LV_FONT_DECLARE(pixel_mplus_bold_10);
LV_FONT_DECLARE(cozetta_icons_13);

static void ble_active_profile_cb(lv_event_t *event) {
  lv_obj_t *container = lv_event_get_current_target(event);
  struct lv_zmk_status *state = lv_event_get_param(event);
  lv_obj_t *icon_label = lv_obj_get_child(container, 0);
  lv_obj_t *index_label = lv_obj_get_child(container, 1);

  switch (state->ble_conn_state) {
    case BLE_NOT_PAIERD:
      lv_label_set_text(icon_label, NF_MDI_BLUETOOTH_SETTING);
      break;
    case BLE_DISCONNECTED:
      lv_label_set_text(icon_label, NF_MDI_BLUETOOTH_OFF);
      break;
    case BLE_CONNECTED:
      lv_label_set_text(icon_label, NF_FA_BLUETOOTH);
      break;
  }
  lv_label_set_text_fmt(index_label, "%i", state->ble_profile_index);
}

#if IS_ENABLED(CONFIG_ZMK_USB) && IS_ENABLED(CONFIG_ZMK_BLE)
static void endpoint_cb(lv_event_t *event) {
  lv_obj_t *container = lv_event_get_current_target(event);
  struct lv_zmk_status *state = lv_event_get_param(event);

  // normal color
  lv_obj_t *parent = lv_obj_get_parent(container);
  lv_color_t bg_color = lv_obj_get_style_bg_color(parent, LV_PART_MAIN);
  lv_color_t text_color = lv_obj_get_style_text_color(parent, LV_PART_MAIN);

  lv_obj_set_style_bg_color(container, state->ble_selected ? text_color : bg_color, LV_PART_MAIN);
  lv_obj_set_style_text_color(container, state->ble_selected ? bg_color : text_color, LV_PART_MAIN);
}
#endif

lv_obj_t *lv_ble_status_create(lv_obj_t *parent, lv_style_t *container_style) {
  lv_obj_t *container = lv_obj_create(parent);
  if (container_style != NULL) {
    lv_obj_add_style(container, container_style, LV_PART_MAIN);
  }
  lv_obj_set_flex_flow(container, LV_FLEX_FLOW_ROW);
  lv_obj_set_style_pad_column(container, 2, LV_PART_MAIN);
  lv_obj_set_flex_align(container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_START);
  lv_obj_set_style_pad_hor(container, 1, LV_PART_MAIN);

  lv_obj_set_style_opa(container, LV_OPA_COVER, LV_PART_MAIN);

  lv_obj_t *icon_label = lv_label_create(container);
  lv_obj_set_style_text_font(icon_label, &cozetta_icons_13, LV_PART_MAIN);

  lv_obj_t *index_label = lv_label_create(container);
  lv_obj_set_style_text_font(index_label, &pixel_mplus_bold_10, LV_PART_MAIN);

  lv_obj_add_event_cb(container, ble_active_profile_cb, LV_ZMK_EVENT_CODE(ble_active_profile),
                      NULL);

#if IS_ENABLED(CONFIG_ZMK_USB) && IS_ENABLED(CONFIG_ZMK_BLE)
  lv_obj_add_event_cb(container, endpoint_cb, LV_ZMK_EVENT_CODE(endpoint), NULL);
#endif

  lv_obj_set_user_data(container, (void *)&zmk_event_interests);

  return container;
}
