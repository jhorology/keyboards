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
#include <zmk/usb.h>
#include <zmk/events/usb_conn_state_changed.h>
#include <zmk/event_manager.h>
#include <zmk/events/battery_state_changed.h>

#include <zmk/display/util_macros.h>
#include <zmk/display/widgets/battery_status_32x12.h>

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

#define WIDTH 32
#define HEIGHT 12

LV_FONT_DECLARE(pixel_mplus_10);
LV_FONT_DECLARE(cozetta_icons_13);

struct battery_status_state {
  uint8_t level;
#if IS_ENABLED(CONFIG_USB_DEVICE_STACK)
  bool usb_present;
#endif
};

static struct zmk_lv_battery_status_user_data user_data;

#define SELECT_BATTERY_ICON(level)  \
  (level > 95   ? NF_MDI_BATTERY    \
   : level > 85 ? NF_MDI_BATTERY_90 \
   : level > 75 ? NF_MDI_BATTERY_80 \
   : level > 65 ? NF_MDI_BATTERY_70 \
   : level > 55 ? NF_MDI_BATTERY_60 \
   : level > 45 ? NF_MDI_BATTERY_50 \
   : level > 35 ? NF_MDI_BATTERY_40 \
   : level > 25 ? NF_MDI_BATTERY_30 \
   : level > 15 ? NF_MDI_BATTERY_20 \
                : NF_MDI_BATTERY_10)

static void battery_status_update_cb(struct battery_status_state state) {
#if IS_ENABLED(CONFIG_USB_DEVICE_STACK)
  lv_label_set_text_fmt(user_data.icons_label, "%s%s", SELECT_BATTERY_ICON(state.level),
                        state.usb_present ? NF_FA_BOLT : "");
#else
  lv_label_set_text_fmt(user_data.icons_label, "%s", SELECT_BATTERY_ICON(state.level));
#endif
  lv_label_set_text_fmt(user_data.perc_label, "%u%%", state.level);
}

static struct battery_status_state battery_status_get_state(const zmk_event_t *eh) {
  const struct zmk_battery_state_changed *ev = as_zmk_battery_state_changed(eh);

  return (struct battery_status_state){
    .level = (ev != NULL) ? ev->state_of_charge : zmk_battery_state_of_charge(),
#if IS_ENABLED(CONFIG_USB_DEVICE_STACK)
    .usb_present = zmk_usb_is_powered(),
#endif
  };
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_battery_status, struct battery_status_state,
                            battery_status_update_cb, battery_status_get_state)

ZMK_SUBSCRIPTION(widget_battery_status, zmk_battery_state_changed);
#if IS_ENABLED(CONFIG_USB_DEVICE_STACK)
ZMK_SUBSCRIPTION(widget_battery_status, zmk_usb_conn_state_changed);
#endif

lv_obj_t *zmk_lv_battery_status_create(lv_obj_t *parent, lv_obj_t *(*container_default)(lv_obj_t *),
                                       lv_align_t align) {
  lv_obj_t *container =
    container_default != NULL ? container_default(parent) : lv_obj_create(parent);
  lv_obj_set_size(container, WIDTH, HEIGHT);
  lv_obj_set_flex_flow(container, LV_FLEX_FLOW_ROW);
  lv_obj_set_style_pad_gap(container, 1, LV_PART_MAIN);
  ALIGN_FLEX_FLOW_ROW_COMPOSITE_WIDGET(container, align, LV_FLEX_ALIGN_END);

  lv_obj_t *icons_label = lv_label_create(container);
  lv_obj_set_style_text_font(icons_label, &cozetta_icons_13, LV_PART_MAIN);

  lv_obj_t *perc_label = lv_label_create(container);
  lv_obj_set_style_text_font(perc_label, &pixel_mplus_10, LV_PART_MAIN);

  user_data.icons_label = icons_label;
  user_data.perc_label = perc_label;
  lv_obj_set_user_data(container, &user_data);

  widget_battery_status_init();

  return container;
}
