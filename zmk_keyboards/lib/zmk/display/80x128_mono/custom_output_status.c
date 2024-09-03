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
  char transport_icon_text[4] = {};
  char transport_desc_text[13] = {};
  char ble_status_text[4] = {};

  switch (state.selected_endpoint.transport) {
    case ZMK_TRANSPORT_USB:
      strcat(transport_icon_text, LV_SYMBOL_USB);
#if IS_ENABLED(CONFIG_ZMK_USB_HOST_OS)
      switch (zmk_usb_host_os_detected()) {
        case USB_HOST_OS_DARWIN:
          strcat(transport_desc_text, "Mac");
          break;
        case USB_HOST_OS_UNKNOWN:
          strcat(transport_desc_text, "Win");
          break;
        default:
          strcat(transport_desc_text, "---");
          break;
      }
#endif  // CONFIG_ZMK_USB_HOST_OS
      break;
    case ZMK_TRANSPORT_BLE:
      strcat(transport_icon_text, LV_SYMBOL_WIFI);
      snprintf(transport_desc_text, sizeof(transport_desc_text), "%i ",
               state.selected_endpoint.ble.profile_index + 1);
      if (state.active_profile_bonded) {
        if (state.active_profile_connected) {
          strcat(ble_status_text, LV_SYMBOL_OK);
        } else {
          strcat(ble_status_text, LV_SYMBOL_CLOSE);
        }
      } else {
        strcat(ble_status_text, LV_SYMBOL_SETTINGS);
      }
      break;
  }
  lv_span_t *span = lv_spangroup_get_child(spangroup, 0);
  lv_span_set_text(span, transport_icon_text);

  span = lv_spangroup_get_child(spangroup, 1);
  lv_span_set_text(span, transport_desc_text);

  span = lv_spangroup_get_child(spangroup, 2);
  lv_span_set_text(span, ble_status_text);

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

#if defined(CONFIG_ZMK_USB_HOST_OS)
ZMK_SUBSCRIPTION(widget_output_status, zmk_usb_host_os_changed);
#endif  // CONFIG_ZMK_USB_HOST_OS

int zmk_widget_output_status_init(struct zmk_widget_output_status *widget, lv_obj_t *parent) {
  widget->obj = lv_spangroup_create(parent);

  // transport icon
  lv_span_t *span = lv_spangroup_new_span(widget->obj);

  // transport desc text
  span = lv_spangroup_new_span(widget->obj);
  lv_style_set_text_font(&span->style, lv_theme_get_font_small(parent));

  // ble status icon
  span = lv_spangroup_new_span(widget->obj);

  sys_slist_append(&widgets, &widget->node);

  widget_output_status_init();
  return 0;
}

lv_obj_t *zmk_widget_output_status_obj(struct zmk_widget_output_status *widget) {
  return widget->obj;
}
