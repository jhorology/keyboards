/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/kernel.h>

#include <zephyr/logging/log.h>
#include "extra/widgets/span/lv_span.h"
#include "font/lv_symbol_def.h"
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/display.h>
#include <zmk/display/widgets/output_status.h>
#include <zmk/event_manager.h>
#include <zmk/events/ble_active_profile_changed.h>
#include <zmk/events/endpoint_changed.h>
#include <zmk/events/usb_conn_state_changed.h>
#include <zmk/usb.h>
#include <zmk/ble.h>
#include <zmk/endpoints.h>

#if IS_ENABLED(CONFIG_ZMK_USB_HOST_OS)
#  include <zmk/usb_host_os.h>
#  include <zmk/events/usb_host_os_changed.h>
#endif  // CONFIG_ZMK_USB_HOST_OS

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

struct output_status_state {
  struct zmk_endpoint_instance selected_endpoint;
  bool active_profile_connected;
  bool active_profile_bonded;
#if IS_ENABLED(CONFIG_ZMK_USB_HOST_OS)
  enum usb_host_os usb_host_os;
#endif  // CONFIG_ZMK_USB_HOST_OS
};

LV_FONT_DECLARE(pixel_mplus_bold_ascii_10);
LV_FONT_DECLARE(cozetta_icons_13);

/* 0x0e615  nf-seti-config */
#define NF_SETI_CONFIG "\xEE\x98\x95"
/* 0x0f00c  nf-fa-check */
#define NF_FA_CHECK "\xEF\x80\x8C"
/* 0x0f00d  nf-fa-close nf-fa-times nf-fa-xmark */
#define NF_FA_CLOSE "\xEF\x80\x8D"
/* 0x0f179  nf-fa-apple */
#define NF_FA_APPLE "\xEF\x85\xB9"
/* 0x0f17a  nf-fa-windows */
#define NF_FA_WINDOWS "\xEF\x85\xBA"
/* 0x0f293  nf-fa-bluetooth */
#define NF_FA_BLUETOOTH "\xEF\x8A\x93"
/* 0x0e725  nf-dev-git_branch */
#define NF_DEV_GIT_BRANCH "\xEE\x9C\xA5"
/* 0x0f5b1=>0xf00b2 =>󰂲 nf-md-bluetooth_off */
#define NF_MDI_BLUETOOTH_OFF "\xEF\x96\xB1"
#define NF_MD_BLUETOOTH_OFF "\xF3\xB0\x82\xb2"
/* 0x0f5b2=>0xf00b3 =>󰂳 nf-md-bluetooth_settings */
#define NF_MDI_BLUETOOTH_SETTING "\xEF\x96\xB2"
#define NF_MD_BLUETOOTH_OFF "\xF3\xB0\x82\xb3"

static struct output_status_state get_state(const zmk_event_t *_eh) {
  return (struct output_status_state){
    .selected_endpoint = zmk_endpoints_selected(),
    .active_profile_connected = zmk_ble_active_profile_is_connected(),
    .active_profile_bonded = !zmk_ble_active_profile_is_open()
#if IS_ENABLED(CONFIG_ZMK_USB_HOST_OS)
      ,
    .usb_host_os = zmk_usb_host_os_detected()
#endif  // CONFIG_ZMK_USB_HOST_OS
  };
  ;
}

static void set_status_symbol(lv_obj_t *spangroup, struct output_status_state state) {
  char span_0_text[10] = {};
  char span_1_icon[10] = {};
  char span_2_text[10] = {};
  char span_3_icon[10] = {};

  switch (state.selected_endpoint.transport) {
    case ZMK_TRANSPORT_USB:
      strcat(span_0_text, "USB ");
      if (zmk_usb_get_conn_state() == ZMK_USB_CONN_HID) {
#if IS_ENABLED(CONFIG_ZMK_USB_HOST_OS)
        switch (zmk_usb_host_os_detected()) {
          case USB_HOST_OS_DARWIN:
            strcat(span_1_icon, NF_FA_APPLE);
            break;
          case USB_HOST_OS_UNKNOWN:
            strcat(span_1_icon, NF_FA_WINDOWS);
            break;
          default:
            strcat(span_0_text, "--");
            break;
        }
#else
        strcat(span_3_icon, NF_FA_CHECK);
#endif  // CONFIG_ZMK_USB_HOST_OS
      } else {
        strcat(span_3_icon, NF_FA_CLOSE);
      }
      break;
    case ZMK_TRANSPORT_BLE:
      if (state.active_profile_bonded) {
        if (state.active_profile_connected) {
          strcat(span_1_icon, NF_FA_BLUETOOTH);
        } else {
          strcat(span_1_icon, NF_MDI_BLUETOOTH_OFF);
        }
      } else {
        strcat(span_1_icon, NF_MDI_BLUETOOTH_SETTING);
      }
      snprintf(span_2_text, sizeof(span_2_text), "%i",
               state.selected_endpoint.ble.profile_index + 1);
      break;
  }
  lv_span_t *span = lv_spangroup_get_child(spangroup, 0);
  lv_span_set_text(span, span_0_text);

  span = lv_spangroup_get_child(spangroup, 1);
  lv_span_set_text(span, span_1_icon);

  span = lv_spangroup_get_child(spangroup, 2);
  lv_span_set_text(span, span_2_text);

  span = lv_spangroup_get_child(spangroup, 3);
  lv_span_set_text(span, span_3_icon);

  lv_spangroup_refr_mode(spangroup);
}

static void output_status_update_cb(struct output_status_state state) {
  struct zmk_widget_output_status *widget;
  SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) { set_status_symbol(widget->obj, state); }
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_output_status, struct output_status_state,
                            output_status_update_cb, get_state)
ZMK_SUBSCRIPTION(widget_output_status, zmk_endpoint_changed);
// We don't get an endpoint changed event when the active profile connects/disconnects
// but there wasn't another endpoint to switch from/to, so update on BLE events too.
#if defined(CONFIG_ZMK_BLE)
ZMK_SUBSCRIPTION(widget_output_status, zmk_ble_active_profile_changed);
#endif  // CONFIG_ZMK_BLE
#if defined(CONFIG_ZMK_USB)
ZMK_SUBSCRIPTION(widget_output_status, zmk_usb_conn_state_changed);
#endif

#if defined(CONFIG_ZMK_USB_HOST_OS)
ZMK_SUBSCRIPTION(widget_output_status, zmk_usb_host_os_changed);
#endif  // CONFIG_ZMK_USB_HOST_OS

int zmk_widget_output_status_init(struct zmk_widget_output_status *widget, lv_obj_t *parent) {
  widget->obj = lv_spangroup_create(parent);

  // span_0_text
  lv_span_t *span = lv_spangroup_new_span(widget->obj);
  lv_style_set_text_font(&span->style, &pixel_mplus_bold_ascii_10);

  // span_1_icon
  span = lv_spangroup_new_span(widget->obj);
  lv_style_set_text_font(&span->style, &cozetta_icons_13);

  // span_2_text
  span = lv_spangroup_new_span(widget->obj);
  lv_style_set_text_font(&span->style, &pixel_mplus_bold_ascii_10);

  // span_3_icon
  span = lv_spangroup_new_span(widget->obj);
  lv_style_set_text_font(&span->style, &cozetta_icons_13);

  sys_slist_append(&widgets, &widget->node);

  widget_output_status_init();
  return 0;
}

lv_obj_t *zmk_widget_output_status_obj(struct zmk_widget_output_status *widget) {
  return widget->obj;
}
