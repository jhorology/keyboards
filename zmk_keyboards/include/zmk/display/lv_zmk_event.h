#pragma once

#include <lvgl.h>
#include <zephyr/sys/util_macro.h>

#define _LV_ZMK_EVENT_LIST                                                               \
  battery_state, usb_conn_state, endpoint, ble_active_profile, layer_state, usb_host_os, \
    split_peripheral_status

#define _LV_ZMK_ENABLED(e) _lv_zmk_##e##_enabled
#define _LV_ZMK_IDX(e) _lv_zmk_##e##_idx
#define _LV_ZMK_EVENT_IDENTITY(e) COND_CODE_1(_LV_ZMK_ENABLED(e), (e), (EMPTY))

#define _lv_zmk_battery_state_enabled IS_ENABLED(CONFIG_ZMK_BATTERY_REPORTING)
#define _lv_zmk_battery_state_idx 0

#define _lv_zmk_usb_conn_state_enabled IS_ENABLED(CONFIG_USB_DEVICE_STACK)
#define _lv_zmk_usb_conn_state_idx                                                   \
  COND_CODE_1(_lv_zmk_usb_conn_state_enabled, (UTIL_INC(_lv_zmk_battery_state_idx)), \
              (_lv_zmk_battery_state_idx))

#define _lv_zmk_endpoint_enabled \
  UTIL_OR(IS_EQ(IS_ENABLED(CONFIG_ZMK_SPLIT), 0), IS_ENABLED(CONFIG_ZMK_SPLIT_ROLE_CENTRAL))
#define _lv_zmk_endpoint_idx                                                    \
  COND_CODE_1(_lv_zmk_endpoint_enabled, (UTIL_INC(_lv_zmk_usb_conn_state_idx)), \
              (_lv_zmk_usb_conn_state_idx))

#define _lv_zmk_ble_active_profile_enabled                                                      \
  UTIL_AND(                                                                                     \
    UTIL_OR(IS_EQ(IS_ENABLED(CONFIG_ZMK_SPLIT), 0), IS_ENABLED(CONFIG_ZMK_SPLIT_ROLE_CENTRAL)), \
    IS_ENABLED(CONFIG_ZMK_BLE))
#define _lv_zmk_ble_active_profile_idx                                              \
  COND_CODE_1(_lv_zmk_ble_active_profile_enabled, (UTIL_INC(_lv_zmk_endpoint_idx)), \
              (_lv_zmk_endpoint_idx))

#define _lv_zmk_layer_state_enabled \
  UTIL_OR(IS_EQ(IS_ENABLED(CONFIG_ZMK_SPLIT), 0), IS_ENABLED(CONFIG_ZMK_SPLIT_ROLE_CENTRAL))
#define _lv_zmk_layer_state_idx                                                        \
  COND_CODE_1(_lv_zmk_layer_state_enabled, (UTIL_INC(_lv_zmk_ble_active_profile_idx)), \
              (_lv_zmk_ble_active_profile_idx))

#define _lv_zmk_usb_host_os_enabled                                                             \
  UTIL_AND(                                                                                     \
    UTIL_OR(IS_EQ(IS_ENABLED(CONFIG_ZMK_SPLIT), 0), IS_ENABLED(CONFIG_ZMK_SPLIT_ROLE_CENTRAL)), \
    IS_ENABLED(CONFIG_ZMK_USB_HOST_OS))
#define _lv_zmk_usb_host_os_idx                                                 \
  COND_CODE_1(_lv_zmk_usb_host_os_enabled, (UTIL_INC(_lv_zmk_layer_state_idx)), \
              (_lv_zmk_layer_state_idx))

#define _lv_zmk_split_peripheral_status_enabled \
  UTIL_AND(IS_ENABLED(CONFIG_ZMK_SPLIT), IS_EQ(IS_ENABLED(CONFIG_ZMK_SPLIT_ROLE_CENTRAL), 0))
#define _lv_zmk_split_peripheral_status_idx                                                 \
  COND_CODE_1(_lv_zmk_split_peripheral_status_enabled, (UTIL_INC(_lv_zmk_usb_host_os_idx)), \
              (_lv_zmk_usb_host_os_idx))

#define LV_ZMK_EVENT_LIST \
  LIST_DROP_EMPTY(FOR_EACH(_LV_ZMK_EVENT_IDENTITY, (, ), _LV_ZMK_EVENT_LIST))
#define LV_ZMK_NUM_EVENTS UTIL_INC(NUM_VA_ARGS_LESS_1(LV_ZMK_EVENT_LIST))
#define LV_ZMK_EVENT_IS_ENABLED(e) _LV_ZMK_ENABLED(e)
#define LV_ZMK_EVENT_CODE(e) lv_zmk_##e##_changed
#define LV_ZMK_EVENT_FOR_EACH(fn, sep) FOR_EACH(fn, sep, LV_ZMK_EVENT_LIST)
#define LV_ZMK_EVENT_FOR_EACH_IDX(fn, sep) FOR_EACH_IDX(fn, sep, LV_ZMK_EVENT_LIST)
#define LV_ZMK_EVENT_INTEREST(e) COND_CODE_1(_LV_ZMK_ENABLED(e), (BIT(_LV_ZMK_IDX(e))), (0))
#define LV_ZMK_EVENT_INTERESTS(...) FOR_EACH(LV_ZMK_EVENT_INTEREST, (|), __VA_ARGS__)

typedef uint32_t lv_zmk_event_interests;

#define _LV_ZMK_EVENT_EXTERN(e) extern lv_event_code_t LV_ZMK_EVENT_CODE(e);
LV_ZMK_EVENT_FOR_EACH(_LV_ZMK_EVENT_EXTERN, ());
