#include <evil/usb_host_os.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/usb/usb_device.h>
#include "zephyr/linker/linker-defs.h"

LOG_MODULE_REGISTER(evil, CONFIG_EVIL_LOG_LEVEL);

#define USB_SETUP_LOG_MAX 64
#define USB_SETUP_TIMEOUT_MS 1000

static usb_host_os_callback user_cb;

static enum usb_host_os detected_os = USB_HOST_OS_UNDEFINED;
static enum usb_host_os notified_os = USB_HOST_OS_UNDEFINED;
static volatile int packet_cnt;
static bool maybe_darwin = false;
static bool during_setup = false;
static bool end_detect = false;

static void usb_end_setup(struct k_work *work);
static K_WORK_DELAYABLE_DEFINE(usb_host_os_work, usb_end_setup);

#if IS_ENABLED(CONFIG_USB_DETECT_HOST_OS_DEBUG)
static struct usb_setup_packet usb_setup_log[USB_SETUP_LOG_MAX];
#endif  // CONFIG_USB_DETECT_HOST_OS_DEBGU

static inline void notify_os() {
  if (user_cb != NULL && detected_os != notified_os) {
    LOG_DBG("notify os: %d", detected_os);
    user_cb(detected_os);
    notified_os = detected_os;
  }
}

void usb_trace_setup(struct usb_setup_packet *setup) {
  if (!during_setup) {
    maybe_darwin = false;
    packet_cnt = 0;
    during_setup = true;
    end_detect = false;
  }
#if IS_ENABLED(CONFIG_USB_DETECT_HOST_OS_DEBUG)
  if (packet_cnt < USB_SETUP_LOG_MAX) {
    usb_setup_log[packet_cnt] = *setup;
  }
#endif  // CONFIG_USB_DETECT_HOST_OS_DEBGU
  if (!end_detect) {
    // TODO super easy way for now
    // TODO dosen't work as expected with nrf52840
    // macOS send SET_ADDRESS request at first
    if (packet_cnt == 0 && setup->bmRequestType == 0 && setup->bRequest == USB_SREQ_SET_ADDRESS) {
      maybe_darwin = true;
    }
    // igonore packets that are sended when wakeup from sleep
    if (packet_cnt >= 4 && (setup->RequestType.type != USB_REQTYPE_TYPE_STANDARD ||
                            setup->RequestType.recipient != USB_REQTYPE_RECIPIENT_DEVICE)) {
      detected_os = maybe_darwin ? USB_HOST_OS_DARWIN : USB_HOST_OS_UNKNOWN;
      LOG_DBG("detected os: %d", detected_os);
      notify_os();
      end_detect = true;
    }
  }
  packet_cnt++;
  k_work_reschedule(&usb_host_os_work, K_MSEC(USB_SETUP_TIMEOUT_MS));
}

static void usb_end_setup(struct k_work *work) {
  LOG_DBG("end setup");
  during_setup = false;
  notify_os();
}

void enable_usb_host_os(usb_host_os_callback cb) { user_cb = cb; };

#if IS_ENABLED(CONFIG_USB_DETECT_HOST_OS_DEBUG)
struct usb_setup_packet *get_usb_setup_log_item(uint8_t index) {
  if (index < packet_cnt) {
    return &usb_setup_log[index];
  }
  return NULL;
}
#endif  // CONFIG_USB_DETECT_HOST_OS_DEBGU
