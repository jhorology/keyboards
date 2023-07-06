#pragma once

#include <zephyr/usb/usb_device.h>

enum usb_host_os { USB_HOST_OS_UNDEFINED, USB_HOST_OS_DARWIN, USB_HOST_OS_UNKNOWN };

typedef void (*usb_host_os_callback)(enum usb_host_os os);

void enable_usb_host_os(usb_host_os_callback cb);

#if IS_ENABLED(CONFIG_ZMK_USB_HOST_OS_DEBUG)
struct usb_setup_packet *get_usb_setup_log_item(int index);
#endif
