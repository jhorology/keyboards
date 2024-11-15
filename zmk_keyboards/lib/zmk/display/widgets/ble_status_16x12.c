/*
 * Copyright (c) 2024 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/display.h>
#include <zmk/event_manager.h>
#include <zmk/events/ble_active_profile_changed.h>
#include <zmk/events/endpoint_changed.h>
#include <zmk/ble.h>
#include <zmk/endpoints.h>

#include <zmk/display/util_macros.h>
#include <zmk/display/widgets/ble_status_16x12.h>

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

#define WIDTH 16
#define HEIGHT 12

enum icon { ICON_OPEN, ICON_CLOSE, ICON_SETTING };

struct ble_status_state {
  struct zmk_endpoint_instance selected_endpoint;
  bool active_profile_connected;
  bool active_profile_bonded;
};

LV_FONT_DECLARE(pixel_mplus_bold_10);
LV_FONT_DECLARE(cozetta_icons_13);

static struct zmk_lv_ble_status_user_data user_data;
static bool initialized;

static struct ble_status_state get_state(const zmk_event_t *eh) {
  return (struct ble_status_state){
    .selected_endpoint = zmk_endpoints_selected(),
    .active_profile_connected = zmk_ble_active_profile_is_connected(),
    .active_profile_bonded = !zmk_ble_active_profile_is_open()};
}

static void ble_status_update_cb(struct ble_status_state state) {
#if IS_ENABLED(CONFIG_ZMK_WIDGET_ENDPOINT_INDICATOR)
  static bool is_ble;
  bool new_is_ble = state.selected_endpoint.transport == ZMK_TRANSPORT_BLE;
  if (initialized || new_is_ble != is_ble) {
    is_ble = new_is_ble;
    lv_obj_t *container = lv_obj_get_parent(user_data.icon_label);
    lv_obj_set_style_bg_color(container, is_ble ? lv_color_black() : lv_color_white(),
                              LV_PART_MAIN);
    lv_obj_set_style_text_color(container, is_ble ? lv_color_white() : lv_color_black(),
                                LV_PART_MAIN);
  }
#endif

  static enum icon icon;
  enum icon new_icon = !state.active_profile_bonded     ? ICON_SETTING
                       : state.active_profile_connected ? ICON_OPEN
                                                        : ICON_CLOSE;
  if (initialized || new_icon != icon) {
    icon = new_icon;
    switch (icon) {
      case ICON_OPEN:
        lv_label_set_text(user_data.icon_label, NF_FA_BLUETOOTH);
        break;
      case ICON_CLOSE:
        lv_label_set_text(user_data.icon_label, NF_MDI_BLUETOOTH_OFF);
        break;
      case ICON_SETTING:
        lv_label_set_text(user_data.icon_label, NF_MDI_BLUETOOTH_SETTING);
        break;
    }
  }

  static uint8_t index;
  uint8_t new_index = state.selected_endpoint.ble.profile_index;
  if (initialized || new_index != index) {
    index = new_index;
    lv_label_set_text_fmt(user_data.index_label, "%i", index + 1);
  }
  if (initialized) initialized = false;
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_ble_status, struct ble_status_state, ble_status_update_cb,
                            get_state)

ZMK_SUBSCRIPTION(widget_ble_status, zmk_endpoint_changed);
#if IS_ENABLED(CONFIG_ZMK_BLE)
ZMK_SUBSCRIPTION(widget_ble_status, zmk_ble_active_profile_changed);
#endif

lv_obj_t *zmk_lv_ble_status_create(lv_obj_t *parent, lv_obj_t *(*container_default)(lv_obj_t *),
                                   lv_align_t align) {
  lv_obj_t *container =
    container_default != NULL ? container_default(parent) : lv_obj_create(parent);
  lv_obj_set_size(container, WIDTH, HEIGHT);
  lv_obj_set_flex_flow(container, LV_FLEX_FLOW_ROW);
  lv_obj_set_style_pad_gap(container, 1, LV_PART_MAIN);
  ALIGN_FLEX_FLOW_ROW_COMPOSITE_WIDGET(container, align, LV_FLEX_ALIGN_END);

  lv_obj_set_style_opa(container, LV_OPA_COVER, LV_PART_MAIN);

  lv_obj_t *icon_label = lv_label_create(container);
  lv_obj_set_style_text_font(icon_label, &cozetta_icons_13, LV_PART_MAIN);

  lv_obj_t *index_label = lv_label_create(container);
  lv_obj_set_style_text_font(index_label, &pixel_mplus_bold_10, LV_PART_MAIN);

  user_data.icon_label = icon_label;
  user_data.index_label = index_label;
  lv_obj_set_user_data(container, &user_data);

  widget_ble_status_init();

  initialized = true;
  return container;
}
