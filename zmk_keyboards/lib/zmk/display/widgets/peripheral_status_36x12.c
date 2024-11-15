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
#include <zmk/split/bluetooth/peripheral.h>
#include <zmk/events/split_peripheral_status_changed.h>

#include <zmk/display/util_macros.h>
#include <zmk/display/widgets/peripheral_status_36x12.h>

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

#define WIDTH 36
#define HEIGHT 12

LV_FONT_DECLARE(cozetta_icons_13);
LV_FONT_DECLARE(pixel_mplus_10);

struct peripheral_status_state {
  bool connected;
};

static struct zmk_lv_peripheral_status_user_data user_data;

static struct peripheral_status_state get_state(const zmk_event_t *_eh) {
  return (struct peripheral_status_state){.connected = zmk_split_bt_peripheral_is_connected()};
}

static void output_status_update_cb(struct peripheral_status_state state) {
  lv_label_set_text(user_data.icon_label, state.connected ? NF_FA_BLUETOOTH : NF_MDI_BLUETOOTH_OFF);

  // TODO RSSI
  lv_label_set_text(user_data.desc_label, "-70dBm");
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_peripheral_status, struct peripheral_status_state,
                            output_status_update_cb, get_state)
ZMK_SUBSCRIPTION(widget_peripheral_status, zmk_split_peripheral_status_changed);

lv_obj_t *zmk_lv_peripheral_status_create(lv_obj_t *parent,
                                          lv_obj_t *(*container_default)(lv_obj_t *),
                                          lv_align_t align) {
  lv_obj_t *container =
    container_default != NULL ? container_default(parent) : lv_obj_create(parent);
  lv_obj_set_size(container, WIDTH, HEIGHT);
  lv_obj_set_flex_flow(container, LV_FLEX_FLOW_ROW);
  ALIGN_FLEX_FLOW_ROW_COMPOSITE_WIDGET(container, align, LV_FLEX_ALIGN_END);

  lv_obj_t *icon_label = lv_label_create(container);
  lv_obj_set_style_text_font(icon_label, &cozetta_icons_13, LV_PART_MAIN);
  lv_obj_set_width(icon_label, 6);

  lv_obj_t *desc_label = lv_label_create(container);
  lv_label_set_long_mode(desc_label, LV_LABEL_LONG_CLIP);
  lv_obj_set_style_text_font(desc_label, &pixel_mplus_10, LV_PART_MAIN);

  user_data.icon_label = icon_label;
  user_data.desc_label = desc_label;
  lv_obj_set_user_data(container, &user_data);

  widget_peripheral_status_init();

  return container;
}
