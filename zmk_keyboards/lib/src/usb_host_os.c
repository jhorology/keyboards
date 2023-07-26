#include <evil/usb_host_os.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/usb/usb_device.h>
#include "zephyr/linker/linker-defs.h"

LOG_MODULE_REGISTER(evil, CONFIG_EVIL_LOG_LEVEL);

#define USB_SETUP_LOG_MAX 64
#define USB_SETUP_TIMEOUT_MS 1000
#define IS_GET_DEVICE_DESCRIPTOR(setup)                                                                               \
  (setup->RequestType.recipient == USB_REQTYPE_RECIPIENT_DEVICE &&                                                    \
   setup->RequestType.type == USB_REQTYPE_TYPE_STANDARD && setup->RequestType.direction == USB_REQTYPE_DIR_TO_HOST && \
   setup->bRequest == USB_SREQ_GET_DESCRIPTOR)

static usb_host_os_callback user_cb;
static enum usb_host_os detected_os = USB_HOST_OS_UNDEFINED;
static enum usb_host_os notified_os = USB_HOST_OS_UNDEFINED;
static volatile int packet_cnt;
static bool detecting = false;
static bool end_detect = false;
static uint32_t wlengths = 0;
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
  bool is_get_device_descriptor = IS_GET_DEVICE_DESCRIPTOR(setup);
  uint8_t descriptor_type = USB_GET_DESCRIPTOR_TYPE(setup->wValue);
  if (!detecting && is_get_device_descriptor && descriptor_type == USB_DESC_DEVICE) {
    packet_cnt = 0;
    detecting = true;
    end_detect = false;
    wlengths = 0;
  }
  if (detecting) {
#if IS_ENABLED(CONFIG_USB_DETECT_HOST_OS_DEBUG)
    if (packet_cnt < USB_SETUP_LOG_MAX) {
      usb_setup_log[packet_cnt++] = *setup;
    }
#endif  // CONFIG_USB_DETECT_HOST_OS_DEBGU
    if (!end_detect) {
      if (is_get_device_descriptor) {
        // detect DARWIN (macOS iOS iPadOS)
        if (descriptor_type == USB_DESC_CONFIGURATION && (wlengths & 0xff00ff00) == 0x02000200) {
          detected_os = USB_HOST_OS_DARWIN;
          LOG_DBG("detected os: %d", detected_os);
          notify_os();
          end_detect = true;
        }
        // TODO other OS
        wlengths <<= 8;
        if (descriptor_type == USB_DESC_STRING) {
          wlengths += setup->wLength & 0xff;
        }
      }
    }
    k_work_reschedule(&usb_host_os_work, K_MSEC(USB_SETUP_TIMEOUT_MS));
  }
}

static void usb_end_setup(struct k_work *work) {
  LOG_DBG("end setup");
  if (!end_detect) {
    detected_os = USB_HOST_OS_UNKNOWN;
    LOG_DBG("detected os: %d", detected_os);
    notify_os();
    end_detect = true;
  }
  detecting = false;
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
