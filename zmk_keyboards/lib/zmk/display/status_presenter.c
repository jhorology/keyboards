
#include <lvgl.h>
#include <zmk/event_manager.h>
#include <zmk/display.h>
#include <zmk/display/lv_zmk_event.h>
#include <zmk/display/lv_zmk_status.h>
#include <zmk/display/status_presenter.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

struct lv_obj_slist {
  lv_obj_t *first;
  lv_obj_t *end;
};

#define ZMK_DISPLAY_STATE_UPDATE(e) display_##e##_update_cb
#define ZMK_DISPLAY_STATE_UPDATE_FUNC(e, state_type, code) \
  static void ZMK_DISPLAY_STATE_UPDATE(e)(state_type state) { __DEBRACKET code; }
#define ZMK_EVENT_TYPE(e) zmk_##e##_changed
#define ZMK_EVENT_AS(e) as_zmk_##e##_changed
#define ZMK_DISPLAY_STATE_GET(e) display_##e##_state_get
#define ZMK_DISPLAY_STATE_GET_FUNC(e, state_type, code)               \
  static state_type ZMK_DISPLAY_STATE_GET(e)(const zmk_event_t *eh) { \
    const struct ZMK_EVENT_TYPE(e) *ev = ZMK_EVENT_AS(e)(eh);         \
    __DEBRACKET code;                                                 \
  }

#define ZMK_EVENT_PRESENTER(e, state_type, update_code, get_code)         \
  ZMK_DISPLAY_STATE_UPDATE_FUNC(e, state_type, update_code)               \
  ZMK_DISPLAY_STATE_GET_FUNC(e, state_type, get_code)                     \
  ZMK_DISPLAY_WIDGET_LISTENER(e, state_type, ZMK_DISPLAY_STATE_UPDATE(e), \
                              ZMK_DISPLAY_STATE_GET(e))                   \
  ZMK_SUBSCRIPTION(e, zmk_##e##_changed)

#define LV_ZMK_EVENT_TARGETS(e) e##_targets
#define _LV_ZMK_EVENT_DECLARE(e)        \
  lv_event_code_t LV_ZMK_EVENT_CODE(e); \
  static struct lv_obj_slist LV_ZMK_EVENT_TARGETS(e) = {0};

FOR_EACH_LV_ZMK_EVENT(_LV_ZMK_EVENT_DECLARE, ())

static lv_obj_t *status_screen;
static struct lv_zmk_status zmk_status;

static void send_event(lv_event_code_t event_code);

// battery_state_changed
// ------------------------------------------------------------
#if IS_LV_ZMK_EVENT_ENABLED(battery_state)
#  include <zmk/battery.h>
#  include <zmk/events/battery_state_changed.h>
ZMK_EVENT_PRESENTER(battery_state, uint8_t,
                    (zmk_status.battery_level = state;
                     send_event(LV_ZMK_EVENT_CODE(battery_state))),
                    (return (ev != NULL) ? ev->state_of_charge : zmk_battery_state_of_charge()));
#endif

// usb_conn_state_changed
// ------------------------------------------------------------
#if IS_LV_ZMK_EVENT_ENABLED(usb_conn_state)
#  include <zmk/usb.h>
#  include <zmk/events/usb_conn_state_changed.h>
ZMK_EVENT_PRESENTER(usb_conn_state, enum zmk_usb_conn_state,
                    (zmk_status.battery_charging = state != ZMK_USB_CONN_NONE;
                     zmk_status.usb_conn_state = state == ZMK_USB_CONN_HID ? USB_CONNECTED
                                                                           : USB_DISCONNECTED;
                     send_event(LV_ZMK_EVENT_CODE(usb_conn_state))),
                    (return (ev != NULL) ? ev->conn_state : zmk_usb_get_conn_state()));
#endif

// endpoint_changed
// ------------------------------------------------------------
#if IS_LV_ZMK_EVENT_ENABLED(endpoint)
#  include <zmk/endpoints_types.h>
#  include <zmk/endpoints.h>
#  include <zmk/events/endpoint_changed.h>
ZMK_EVENT_PRESENTER(endpoint, struct zmk_endpoint_instance,
                    (zmk_status.usb_selected = state.transport == ZMK_TRANSPORT_USB;
                     zmk_status.ble_selected = state.transport == ZMK_TRANSPORT_BLE;
                     send_event(LV_ZMK_EVENT_CODE(endpoint))),
                    (return (ev != NULL) ? ev->endpoint : zmk_endpoints_selected()));

#endif

// ble_active_profile_changed
// ------------------------------------------------------------
#if IS_LV_ZMK_EVENT_ENABLED(ble_active_profile)
#  include <zmk/ble.h>
#  include <zmk/events/ble_active_profile_changed.h>

struct ble_active_profile {
  uint8_t index;
  enum lv_zmk_ble_conn_state conn_state;
};

ZMK_EVENT_PRESENTER(ble_active_profile, struct ble_active_profile,
                    (zmk_status.ble_profile_index = state.index + 1;
                     zmk_status.ble_conn_state = state.conn_state;
                     send_event(LV_ZMK_EVENT_CODE(ble_active_profile))),
                    (return (struct ble_active_profile){
                      .index = ev->index,
                      .conn_state = zmk_ble_active_profile_is_connected() ? BLE_CONNECTED
                                    : zmk_ble_active_profile_is_open()    ? BLE_DISCONNECTED
                                                                          : BLE_NOT_PAIERD,
                    }));
#endif

// layer_state_changed
// ------------------------------------------------------------
#if IS_LV_ZMK_EVENT_ENABLED(layer_state)
#  include <zmk/keymap.h>
#  include <zmk/events/layer_state_changed.h>

struct layer_state {
  uint8_t index;
  const char *name;
};
ZMK_EVENT_PRESENTER(layer_state, struct layer_state,
                    (zmk_status.layer_index = state.index + 1; zmk_status.layer_name = state.name;
                     send_event(LV_ZMK_EVENT_CODE(layer_state))),
                    (ARG_UNUSED(ev);
                     zmk_keymap_layer_index_t index = zmk_keymap_highest_layer_active();
                     return (struct layer_state){
                       .index = index,
                       .name = zmk_keymap_layer_name(zmk_keymap_layer_index_to_id(index)),
                     }));
#endif

// usb_host_os_changed
// ------------------------------------------------------------
#if IS_LV_ZMK_EVENT_ENABLED(usb_host_os)
#  include <zmk/usb_host_os.h>
#  include <zmk/events/usb_host_os_changed.h>
ZMK_EVENT_PRESENTER(usb_host_os, enum usb_host_os,
                    (if (zmk_status.usb_conn_state >= USB_CONNECTED) {
                      switch (state) {
                        case USB_HOST_OS_DARWIN:
                          zmk_status.usb_conn_state = USB_CONNECTED_MAC;
                          send_event(LV_ZMK_EVENT_CODE(usb_host_os));
                          break;
                        case USB_HOST_OS_UNKNOWN:
                          zmk_status.usb_conn_state = USB_CONNECTED_WIN;
                          send_event(LV_ZMK_EVENT_CODE(usb_host_os));
                          break;
                        default:
                          break;
                      }
                    }),
                    (return ev != NULL ? ev->os : zmk_usb_host_os_detected()));
#endif

// split_peripheral_status_changed
// ------------------------------------------------------------
#if IS_LV_ZMK_EVENT_ENABLED(split_peripheral_status)
#  include <zmk/split/bluetooth/peripheral.h>
#  include <zmk/events/split_peripheral_status_changed.h>
ZMK_EVENT_PRESENTER(split_peripheral_status, bool,
                    (zmk_status.peripheral_connected = state;
                     send_event(LV_ZMK_EVENT_CODE(split_peripheral_status))),
                    (return ev != NULL ? ev->connected : zmk_split_bt_peripheral_is_connected()));
#endif

/* TODO auto register */
/*
static void _dispatch(lv_obj_t *obj) {
  uint32_t child_cnt = lv_obj_get_child_cnt(obj);
  // TODO obj -> append target list
  if (child_cnt > 0) {
    for (int idx = 0; idx < child_cnt; idx++) {
      _dispatch(lv_obj_get_child(obj, idx));
    }
  }
}
*/

#define EVENT_CODE_REGISTER(e) LV_ZMK_EVENT_CODE(e) = lv_event_register_id()
void zmk_status_presenter_init(void) { FOR_EACH_LV_ZMK_EVENT(EVENT_CODE_REGISTER, (;)); }

#define GET_TARGETS(e)                      \
  if (event_code == LV_ZMK_EVENT_CODE(e)) { \
    return &LV_ZMK_EVENT_TARGETS(e);        \
  }

static inline struct lv_obj_slist *get_targets(lv_event_code_t event_code) {
  FOR_EACH_LV_ZMK_EVENT(GET_TARGETS, ());
  return NULL;
}

static void send_event(lv_event_code_t event_code) {
  struct lv_obj_slist *list = get_targets(event_code);
  if (list != NULL) {
    lv_obj_t *obj = list->first;
    while (obj != NULL) {
      lv_event_send(obj, event_code, &zmk_status);
      obj = lv_obj_get_user_data(obj);
    }
  }
}

/* TODO auto register */

void zmk_status_presenter_register(lv_obj_t *obj, lv_event_code_t event_code) {
  struct lv_obj_slist *list = get_targets(event_code);
  if (list != NULL) {
    lv_obj_set_user_data(obj, NULL);
    if (list->first != NULL) {
      lv_obj_set_user_data(list->end, obj);
    } else {
      list->first = obj;
    }
    list->end = obj;
  }
}

#define ZMK_EVENT_INIT_FUNC(e) e##_init();
void zmk_status_presenter_dispatch(lv_obj_t *container) {
  status_screen = container;
  /* TODO auto register here */
  FOR_EACH_LV_ZMK_EVENT(ZMK_EVENT_INIT_FUNC, ())
}
