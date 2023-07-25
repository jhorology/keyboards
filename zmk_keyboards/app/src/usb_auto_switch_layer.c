
#include <stdint.h>
#include <zephyr/kernel.h>

#include <zmk/evil/usb_host_os.h>
#include <zmk/evil/events/usb_host_os_changed.h>

#include <zmk/event_manager.h>
#include <zmk/keymap.h>
#include <zmk/endpoints.h>
#include <zmk/events/endpoint_selection_changed.h>

LOG_MODULE_DECLARE(evil, CONFIG_EVIL_LOG_LEVEL);

static int auto_switch_layer_listener(const zmk_event_t *eh) {
  enum zmk_endpoint endpoint = zmk_endpoints_selected();
  enum usb_host_os os = zmk_usb_host_os_detected();
  LOG_DBG("os: %d endpoint: %d", os, endpoint);
  if (endpoint == ZMK_ENDPOINT_USB && os != USB_HOST_OS_UNDEFINED) {
    uint8_t layer = os == USB_HOST_OS_DARWIN ? CONFIG_ZMK_USB_AUTO_SWITCH_LAYER_IF_DARWIN
                                             : CONFIG_ZMK_USB_AUTO_SWITCH_LAYER_UNLESS_DARWIN;
    LOG_DBG("layer_to: %d", layer);
    zmk_keymap_layer_to(layer);
  }
  return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(auto_switch_layer, auto_switch_layer_listener);
ZMK_SUBSCRIPTION(auto_switch_layer, zmk_endpoint_selection_changed);
ZMK_SUBSCRIPTION(auto_switch_layer, zmk_usb_host_os_changed);
