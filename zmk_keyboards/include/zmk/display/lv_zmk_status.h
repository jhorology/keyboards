#pragma once

#include <zephyr/sys/util.h>
#include <zmk/display/lv_zmk_event.h>

enum lv_zmk_ble_conn_state { BLE_NOT_PAIERD, BLE_DISCONNECTED, BLE_CONNECTED };
enum lv_zmk_usb_conn_state {
  USB_DISCONNECTED,
  USB_CONNECTED,
  USB_CONNECTED_MAC,
  USB_CONNECTED_WIN
};

struct lv_zmk_status {
#if IS_LV_ZMK_EVENT_ENABLED(battery_state)
  /* LV_ZMK_BATTERY_STATE_CHANGED  */
  uint8_t battery_level;
#endif

#if IS_LV_ZMK_EVENT_ENABLED(usb_conn_state)
  /* LV_ZMK_USB_CONN_STATE_CHANGED */
  bool battery_charging;
#endif

#if IS_LV_ZMK_EVENT_ENABLED(endpoint)
  /* LV_ZMK_ENDPOINT_CHANGED */
  bool ble_selected;
#endif

#if IS_LV_ZMK_EVENT_ENABLED(ble_active_profile)
  /* LV_ZMK_BLE_ACTIVE_PROFILE_CHANGED */
  uint8_t ble_profile_index;

  /* LV_ZMK_BLE_ACTIVE_PROFILE_CHANGED */
  enum lv_zmk_ble_conn_state ble_conn_state;

  /* TODO */
  uint8_t ble_rssi;
#endif

#if IS_LV_ZMK_EVENT_ENABLED(endpoint)
  /* LV_ZMK_ENDPOINT_CHANGED */
  bool usb_selected;
#endif

#if IS_LV_ZMK_EVENT_ENABLED(usb_conn_state)
  /* LV_ZMK_USB_CONN_STATE_CHANGED */
  enum lv_zmk_usb_conn_state usb_conn_state;
#endif

#if IS_LV_ZMK_EVENT_ENABLED(layer_state)
  /* LV_ZMK_LAYER_STATE_CHANGED */
  uint8_t layer_index;

  /* LV_ZMK_LAYER_STATE_CHANGED */
  const char *layer_name;
#endif

#if IS_LV_ZMK_EVENT_ENABLED(split_peripheral_status)
  /* LV_ZMK_SPLIT_PERIPHERAL_STATUS_CHANGED */
  bool peripheral_connected;

  /* TODO  */
  uint8_t peropheral_rssi;
#endif
};
