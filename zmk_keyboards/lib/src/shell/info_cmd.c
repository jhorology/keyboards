#include <zephyr/shell/shell.h>

#if IS_ENABLED(CONFIG_PICOSDK_USE_FLASH)
#  include <hardware/flash.h>
#  define FLASH_RDID_CMD 0x9f
#  define FLASH_RDID_TOTAL_BYTES (3 + 1)
#endif
#if IS_ENABLED(CONFIG_USB_DETECT_HOST_OS_DEBUG)
#  include <evil/usb_host_os.h>
#  include <zephyr/usb/usb_device.h>
#  include <zephyr/usb/class/usb_hid.h>
#endif

#define USB_SETUP_LOG_MAX 64

static int cmd_hello(const struct shell *sh, size_t argc, char **argv) {
  shell_fprintf(sh, SHELL_NORMAL, "hello!\n");
  return 0;
}

#if IS_ENABLED(CONFIG_PICOSDK_USE_FLASH)
// zephyr 3.2 Flash API dosen't support RP2040
// for research purposes only, maybe danger with running application
static int cmd_get_rp2_flash_info(const struct shell *sh, size_t argc, char **argv) {
  uint8_t txbuf[FLASH_RDID_TOTAL_BYTES] = {0};
  uint8_t rxbuf[FLASH_RDID_TOTAL_BYTES] = {0};
  txbuf[0] = FLASH_RDID_CMD;
  flash_do_cmd(txbuf, rxbuf, FLASH_RDID_TOTAL_BYTES);
  shell_fprintf(sh, SHELL_NORMAL, "jedec-id = [%02x %02x %02x];\n", rxbuf[1], rxbuf[2], rxbuf[3]);
  shell_fprintf(sh, SHELL_NORMAL, "Probably the capacity is %d bytes\n", 1 << rxbuf[3]);
  return 0;
}
#endif

#if IS_ENABLED(CONFIG_USB_DETECT_HOST_OS_DEBUG)

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

static const char *strRequest(struct usb_setup_packet *setup) {
  // Standard Device Request
  if (setup->RequestType.type == USB_REQTYPE_TYPE_STANDARD) {
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

static int cmd_get_usb_setup_log(const struct shell *sh, size_t argc, char **argv) {
  struct usb_setup_packet *setup;
  uint8_t i = 0;
  while ((setup = get_usb_setup_log_item(i++)) != NULL) {
    shell_fprintf(sh, SHELL_NORMAL,
                  "{\n"
                  "  bmRequestType: {\n"
                  "    recipient: '%d:%s',\n"
                  "    type: '%d:%s',\n"
                  "    direction: '%d:%s'\n"
                  "  },\n"
                  "  bRequest: '%d:%s',\n",
                  setup->RequestType.recipient, strRecipiente(setup), setup->RequestType.type, strType(setup),
                  setup->RequestType.direction, usb_reqtype_is_to_host(setup) ? "TO_HOST" : "TO_DEVICE",
                  setup->bRequest, strRequest(setup));
    if (setup->RequestType.type == USB_REQTYPE_TYPE_STANDARD && setup->bRequest == USB_SREQ_GET_DESCRIPTOR) {
      shell_fprintf(sh, SHELL_NORMAL,
                    "  descType: '%doo:%s',\n"
                    "  descIndex: %d,\n",
                    USB_GET_DESCRIPTOR_TYPE(setup->wValue), strDescType(setup),
                    USB_GET_DESCRIPTOR_INDEX(setup->wValue));
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
#endif  // CONFIG_USB_DETECT_HOST_OS_DEBUG

SHELL_STATIC_SUBCMD_SET_CREATE(sub_info, SHELL_CMD_ARG(hello, NULL, "Hello", cmd_hello, 1, 0),
#if IS_ENABLED(CONFIG_PICOSDK_USE_FLASH)
                               SHELL_CMD_ARG(rp2flash, NULL, "show flash chip info", cmd_get_rp2_flash_info, 1, 0),
#endif
#if IS_ENABLED(CONFIG_USB_DETECT_HOST_OS_DEBUG)
                               SHELL_CMD_ARG(usb_setup, NULL, "show usb setup log", cmd_get_usb_setup_log, 1, 0),
#endif
                               SHELL_SUBCMD_SET_END /* Array terminated. */
);
SHELL_CMD_ARG_REGISTER(info, &sub_info, "INFO commands", NULL, 2, 0);
