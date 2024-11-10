/*
 * Copyright (c) 2022 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/kernel.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/display.h>
#include <zmk/display/widgets/peripheral_status.h>
#include <zmk/event_manager.h>
#include <zmk/split/bluetooth/peripheral.h>
#include <zmk/events/split_peripheral_status_changed.h>

LV_FONT_DECLARE(cozetta_icons_13);

/* 0x0f293  nf-fa-bluetooth */
#define NF_FA_BLUETOOTH "\xEF\x8A\x93"

/* 0x0f5b1=>0xf00b2 =>󰂲 nf-md-bluetooth_off */
#define NF_MDI_BLUETOOTH_OFF "\xEF\x96\xB1"
#define NF_MD_BLUETOOTH_OFF "\xF3\xB0\x82\xb2"

/* 0x0f836=>0xf0337 =>󰌷 nf-md-link */
#define NF_MDI_LINK "\xEF\xA0\xB6"
#define NF_MD_LINK "\xF3\xB0\x8C\xB7"

/* 0x0f837=>0xf0338 =>󰌸 nf-md-link_off */
#define NF_MDI_LINK_OFF "\xEF\xA0\xB7"
#define NF_MD_LINK_OFF "\xF3\xB0\x8C\xB8"

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

struct peripheral_status_state {
  bool connected;
};

static struct peripheral_status_state get_state(const zmk_event_t *_eh) {
  return (struct peripheral_status_state){.connected = zmk_split_bt_peripheral_is_connected()};
}

static void set_status_symbol(lv_obj_t *label, struct peripheral_status_state state) {
  lv_label_set_text_static(label, state.connected ? NF_FA_BLUETOOTH : NF_MDI_BLUETOOTH_OFF);

  // TODO RSSI
}

static void output_status_update_cb(struct peripheral_status_state state) {
  struct zmk_widget_peripheral_status *widget;
  SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) { set_status_symbol(widget->obj, state); }
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_peripheral_status, struct peripheral_status_state,
                            output_status_update_cb, get_state)
ZMK_SUBSCRIPTION(widget_peripheral_status, zmk_split_peripheral_status_changed);

int zmk_widget_peripheral_status_init(struct zmk_widget_peripheral_status *widget,
                                      lv_obj_t *parent) {
  widget->obj = lv_label_create(parent);
  lv_obj_set_style_text_font(widget->obj, &cozetta_icons_13, 0);

  sys_slist_append(&widgets, &widget->node);

  widget_peripheral_status_init();
  return 0;
}

lv_obj_t *zmk_widget_peripheral_status_obj(struct zmk_widget_peripheral_status *widget) {
  return widget->obj;
}
