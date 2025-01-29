
#include <lvgl.h>
#include <zmk/event_manager.h>
#include <zmk/display.h>
#include <zmk/display/lv_zmk_event.h>
#include <zmk/display/lv_zmk_status.h>
#include <zmk/display/status_presenter.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#define ZMK_EVENT_PRESENTER(e, state_type, update_code, get_code)                               \
  static void display_##e##_update_cb(state_type state) { __DEBRACKET update_code }             \
  static state_type display_##e##_state_get(const zmk_event_t *eh) {                            \
    const struct zmk_##e##_changed *ev = as_zmk_##e##_changed(eh);                              \
    __DEBRACKET get_code                                                                        \
  }                                                                                             \
  ZMK_DISPLAY_WIDGET_LISTENER(e, state_type, display_##e##_update_cb, display_##e##_state_get); \
  ZMK_SUBSCRIPTION(e, zmk_##e##_changed);

#define _LV_ZMK_EVENT_DECLARE(e) lv_event_code_t LV_ZMK_EVENT_CODE(e);
LV_ZMK_EVENT_FOR_EACH(_LV_ZMK_EVENT_DECLARE, ())

static lv_obj_t *status_screen;
static uint8_t broadcast_depth;
static struct lv_zmk_status zmk_status;

static void send_event(lv_obj_t *obj, lv_event_code_t event_code, lv_zmk_event_interests mask,
                       uint8_t depth);

#define SEND_EVENT(e) send_event(status_screen, LV_ZMK_EVENT_CODE(e), LV_ZMK_EVENT_INTEREST(e), 0)

// battery_state_changed
// ------------------------------------------------------------
#if LV_ZMK_EVENT_IS_ENABLED(battery_state)
#  include <zmk/battery.h>
#  include <zmk/events/battery_state_changed.h>
ZMK_EVENT_PRESENTER(battery_state, uint8_t,
                    (zmk_status.battery_level = state; SEND_EVENT(battery_state);),
                    (return (ev != NULL) ? ev->state_of_charge : zmk_battery_state_of_charge();));
#endif

// usb_conn_state_changed
// ------------------------------------------------------------
#if LV_ZMK_EVENT_IS_ENABLED(usb_conn_state)
#  include <zmk/usb.h>
#  include <zmk/events/usb_conn_state_changed.h>
ZMK_EVENT_PRESENTER(usb_conn_state, enum zmk_usb_conn_state,
                    (zmk_status.battery_charging = state != ZMK_USB_CONN_NONE;
                     zmk_status.usb_conn_state = state == ZMK_USB_CONN_HID ? USB_CONNECTED
                                                                           : USB_DISCONNECTED;
                     SEND_EVENT(usb_conn_state);),
                    (return (ev != NULL) ? ev->conn_state : zmk_usb_get_conn_state();));
#endif

// endpoint_changed
// ------------------------------------------------------------
#if LV_ZMK_EVENT_IS_ENABLED(endpoint)
#  include <zmk/endpoints_types.h>
#  include <zmk/endpoints.h>
#  include <zmk/events/endpoint_changed.h>
ZMK_EVENT_PRESENTER(endpoint, struct zmk_endpoint_instance,
                    (zmk_status.usb_selected = state.transport == ZMK_TRANSPORT_USB;
                     zmk_status.ble_selected = state.transport == ZMK_TRANSPORT_BLE;
                     SEND_EVENT(endpoint);),
                    (return (ev != NULL) ? ev->endpoint : zmk_endpoints_selected();));

#endif

// ble_active_profile_changed
// ------------------------------------------------------------
#if LV_ZMK_EVENT_IS_ENABLED(ble_active_profile)
#  include <zmk/ble.h>
#  include <zmk/events/ble_active_profile_changed.h>
struct ble_active_profile {
  uint8_t index;
  enum lv_zmk_ble_conn_state conn_state;
};
ZMK_EVENT_PRESENTER(ble_active_profile, struct ble_active_profile,
                    (zmk_status.ble_profile_index = state.index + 1;
                     zmk_status.ble_conn_state = state.conn_state; SEND_EVENT(ble_active_profile);),
                    (return (struct ble_active_profile){
                      .index = ev != NULL ? ev->index : zmk_ble_active_profile_index(),
                      .conn_state = zmk_ble_active_profile_is_connected() ? BLE_CONNECTED
                                    : zmk_ble_active_profile_is_open()    ? BLE_DISCONNECTED
                                                                          : BLE_NOT_PAIERD,
                    };));
#endif

// layer_state_changed
// ------------------------------------------------------------
#if LV_ZMK_EVENT_IS_ENABLED(layer_state)
#  include <zmk/keymap.h>
#  include <zmk/events/layer_state_changed.h>
struct layer_state {
  uint8_t index;
  const char *name;
};
ZMK_EVENT_PRESENTER(layer_state, struct layer_state,
                    (zmk_status.layer_index = state.index + 1; zmk_status.layer_name = state.name;
                     SEND_EVENT(layer_state);),
                    (ARG_UNUSED(ev);
                     zmk_keymap_layer_index_t index = zmk_keymap_highest_layer_active();
                     return (struct layer_state){
                       .index = index,
                       .name = zmk_keymap_layer_name(zmk_keymap_layer_index_to_id(index)),
                     };));
#endif

// usb_host_os_changed
// ------------------------------------------------------------
#if LV_ZMK_EVENT_IS_ENABLED(usb_host_os)
#  include <zmk/usb_host_os.h>
#  include <zmk/events/usb_host_os_changed.h>
ZMK_EVENT_PRESENTER(usb_host_os, enum usb_host_os,
                    (if (zmk_status.usb_conn_state >= USB_CONNECTED) {
                      switch (state) {
                        case USB_HOST_OS_DARWIN:
                          zmk_status.usb_conn_state = USB_CONNECTED_MAC;
                          SEND_EVENT(usb_host_os);
                          break;
                        case USB_HOST_OS_UNKNOWN:
                          zmk_status.usb_conn_state = USB_CONNECTED_WIN;
                          SEND_EVENT(usb_host_os);
                          break;
                        default:
                          break;
                      }
                    }),
                    (return ev != NULL ? ev->os : zmk_usb_host_os_detected();));
#endif

// split_peripheral_status_changed
// ------------------------------------------------------------
#if LV_ZMK_EVENT_IS_ENABLED(split_peripheral_status)
#  include <zmk/split/bluetooth/peripheral.h>
#  include <zmk/events/split_peripheral_status_changed.h>
ZMK_EVENT_PRESENTER(split_peripheral_status, bool,
                    (zmk_status.peripheral_connected = state; SEND_EVENT(split_peripheral_status);),
                    (return ev != NULL ? ev->connected : zmk_split_bt_peripheral_is_connected();));
#endif

/* TODO use pre-registration list instead of broadcast  */
static void send_event(lv_obj_t *obj, lv_event_code_t event_code, lv_zmk_event_interests mask,
                       uint8_t depth) {
  lv_zmk_event_interests *interests = (lv_zmk_event_interests *)lv_obj_get_user_data(obj);
  LOG_DBG("event: %d, mask: 0x%04x, depth: %d", event_code, mask, depth);
  if (interests != NULL) {
    LOG_DBG("interests: 0x%04x", *interests);
    if (*interests & mask) {
      lv_event_send(obj, event_code, &zmk_status);
      /* it is parent's responsibility to propagate events to children. */
      return;
    }
  }
  uint32_t cnt = lv_obj_get_child_cnt(obj);
  if (cnt > 0 && depth < broadcast_depth) {
    for (uint32_t i = 0; i < cnt; i++) {
      send_event(lv_obj_get_child(obj, i), event_code, mask, depth + 1);
    }
  }
}

#define EVENT_CODE_REGISTER(e) LV_ZMK_EVENT_CODE(e) = lv_event_register_id();
void zmk_status_presenter_init(void) { LV_ZMK_EVENT_FOR_EACH(EVENT_CODE_REGISTER, ()) }

#define ZMK_EVENT_INIT_FUNC(e) e##_init();
void zmk_status_presenter_dispatch(lv_obj_t *container, uint8_t depth) {
  status_screen = container;
  broadcast_depth = depth;
  /* TODO pre-registration  */
  LV_ZMK_EVENT_FOR_EACH(ZMK_EVENT_INIT_FUNC, ())
}
