/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/kernel.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/battery.h>
#include <zmk/display.h>
#include <zmk/display/widgets/battery_status.h>
#include <zmk/usb.h>
#include <zmk/events/usb_conn_state_changed.h>
#include <zmk/event_manager.h>
#include <zmk/events/battery_state_changed.h>

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

struct battery_status_state {
  uint8_t level;
#if IS_ENABLED(CONFIG_USB_DEVICE_STACK)
  bool usb_present;
#endif
};

static void set_battery_symbol(lv_obj_t *spangroup, struct battery_status_state state) {
  char icon[4] = {};
  char perc[12] = {};

  uint8_t level = state.level;

#if IS_ENABLED(CONFIG_USB_DEVICE_STACK)
  if (state.usb_present) {
    strcpy(icon, LV_SYMBOL_CHARGE);
  } else {
    if (level > 95) {
      strcat(icon, LV_SYMBOL_BATTERY_FULL);
    } else if (level > 65) {
      strcat(icon, LV_SYMBOL_BATTERY_3);
    } else if (level > 35) {
      strcat(icon, LV_SYMBOL_BATTERY_2);
    } else if (level > 5) {
      strcat(icon, LV_SYMBOL_BATTERY_1);
    } else {
      strcat(icon, LV_SYMBOL_BATTERY_EMPTY);
    }
  }
#else
  if (level > 95) {
    strcat(icon, LV_SYMBOL_BATTERY_FULL);
  } else if (level > 65) {
    strcat(icon, LV_SYMBOL_BATTERY_3);
  } else if (level > 35) {
    strcat(icon, LV_SYMBOL_BATTERY_2);
  } else if (level > 5) {
    strcat(icon, LV_SYMBOL_BATTERY_1);
  } else {
    strcat(icon, LV_SYMBOL_BATTERY_EMPTY);
  }
#endif  // CONFIG_USB_DEVICE_STACK
  snprintf(perc, sizeof(perc), " %u", level);

  lv_span_t *span = lv_spangroup_get_child(spangroup, 0);
  lv_span_set_text(span, icon);

  span = lv_spangroup_get_child(spangroup, 1);
  lv_span_set_text(span, perc);

  lv_spangroup_refr_mode(spangroup);
}

void battery_status_update_cb(struct battery_status_state state) {
  struct zmk_widget_battery_status *widget;
  SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) { set_battery_symbol(widget->obj, state); }
}

static struct battery_status_state battery_status_get_state(const zmk_event_t *eh) {
  const struct zmk_battery_state_changed *ev = as_zmk_battery_state_changed(eh);

  return (struct battery_status_state){
    .level = (ev != NULL) ? ev->state_of_charge : zmk_battery_state_of_charge(),
#if IS_ENABLED(CONFIG_USB_DEVICE_STACK)
    .usb_present = zmk_usb_is_powered(),
#endif /* IS_ENABLED(CONFIG_USB_DEVICE_STACK) */
  };
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_battery_status, struct battery_status_state,
                            battery_status_update_cb, battery_status_get_state)

ZMK_SUBSCRIPTION(widget_battery_status, zmk_battery_state_changed);
#if IS_ENABLED(CONFIG_USB_DEVICE_STACK)
ZMK_SUBSCRIPTION(widget_battery_status, zmk_usb_conn_state_changed);
#endif /* IS_ENABLED(CONFIG_USB_DEVICE_STACK) */

int zmk_widget_battery_status_init(struct zmk_widget_battery_status *widget, lv_obj_t *parent) {
  widget->obj = lv_spangroup_create(parent);

  // icon
  lv_span_t *span = lv_spangroup_new_span(widget->obj);

  // percentage
  span = lv_spangroup_new_span(widget->obj);
  lv_style_set_text_font(&span->style, lv_theme_get_font_small(parent));

  sys_slist_append(&widgets, &widget->node);

  widget_battery_status_init();
  return 0;
}

lv_obj_t *zmk_widget_battery_status_obj(struct zmk_widget_battery_status *widget) {
  return widget->obj;
}
