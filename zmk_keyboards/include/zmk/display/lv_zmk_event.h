#pragma once

#include <lvgl.h>
#include <zephyr/sys/util_macro.h>

#define _LV_ZMK_EVENT_LIST                                                               \
  battery_state, usb_conn_state, endpoint, ble_active_profile, layer_state, usb_host_os, \
    split_peripheral_status

#if IS_ENABLED(CONFIG_ZMK_BATTERY_REPORTING)
#  define _lv_zmk_battery_state_enabled 1
#endif
#if IS_ENABLED(CONFIG_USB_DEVICE_STACK)
#  define _lv_zmk_usb_conn_state_enabled 1
#endif
#if !IS_ENABLED(CONFIG_ZMK_SPLIT) || IS_ENABLED(CONFIG_ZMK_SPLIT_ROLE_CENTRAL)
#  define _lv_zmk_endpoint_enabled 1
#  if IS_ENABLED(CONFIG_ZMK_BLE)
#    define _lv_zmk_ble_active_profile_enabled 1
#  endif
#  define _lv_zmk_layer_state_enabled 1
#  if IS_ENABLED(CONFIG_ZMK_USB_HOST_OS)
#    define _lv_zmk_usb_host_os_enabled 1
#  endif
#endif
#if IS_ENABLED(CONFIG_ZMK_SPLIT) && !IS_ENABLED(CONFIG_ZMK_SPLIT_ROLE_CENTRAL)
#  define _lv_zmk_split_peripheral_status_enabled 1
#endif

#define _LV_ZMK_EVENT_FN(e, fn) COND_CODE_1(_lv_zmk_##e##_enabled, (fn(e)), ())
#define _LV_ZMK_EVENT_EXTERN(e) extern lv_event_code_t LV_ZMK_EVENT_CODE(e);

#define LV_ZMK_EVENT_CODE(e) lv_zmk_##e##_changed
#define FOR_EACH_LV_ZMK_EVENT(fn, sep) \
  FOR_EACH_FIXED_ARG(_LV_ZMK_EVENT_FN, sep, fn, _LV_ZMK_EVENT_LIST)
#define IS_LV_ZMK_EVENT_ENABLED(e) IS_ENABLED(_lv_zmk_##e##_enabled)

FOR_EACH_LV_ZMK_EVENT(_LV_ZMK_EVENT_EXTERN, ());
