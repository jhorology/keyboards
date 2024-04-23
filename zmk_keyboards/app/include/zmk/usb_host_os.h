#pragma once

#include <zephyr/usb/usb_device.h>

enum usb_host_os { USB_HOST_OS_UNDEFINED, USB_HOST_OS_DARWIN, USB_HOST_OS_UNKNOWN };

enum usb_host_os zmk_usb_host_os_detected(void);
void zmk_usb_host_os_trace_hid_setup(struct usb_setup_packet *setup);
#if IS_ENABLED(CONFIG_ZMK_USB_HOST_OS_DEBUG)
struct usb_setup_packet *get_usb_hid_setup_log_item(uint8_t index);
#endif
