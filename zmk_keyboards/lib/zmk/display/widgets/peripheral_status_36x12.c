/*
 * Copyright (c) 2024 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <zmk/display/lv_zmk_event.h>
#include <zmk/display/lv_zmk_status.h>
#include <zmk/display/status_presenter.h>
#include <zmk/display/util_macros.h>
#include <zmk/display/widgets/peripheral_status_36x12.h>
#include <zephyr/logging/log.h>
#include "core/lv_event.h"
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

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

static void icon_cb(lv_event_t *event) {
  lv_obj_t *icon_label = lv_event_get_current_target(event);
  struct lv_zmk_status *state = lv_event_get_param(event);

  lv_label_set_text(icon_label,
                    state->peripheral_connected ? NF_FA_BLUETOOTH : NF_MDI_BLUETOOTH_OFF);
}

lv_obj_t *lv_peripheral_status_create(lv_obj_t *parent, lv_obj_t *(*container_default)(lv_obj_t *),
                                      lv_align_t align) {
  lv_obj_t *container =
    container_default != NULL ? container_default(parent) : lv_obj_create(parent);
  lv_obj_set_size(container, WIDTH, HEIGHT);
  lv_obj_set_flex_flow(container, LV_FLEX_FLOW_ROW);
  ALIGN_FLEX_FLOW_ROW_COMPOSITE_WIDGET(container, align, LV_FLEX_ALIGN_END);

  lv_obj_t *icon_label = lv_label_create(container);
  lv_obj_set_style_text_font(icon_label, &cozetta_icons_13, LV_PART_MAIN);
  lv_obj_set_width(icon_label, 6);
  lv_obj_add_event_cb(icon_label, icon_cb, LV_ZMK_EVENT_CODE(split_peripheral_status), NULL);
  /* TODO auto register  */
  zmk_status_presenter_register(icon_label, LV_ZMK_EVENT_CODE(split_peripheral_status));

  lv_obj_t *rssi_label = lv_label_create(container);
  lv_label_set_long_mode(rssi_label, LV_LABEL_LONG_CLIP);
  lv_obj_set_style_text_font(rssi_label, &pixel_mplus_10, LV_PART_MAIN);
  // TODO RSSI
  lv_label_set_text(rssi_label, "-70dBm");

  return container;
}
