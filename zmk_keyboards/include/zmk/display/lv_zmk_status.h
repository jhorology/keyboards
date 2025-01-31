#pragma once

#include <zmk/display/lv_zmk_event.h>

enum lv_zmk_ble_conn_state { BLE_NOT_PAIERD, BLE_DISCONNECTED, BLE_CONNECTED };
enum lv_zmk_usb_conn_state {
  USB_DISCONNECTED,
  USB_CONNECTED,
  USB_CONNECTED_MAC,
  USB_CONNECTED_WIN
};

enum lv_zmk_indicator {
  INDICATOR_NUM_LOCK,
  INDICATOR_CAPS_LOCK,
  INDICATOR_SCROLL_LOCK,
  INDICATOR_COMPOSE,
  INDICATOR_KANA_LOCK
};

struct lv_zmk_status {
#if LV_ZMK_EVENT_IS_ENABLED(battery_state)
  uint8_t battery_level;
#endif

#if LV_ZMK_EVENT_IS_ENABLED(usb_conn_state)
  bool battery_charging;
#endif

#if LV_ZMK_EVENT_IS_ENABLED(endpoint)
  bool ble_selected;
#endif

#if LV_ZMK_EVENT_IS_ENABLED(ble_active_profile)
  uint8_t ble_profile_index;

  /* LV_ZMK_BLE_ACTIVE_PROFILE_CHANGED */
  enum lv_zmk_ble_conn_state ble_conn_state;

  /* TODO */
  uint8_t ble_rssi;
#endif

#if LV_ZMK_EVENT_IS_ENABLED(endpoint)
  bool usb_selected;
#endif

#if LV_ZMK_EVENT_IS_ENABLED(usb_conn_state)
  enum lv_zmk_usb_conn_state usb_conn_state;
#endif

#if LV_ZMK_EVENT_IS_ENABLED(layer_state)
  uint8_t layer_index;
  uint16_t layers_state;
  const char *layer_name;
#endif

#if LV_ZMK_EVENT_IS_ENABLED(split_peripheral_status)
  bool peripheral_connected;

  /* TODO */
  uint8_t peropheral_rssi;
#endif

#if LV_ZMK_EVENT_IS_ENABLED(hid_indicators)
  uint8_t indicators_state;
#endif
};
