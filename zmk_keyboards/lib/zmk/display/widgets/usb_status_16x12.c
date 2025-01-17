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
#include <zmk/events/endpoint_changed.h>
#include <zmk/events/usb_conn_state_changed.h>
#include <zmk/events/endpoint_changed.h>
#include <zmk/usb.h>
#include <zmk/endpoints.h>

#if IS_ENABLED(CONFIG_ZMK_USB_HOST_OS)
#  include <zmk/usb_host_os.h>
#  include <zmk/events/usb_host_os_changed.h>
#endif  // CONFIG_ZMK_USB_HOST_OS
#include <zmk/display/util_macros.h>
#include <zmk/display/widgets/usb_status_16x12.h>

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

enum icon { ICON_APPLE, ICON_WINDOWS, ICON_OPEN, ICON_CLOSE };

struct usb_status_state {
  struct zmk_endpoint_instance selected_endpoint;
  enum zmk_usb_conn_state conn_state;
#if IS_ENABLED(CONFIG_ZMK_USB_HOST_OS)
  enum usb_host_os usb_host_os;
#endif  // CONFIG_ZMK_USB_HOST_OS
};

LV_FONT_DECLARE(cozetta_icons_13);

static struct zmk_lv_usb_status_user_data user_data;
static bool initialized;

static struct usb_status_state get_state(const zmk_event_t *_eh) {
  return (struct usb_status_state){.selected_endpoint = zmk_endpoints_selected(),
                                   .conn_state = zmk_usb_get_conn_state()
#if IS_ENABLED(CONFIG_ZMK_USB_HOST_OS)
                                     ,
                                   .usb_host_os = zmk_usb_host_os_detected()
#endif  // CONFIG_ZMK_USB_HOST_OS
  };
}

static void usb_status_update_cb(struct usb_status_state state) {
#if IS_ENABLED(CONFIG_ZMK_WIDGET_ENDPOINT_INDICATOR)
  static bool is_usb;
  bool new_is_usb = state.selected_endpoint.transport == ZMK_TRANSPORT_USB;
  if (initialized || new_is_usb != is_usb) {
    is_usb = new_is_usb;
    lv_obj_t *container = lv_obj_get_parent(user_data.icon_label);
    lv_obj_set_style_bg_color(container, is_usb ? lv_color_black() : lv_color_white(),
                              LV_PART_MAIN);
    lv_obj_set_style_text_color(container, is_usb ? lv_color_white() : lv_color_black(),
                                LV_PART_MAIN);
  }
#endif

  static enum icon icon;
  enum icon new_icon;
  if (state.conn_state == ZMK_USB_CONN_HID) {
#if IS_ENABLED(CONFIG_ZMK_USB_HOST_OS)
    switch (state.usb_host_os) {
      case USB_HOST_OS_DARWIN:
        new_icon = ICON_APPLE;
        break;
      case USB_HOST_OS_UNKNOWN:
        new_icon = ICON_WINDOWS;
        break;
      default:
        new_icon = ICON_OPEN;
        break;
    }
#else
    new_icon = ICON_OPEN;
#endif  // CONFIG_ZMK_USB_HOST_OS
  } else {
    new_icon = ICON_CLOSE;
  }
  if (initialized || new_icon != icon) {
    icon = new_icon;
    switch (icon) {
      case ICON_APPLE:
        lv_label_set_text(user_data.icon_label, NF_FA_APPLE);
        break;
      case ICON_WINDOWS:
        lv_label_set_text(user_data.icon_label, NF_FA_WINDOWS);
        break;
      case ICON_OPEN:
        lv_label_set_text(user_data.icon_label, NF_FA_CHECK);
        break;
      case ICON_CLOSE:
        lv_label_set_text(user_data.icon_label, NF_FA_CLOSE);
        break;
    }
  }
  if (initialized) initialized = false;
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_usb_status, struct usb_status_state, usb_status_update_cb,
                            get_state)
ZMK_SUBSCRIPTION(widget_usb_status, zmk_usb_conn_state_changed);
ZMK_SUBSCRIPTION(widget_usb_status, zmk_endpoint_changed);

#if defined(CONFIG_ZMK_USB_HOST_OS)
ZMK_SUBSCRIPTION(widget_usb_status, zmk_usb_host_os_changed);
#endif  // CONFIG_ZMK_USB_HOST_OS

lv_obj_t *zmk_lv_usb_status_create(lv_obj_t *parent, lv_obj_t *(*container_default)(lv_obj_t *),
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
  // lv_label_set_text_static(title_label, NF_DEV_GIT_BRANCH);
  lv_label_set_text(title_label, NF_DEV_GIT_BRANCH);

  lv_obj_t *icon_label = lv_label_create(container);
  lv_obj_set_style_text_font(icon_label, &cozetta_icons_13, 0);
  lv_label_set_text(icon_label, " ");

  user_data.title_label = title_label;
  user_data.icon_label = icon_label;
  lv_obj_set_user_data(container, &user_data);

  widget_usb_status_init();
  initialized = true;
  return container;
}
