#pragma once

#include <zmk/usb_host_os.h>
#include <zmk/event_manager.h>

struct zmk_usb_host_os_changed {
  enum usb_host_os os;
};

ZMK_EVENT_DECLARE(zmk_usb_host_os_changed);
