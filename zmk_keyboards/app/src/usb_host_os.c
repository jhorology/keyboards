#include <zephyr/init.h>
#include <evil/usb_host_os.h>

#include <zmk/evil/usb_host_os.h>
#include <zmk/event_manager.h>
#include <zmk/evil/events/usb_host_os_changed.h>

LOG_MODULE_DECLARE(evil, CONFIG_EVIL_LOG_LEVEL);

static enum usb_host_os detected_os = USB_HOST_OS_UNDEFINED;

static void raise_usb_host_os_changed_event(struct k_work *_work) {
  ZMK_EVENT_RAISE(new_zmk_usb_host_os_changed((struct zmk_usb_host_os_changed){.os = zmk_usb_host_os_detected()}));
}

K_WORK_DEFINE(usb_host_os_notifier_work, raise_usb_host_os_changed_event);

enum usb_host_os zmk_usb_host_os_detected() { return detected_os; }

static void zmk_usb_host_os_cb(enum usb_host_os os) {
  detected_os = os;
  k_work_submit(&usb_host_os_notifier_work);
};

static int zmk_usb_host_os_init(void) {
  enable_usb_host_os(zmk_usb_host_os_cb);
  return 0;
}

SYS_INIT(zmk_usb_host_os_init, APPLICATION, CONFIG_ZMK_USB_INIT_PRIORITY);
