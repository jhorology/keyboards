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

LV_FONT_DECLARE(pixel_mplus_bold_ascii_10);
LV_FONT_DECLARE(cozetta_icons_13);

/* 0x0f0e7  nf-fa-bolt nf-fa-flash */
#define NF_FA_BOLT "\xEF\x83\xA7"
/* 0x0f578=>0xf0079 =>󰁹 nf-md-battery */
#define NF_MDI_BATTERY "\xEF\x95\xB8"
/* 0x0f579=>0xf007a =>󰁺 nf-md-battery_10 */
#define NF_MDI_BATTERY_10 "\xEF\x95\xB9"
/* 0x0f57a=>0xf007b =>󰁻 nf-md-battery_20 */
#define NF_MDI_BATTERY_20 "\xEF\x95\xBA"
/* 0x0f57b=>0xf007c =>󰁼 nf-md-battery_30 */
#define NF_MDI_BATTERY_30 "\xEF\x95\xBB"
/* 0x0f57c=>0xf007d =>󰁽 nf-md-battery_40 */
#define NF_MDI_BATTERY_40 "\xEF\x95\xBC"
/* 0x0f57d=>0xf007e =>󰁾 nf-md-battery_50 */
#define NF_MDI_BATTERY_50 "\xEF\x95\xBD"
/* 0x0f57e=>0xf007f =>󰁿 nf-md-battery_60 */
#define NF_MDI_BATTERY_60 "\xEF\x95\xBE"
/* 0x0f57f=>0xf0080 =>󰂀 nf-md-battery_70 */
#define NF_MDI_BATTERY_70 "\xEF\x95\xBF"
/* 0x0f580=>0xf0081 =>󰂁 nf-md-battery_80 */
#define NF_MDI_BATTERY_80 "\xEF\x96\x80"
/* 0x0f581=>0xf0082 =>󰂂 nf-md-battery_90 */
#define NF_MDI_BATTERY_90 "\xEf\x96\x81"

/* 0x0f584=>0xf0085 =>󰂅 nf-md-battery_charging_100 */
#define NF_MDI_BATTERY_CHARGING_100 "\xEF\x96\x84"
/* 0x0f585=>0xf0086 =>󰂆 nf-md-battery_charging_20 */
#define NF_MDI_BATTERY_CHARGING_20 "\xEF\x96\x85"
/* 0x0f586=>0xf0087 =>󰂇 nf-md-battery_charging_30 */
#define NF_MDI_BATTERY_CHARGING_30 "\xEF\x96\x86"
/* 0x0f587=>0xf0088 =>󰂈 nf-md-battery_charging_40 */
#define NF_MDI_BATTERY_CHARGING_40 "\xEF\x96\x87"
/* 0x0f588=>0xf0089 =>󰂉 nf-md-battery_charging_60 */
#define NF_MDI_BATTERY_CHARGING_60 "\xEF\x96\x88"
/* 0x0f589=>0xf008a =>󰂊 nf-md-battery_charging_80 */
#define NF_MDI_BATTERY_CHARGING_80 "\xEF\x96\x89"
/* 0x0f58a=>0xf008b =>󰂋 nf-md-battery_charging_90 */
#define NF_MDI_BATTERY_CHARGING_90 "\xEF\x96\x8A"

static void set_battery_symbol(lv_obj_t *spangroup, struct battery_status_state state) {
  char icon[7] = {};
  char perc[6] = {};

  uint8_t level = state.level;

  if (level > 95) {
    strcat(icon, NF_MDI_BATTERY);
  } else if (level > 85) {
    strcat(icon, NF_MDI_BATTERY_90);
  } else if (level > 75) {
    strcat(icon, NF_MDI_BATTERY_80);
  } else if (level > 65) {
    strcat(icon, NF_MDI_BATTERY_70);
  } else if (level > 55) {
    strcat(icon, NF_MDI_BATTERY_60);
  } else if (level > 45) {
    strcat(icon, NF_MDI_BATTERY_50);
  } else if (level > 35) {
    strcat(icon, NF_MDI_BATTERY_40);
  } else if (level > 25) {
    strcat(icon, NF_MDI_BATTERY_30);
  } else if (level > 15) {
    strcat(icon, NF_MDI_BATTERY_20);
  } else {
    strcat(icon, NF_MDI_BATTERY_10);
  }
#if IS_ENABLED(CONFIG_USB_DEVICE_STACK)
  if (state.usb_present) {
    strcat(icon, NF_FA_BOLT);
  }
#endif  // CONFIG_USB_DEVICE_STACK
  snprintf(perc, sizeof(perc), "%u%%", level);

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
  lv_style_set_text_font(&span->style, &cozetta_icons_13);

  // percentage
  span = lv_spangroup_new_span(widget->obj);
  lv_style_set_text_font(&span->style, &pixel_mplus_bold_ascii_10);

  sys_slist_append(&widgets, &widget->node);

  widget_battery_status_init();
  return 0;
}

lv_obj_t *zmk_widget_battery_status_obj(struct zmk_widget_battery_status *widget) {
  return widget->obj;
}
