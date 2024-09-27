#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/usb/class/hid.h>

#include <zmk/usb_host_os.h>
#include <zmk/event_manager.h>
#include <zmk/events/usb_host_os_changed.h>
#include <zmk/events/usb_conn_state_changed.h>
#include "zephyr/drivers/usb/usb_dc.h"
#include "zmk/usb.h"

#define USB_HID_SETUP_TIMEOUT_MS 500

#if IS_ENABLED(CONFIG_ZMK_USB_HOST_OS_DEBUG)
#  define USB_SETUP_LOG_MAX 64
static struct usb_setup_packet usb_hid_setup_log[USB_SETUP_LOG_MAX];
#endif
static int packet_cnt;
static bool detecting;

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

static enum usb_host_os detected_os = USB_HOST_OS_UNDEFINED;

static void end_usb_hid_setup(struct k_work *work) {
  if (detected_os == USB_HOST_OS_UNDEFINED) {
    detected_os = USB_HOST_OS_UNKNOWN;
  }
  detecting = false;
  LOG_DBG("os detection end, packet_cnt: %d, detected_os:%d", packet_cnt, detected_os);
  raise_zmk_usb_host_os_changed((struct zmk_usb_host_os_changed){.os = zmk_usb_host_os_detected()});
}

static K_WORK_DELAYABLE_DEFINE(usb_host_os_work, end_usb_hid_setup);

void zmk_usb_host_os_trace_hid_setup(struct usb_setup_packet *setup) {
#if IS_ENABLED(CONFIG_ZMK_USB_HOST_OS_DEBUG)
  if (packet_cnt < USB_SETUP_LOG_MAX) {
    usb_hid_setup_log[packet_cnt] = *setup;
  }
#endif
  packet_cnt++;
  if (detected_os == USB_HOST_OS_UNDEFINED) {
    // only work when CONFIG_USB_DEVICE_VID=0x05AC and CONFIG_USB_DEVICE_PID = 0x024F
    //
    //   bRequest: '9:HID_SET_REPORT'
    //   reportType: '3:FEATURE_REPORT'
    //   reportId : 9
    //
    // TODO not tested on iOS
    //
    LOG_DBG("usb_host_os: bRequest: %d, wValue: %0x", setup->bRequest, setup->wValue);
    if (setup->bRequest == USB_HID_SET_REPORT && setup->wValue == 0x0309) {
      detected_os = USB_HOST_OS_DARWIN;
    }
    k_work_reschedule(&usb_host_os_work, K_MSEC(USB_HID_SETUP_TIMEOUT_MS));
  }
}

enum usb_host_os zmk_usb_host_os_detected() { return detected_os; }

#if IS_ENABLED(CONFIG_ZMK_USB_HOST_OS_DEBUG)
struct usb_setup_packet *get_usb_hid_setup_log_item(uint8_t index) {
  if (index < packet_cnt) {
    return &usb_hid_setup_log[index];
  }
  return NULL;
}
#endif  // CONFIG_ZMK_USB_HOST_OS_DEBUG

static int usb_conn_listener(const zmk_event_t *eh) {
  const struct zmk_usb_conn_state_changed *ev = as_zmk_usb_conn_state_changed(eh);
  static enum zmk_usb_conn_state prev_conn_state = ZMK_USB_CONN_NONE;
  /*
   * NRF (battery powered)
   *  reset keybaod
   *      USB_DC_RESET -> USB_DC_CONFIGURED
   *  disconnect cable
   *     USB_DC_CONFIGURED -> USB_DC_SUSPEND -> USB_DC_DISCONNECTED
   *  connnect cable
   *    USB_DC_DISCONNECTED -> USB_DC_RESUME -> USB_DC_CONFIGURED
   *  host os sleep
   *    USB_DC_CONFIGURED -> USB_DC_SUSPEND
   *  host os wake
   *    USB_DC_SUSPEND -> USB_DC_RESUME -> USB_DC_CONFIGURED
   */
  /*
  static uint8_t status_log[32];
  static uint8_t status_log_ptr;
  if (zmk_usb_get_status() == USB_DC_RESET) {
    memset(status_log, 0, 32);
    status_log_ptr = 0;
  }
  status_log[status_log_ptr++] = zmk_usb_get_status();
  LOG_HEXDUMP_DBG(status_log, 32, "USB Status log:");
  */
  if (!detecting &&
      (zmk_usb_get_status() == USB_DC_RESET ||
       (prev_conn_state == ZMK_USB_CONN_NONE && ev->conn_state == ZMK_USB_CONN_HID))) {
    LOG_DBG("os detection start");
    detecting = true;
    packet_cnt = 0;
    detected_os = USB_HOST_OS_UNDEFINED;
    k_work_reschedule(&usb_host_os_work, K_MSEC(USB_HID_SETUP_TIMEOUT_MS));
  }
  prev_conn_state = ev->conn_state;
  return 0;
}

ZMK_LISTENER(usb_host_os_usb_conn, usb_conn_listener);
ZMK_SUBSCRIPTION(usb_host_os_usb_conn, zmk_usb_conn_state_changed);

// TODO research the support status of each indicator usage ID
// ZMK_LISTENER(usb_host_os_indicator, indicators_listener);
// ZMK_SUBSCRIPTION(usb_host_os_indicators, zmk_hid_indicators_changed);
