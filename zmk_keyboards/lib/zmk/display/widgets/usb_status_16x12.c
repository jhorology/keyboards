/*
 * Copyright (c) 2024 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <lvgl.h>
#include <zmk/display/lv_zmk_event.h>
#include <zmk/display/lv_zmk_status.h>
#include <zmk/display/util_macros.h>
#include <zmk/display/widgets/usb_status_16x12.h>
#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

/* 0x0f00c  nf-fa-check */
#define NF_FA_CHECK "\xEF\x80\x8C"
/* 0x0f00d  nf-fa-close nf-fa-times nf-fa-xmark */
#define NF_FA_CLOSE "\xEF\x80\x8D"
/* 0x0f179  nf-fa-apple */
#define NF_FA_APPLE "\xEF\x85\xB9"
/* 0x0f17a  nf-fa-windows */
#define NF_FA_WINDOWS "\xEF\x85\xBA"
/* 0x0e725  nf-dev-git_branch */
#define NF_DEV_GIT_BRANCH "\xEE\x9C\xA5"

#define WIDTH 16
#define HEIGHT 12

static const lv_zmk_event_interests zmk_event_interests =
  LV_ZMK_EVENT_INTERESTS(usb_conn_state, usb_host_os, endpoint);

LV_FONT_DECLARE(cozetta_icons_13);

static void usb_conn_state_cb(lv_event_t *event) {
  lv_obj_t *container = lv_event_get_current_target(event);
  struct lv_zmk_status *state = lv_event_get_param(event);
  lv_obj_t *icon_label = lv_obj_get_child(container, 1);

  switch (state->usb_conn_state) {
    case USB_DISCONNECTED:
      lv_label_set_text(icon_label, NF_FA_CLOSE);
      break;
    case USB_CONNECTED:
      lv_label_set_text(icon_label, NF_FA_CHECK);

      break;
    case USB_CONNECTED_MAC:
      lv_label_set_text(icon_label, NF_FA_APPLE);
      break;
    case USB_CONNECTED_WIN:
      lv_label_set_text(icon_label, NF_FA_WINDOWS);
      break;
    default:
      break;
  }
}

#if IS_ENABLED(CONFIG_ZMK_USB) && IS_ENABLED(CONFIG_ZMK_BLE)
static void endpoint_cb(lv_event_t *event) {
  lv_obj_t *container = lv_event_get_current_target(event);
  struct lv_zmk_status *state = lv_event_get_param(event);

  lv_obj_set_style_bg_color(container, state->usb_selected ? lv_color_black() : lv_color_white(),
                            LV_PART_MAIN);
  lv_obj_set_style_text_color(container, state->usb_selected ? lv_color_white() : lv_color_black(),
                              LV_PART_MAIN);
}
#endif

lv_obj_t *lv_usb_status_create(lv_obj_t *parent, lv_obj_t *(*container_default)(lv_obj_t *),
                               lv_align_t align) {
  lv_obj_t *container =
    container_default != NULL ? container_default(parent) : lv_obj_create(parent);
  lv_obj_set_size(container, WIDTH, HEIGHT);
  lv_obj_set_flex_flow(container, LV_FLEX_FLOW_ROW);
  lv_obj_set_style_pad_gap(container, 1, LV_PART_MAIN);
  lv_obj_set_style_pad_top(container, 1, LV_PART_MAIN);
  ALIGN_FLEX_FLOW_ROW_COMPOSITE_WIDGET(container, align, LV_FLEX_ALIGN_END);

  lv_obj_t *title_label = lv_label_create(container);
  lv_obj_set_style_text_font(title_label, &cozetta_icons_13, 0);
  lv_label_set_text_static(title_label, NF_DEV_GIT_BRANCH);

  lv_obj_t *icon_label = lv_label_create(container);
  lv_obj_set_style_text_font(icon_label, &cozetta_icons_13, 0);

  // events

  lv_obj_add_event_cb(container, usb_conn_state_cb, LV_ZMK_EVENT_CODE(usb_conn_state), NULL);
  lv_obj_add_event_cb(container, usb_conn_state_cb, LV_ZMK_EVENT_CODE(usb_host_os), NULL);
#if IS_ENABLED(CONFIG_ZMK_USB) && IS_ENABLED(CONFIG_ZMK_BLE)
  lv_obj_add_event_cb(container, endpoint_cb, LV_ZMK_EVENT_CODE(endpoint), NULL);
#endif

  lv_obj_set_user_data(container, (void *)&zmk_event_interests);

  return container;
}
