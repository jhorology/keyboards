#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/usb/class/hid.h>

#include <zmk/usb_host_os.h>
#include <zmk/event_manager.h>
#include <zmk/events/usb_host_os_changed.h>
#include <zmk/events/usb_conn_state_changed.h>
#include "zephyr/drivers/usb/usb_dc.h"
#include "zmk/usb.h"

#define USB_HID_SETUP_TIMEOUT_MS 300

#if IS_ENABLED(CONFIG_ZMK_USB_HOST_OS_DEBUG)
#  define USB_SETUP_LOG_MAX 64
static struct usb_setup_packet usb_hid_setup_log[USB_SETUP_LOG_MAX];
static volatile int packet_cnt;
#endif

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

static enum usb_host_os detected_os = USB_HOST_OS_UNDEFINED;
static volatile enum usb_host_os _detected_os;

static void end_usb_hid_setup(struct k_work *work) {
  detected_os = _detected_os;
  _detected_os = USB_HOST_OS_UNDEFINED;
  LOG_DBG("os detection end,  detected_os:%d", detected_os);
  raise_zmk_usb_host_os_changed((struct zmk_usb_host_os_changed){.os = zmk_usb_host_os_detected()});
}
static K_WORK_DELAYABLE_DEFINE(usb_host_os_work, end_usb_hid_setup);

void zmk_usb_host_os_trace_hid_setup(struct usb_setup_packet *setup) {
#if IS_ENABLED(CONFIG_ZMK_USB_HOST_OS_DEBUG)
  if (packet_cnt < USB_SETUP_LOG_MAX) {
    usb_hid_setup_log[packet_cnt++] = *setup;
  }
#endif
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
    _detected_os = USB_HOST_OS_DARWIN;
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
  /*
   * NRF (battery powered)
   *  reset keybaod
   *     2 USB_DC_CONNECTED
   *     5 USB_DC_SUSPEND
   *     1 USB_DC_RESET
   *     3 USB_DC_CONFIGURED
   *  disconnect cable
   *     3 USB_DC_CONFIGURED
   *     5 USB_DC_SUSPEND
   *     4 USB_DC_DISCONNECTED
   *  connnect cable
   *    4 USB_DC_DISCONNECTED
   *    2 USB_DC_CONNECTED
   *    5 USB_DC_SUSPEND
   *    6 USB_DC_RESUME
   *    3 USB_DC_CONFIGURED
   *  host os sleep
   *    3 USB_DC_CONFIGURED
   *    5 USB_DC_SUSPEND
   *  host os wake
   *    5 USB_DC_SUSPEND
   *    6 USB_DC_RESUME
   *  KVM Switch
   *    Reset ->      Switch Win ->                Switch Mac
   *    02 05 01 03   05 04 01 05 06 05 06 01 03   05 04 01 05 06 05 06 03
   *  KVM Switch (cascade powered-hub)
   *    Reste ->      Switch Win ->             Switch Mac
   *    02 05 01 03   05 06 05 06 05 06 01 03   05 06 05 06 05 06 03
   */
  if (zmk_usb_get_status() == USB_DC_CONFIGURED) {
    k_work_reschedule(&usb_host_os_work, K_MSEC(USB_HID_SETUP_TIMEOUT_MS));
  }
  if (_detected_os == USB_HOST_OS_UNDEFINED) {
    _detected_os = USB_HOST_OS_UNKNOWN;
#if IS_ENABLED(CONFIG_ZMK_USB_HOST_OS_DEBUG)
    packet_cnt = 0;
#endif
  }
  return 0;
}

ZMK_LISTENER(usb_host_os_usb_conn, usb_conn_listener);
ZMK_SUBSCRIPTION(usb_host_os_usb_conn, zmk_usb_conn_state_changed);

// TODO research the support status of each indicator usage ID
// ZMK_LISTENER(usb_host_os_indicator, indicators_listener);
// ZMK_SUBSCRIPTION(usb_host_os_indicators, zmk_hid_indicators_changed);
