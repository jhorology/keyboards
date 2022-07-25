#include QMK_KEYBOARD_H

#include "my_keyboard_common.h"
#include "usb_descriptor.h"
#include "usb_descriptor_common.h"

#ifdef ALTERNATE_PRODUCT_ID

/*
 * alternate Device descriptor
 */
const USB_Descriptor_Device_t PROGMEM AlternateDeviceDescriptor = {
    .Header = {.Size = sizeof(USB_Descriptor_Device_t), .Type = DTYPE_Device},
    .USBSpecification = VERSION_BCD(2, 0, 0),

#  if VIRTSER_ENABLE
    .Class = USB_CSCP_IADDeviceClass,
    .SubClass = USB_CSCP_IADDeviceSubclass,
    .Protocol = USB_CSCP_IADDeviceProtocol,
#  else
    .Class = USB_CSCP_NoDeviceClass,
    .SubClass = USB_CSCP_NoDeviceSubclass,
    .Protocol = USB_CSCP_NoDeviceProtocol,
#  endif

    .Endpoint0Size = FIXED_CONTROL_ENDPOINT_SIZE,
    // Specified in config.h
    .VendorID = ALTERNATE_VENDOR_ID,
    .ProductID = ALTERNATE_PRODUCT_ID,
    .ReleaseNumber = DEVICE_VER,
    .ManufacturerStrIndex = 0x01,
    .ProductStrIndex = 0x02,
#  if defined(SERIAL_NUMBER)
    .SerialNumStrIndex = 0x03,
#  else
    .SerialNumStrIndex = 0x00,
#  endif
    .NumberOfConfigurations = FIXED_NUM_CONFIGURATIONS};

const USB_Descriptor_Device_t *get_usb_device_descriptor_ptr() {
  return g_user_config.usb_alternate ? &AlternateDeviceDescriptor : &DeviceDescriptor;
};

#endif
