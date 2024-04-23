#include <zephyr/shell/shell.h>
#include <zephyr/usb/class/hid.h>
#if IS_ENABLED(CONFIG_ZMK_USB_HOST_OS_DEBUG)
#  include <zephyr/usb/usb_device.h>
#  include <zmk/usb_host_os.h>
#endif  // CONFIG_ZMK_USB_HOST_OS_DEBUG

static int cmd_hello(const struct shell *sh, size_t argc, char **argv) {
  shell_fprintf(sh, SHELL_NORMAL, "hello!\n");
  return 0;
}

#if IS_ENABLED(CONFIG_ZMK_USB_HOST_OS_DEBUG)

static const char *strType(struct usb_setup_packet *setup) {
  switch (setup->RequestType.type) {
    case USB_REQTYPE_TYPE_STANDARD:
      return "STANDARD";
    case USB_REQTYPE_TYPE_CLASS:
      return "CLASS";
    case USB_REQTYPE_TYPE_VENDOR:
      return "VENDOR";
    case USB_REQTYPE_TYPE_RESERVED:
      return "RESERVED";
  }
  return "UNKNOWN";
}

static const char *strRecipiente(struct usb_setup_packet *setup) {
  switch (setup->RequestType.recipient) {
    case USB_REQTYPE_RECIPIENT_DEVICE:
      return "DEVICE";
    case USB_REQTYPE_RECIPIENT_INTERFACE:
      return "INTERFACE";
    case USB_REQTYPE_RECIPIENT_ENDPOINT:
      return "ENDPOINT";
    case USB_REQTYPE_RECIPIENT_OTHER:
      return "OTHER";
  }
  return "UNKNOWN";
}

static const char *strStandardRequest(struct usb_setup_packet *setup) {
  // Standard Device Request
  switch (setup->bRequest) {
    case USB_SREQ_GET_STATUS:
      return "GET_STATUS";
    case USB_SREQ_CLEAR_FEATURE:
      return "CLEAR_FEATURE";
    case USB_SREQ_SET_FEATURE:
      return "SET_FEATURE";
    case USB_SREQ_SET_ADDRESS:
      return "SET_ADDRESS";
    case USB_SREQ_GET_DESCRIPTOR:
      return "GET_DESCRIPTOR";
    case USB_SREQ_SET_DESCRIPTOR:
      return "SET_DESCRIPTOR";
    case USB_SREQ_GET_CONFIGURATION:
      return "GET_CONFIGURATION";
    case USB_SREQ_SET_CONFIGURATION:
      return "SET_CONFIGURATION";
    case USB_SREQ_GET_INTERFACE:
      return "GET_INTERFACE";
    case USB_SREQ_SET_INTERFACE:
      return "SET_INTERFACE";
    case USB_SREQ_SYNCH_FRAME:
      return "SYNCH_FRAME";
  }
  return "UNKNOWN";
}

static const char *strHIDClassRequest(struct usb_setup_packet *setup) {
  // Standard Device Request
  switch (setup->bRequest) {
    case USB_HID_GET_REPORT:
      return "HID_GET_REPORT";
    case USB_HID_GET_IDLE:
      return "HID_GET_IDLE";
    case USB_HID_GET_PROTOCOL:
      return "HID_GET_PROTOCOL";
    case USB_HID_SET_REPORT:
      return "HID_SET_REPORT";
    case USB_HID_SET_IDLE:
      return "HID_SET_IDLE";
    case USB_HID_SET_PROTOCOL:
      return "HID_SET_PROTOCOL";
  }
  return "UNKNOWN";
}

static const char *strDescType(struct usb_setup_packet *setup) {
  switch (USB_GET_DESCRIPTOR_TYPE(setup->wValue)) {
    case USB_DESC_DEVICE:
      return "DEVICE";
    case USB_DESC_CONFIGURATION:
      return "CONFIGURATION";
    case USB_DESC_STRING:
      return "STRING";
    case USB_DESC_INTERFACE:
      return "INTERFACE";
    case USB_DESC_ENDPOINT:
      return "ENDPOINT";
    case USB_DESC_DEVICE_QUALIFIER:
      return "DEVICE_QUALIFIER";
    case USB_DESC_OTHER_SPEED:
      return "OTHER_SPEED";
    case USB_DESC_INTERFACE_POWER:
      return "INTERFACE_POWER";

      /** Additional Descriptor Types defined in USB 3 spec. Table 9-5 */
    case USB_DESC_OTG:
      return "OTG";
    case USB_DESC_DEBUG:
      return "DEBUG";
    case USB_DESC_INTERFACE_ASSOC:
      return "INTERFACE_ASSOC";
    case USB_DESC_BOS:
      return "BOS";
    case USB_DESC_DEVICE_CAPABILITY:
      return "DEVICE_CAPABILITY";

      /** Class-Specific Descriptor Types as defined by
       *  USB Common Class Specification
       */
    case USB_DESC_CS_DEVICE:
      return "CLASS_DEVICE";
    case USB_DESC_CS_CONFIGURATION:
      return "CLASS_CONFIGURATION";
    case USB_DESC_CS_STRING:
      return "CLASS_STRING";
    case USB_DESC_CS_INTERFACE:
      return "CLASS_INTERFACE";
    case USB_DESC_CS_ENDPOINT:
      return "CLASS_ENDPOINT";
  }
  return "UNKNOWN";
}

static const char *strHIDReportType(struct usb_setup_packet *setup) {
  switch (USB_GET_DESCRIPTOR_TYPE(setup->wValue)) {
    case 1:
      return "INPUT_REPORT";
    case 2:
      return "OUTPUT_REPORT";
    case 3:
      return "FEATURE_REPORT";
  }
  return "UNKNOWN";
}

static int cmd_usb_setup_log(const struct shell *sh, size_t argc, char **argv) {
  struct usb_setup_packet *setup;
  uint8_t i = 0;
  while ((setup = get_usb_hid_setup_log_item(i++)) != NULL) {
    shell_fprintf(sh, SHELL_NORMAL,
                  "{\n"
                  "  bmRequestType: {\n"
                  "    recipient: '%d:%s',\n"
                  "    type: '%d:%s',\n"
                  "    direction: '%d:%s'\n"
                  "  },\n"
                  "  bRequest: '%d:%s',\n",
                  setup->RequestType.recipient, strRecipiente(setup), setup->RequestType.type,
                  strType(setup), setup->RequestType.direction,
                  usb_reqtype_is_to_host(setup) ? "TO_HOST" : "TO_DEVICE", setup->bRequest,
                  setup->RequestType.type == USB_REQTYPE_TYPE_STANDARD ? strStandardRequest(setup)
                  : setup->RequestType.type == USB_REQTYPE_TYPE_CLASS  ? strHIDClassRequest(setup)
                                                                       : "Unknown");
    if (setup->RequestType.type == USB_REQTYPE_TYPE_STANDARD &&
        setup->bRequest == USB_SREQ_GET_DESCRIPTOR) {
      shell_fprintf(sh, SHELL_NORMAL,
                    "  descType: '%d:%s',\n"
                    "  descIndex: %d,\n",
                    USB_GET_DESCRIPTOR_TYPE(setup->wValue), strDescType(setup),
                    USB_GET_DESCRIPTOR_INDEX(setup->wValue));
    } else if (setup->RequestType.type == USB_REQTYPE_TYPE_CLASS) {
      switch (setup->bRequest) {
        case USB_HID_GET_REPORT:
        case USB_HID_GET_IDLE:
        case USB_HID_GET_PROTOCOL:
        case USB_HID_SET_REPORT:
        case USB_HID_SET_PROTOCOL:
          shell_fprintf(sh, SHELL_NORMAL,
                        "  reportType: '%d:%s',\n"
                        "  reportId: %d,\n",
                        USB_GET_DESCRIPTOR_TYPE(setup->wValue), strHIDReportType(setup),
                        USB_GET_DESCRIPTOR_INDEX(setup->wValue));
          break;
        case USB_HID_SET_IDLE:
          shell_fprintf(sh, SHELL_NORMAL,
                        "  intervalMs: %d,\n"
                        "  reportId: %d,\n",
                        USB_GET_DESCRIPTOR_TYPE(setup->wValue) * 4,
                        USB_GET_DESCRIPTOR_INDEX(setup->wValue));
          break;
        default:
          shell_fprintf(sh, SHELL_NORMAL, "  wValue: 0x%04x\n", setup->wValue);
      }
    } else {
      shell_fprintf(sh, SHELL_NORMAL, "  wValue: 0x%04x,\n", setup->wValue);
    }
    shell_fprintf(sh, SHELL_NORMAL,
                  "  wIndex: 0x%04x,\n"
                  "  wLength: 0x%04x\n"
                  "},\n",
                  setup->wIndex, setup->wLength);
  }
  return 0;
}
#endif  // CONFIG_ZMK_USB_HOST_OS_DEBUG

SHELL_STATIC_SUBCMD_SET_CREATE(sub_zmk, SHELL_CMD_ARG(hello, NULL, "Hello", cmd_hello, 1, 0),
#if IS_ENABLED(CONFIG_ZMK_USB_HOST_OS_DEBUG)
                               SHELL_CMD_ARG(usb_setup_log, NULL, "show usb HID class setup log",
                                             cmd_usb_setup_log, 1, 0),
#endif  // CONFIG_ZMK_USB_HOST_OS_DEBUG

                               SHELL_SUBCMD_SET_END /* Array terminated. */
);
SHELL_CMD_ARG_REGISTER(zmk, &sub_zmk, "INFO commands", NULL, 2, 0);
