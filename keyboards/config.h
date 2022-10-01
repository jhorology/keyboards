#define DEBOUNCE 5

#define LOCKING_SUPPORT_ENABLE
#define LOCKING_RESYNC_ENABLE

/*
   polling rate
   see https://github.com/qmk/qmk_firmware/blob/master/docs/config_options.md
 */
#define USB_POLLING_INTERVAL_MS 1

#define ALTERNATE_VENDOR_ID 0x05Ac   // Apple
#define ALTERNATE_PRODUCT_ID 0x024f  // ANSI

#ifdef VIAL_ENABLE
/*
  radial controler: 2 bytes
 */
#  define VIA_EEPROM_CUSTOM_CONFIG_SIZE 4
#else
#  define COMBO_COUNT 0
#  define TAP_DANCE_ENTRIES 8
#  define TAPPING_TERM_PER_KEY

/*
  radial controler: 4 bytes
  tap dance : 10 * TAP_DANCE_ENTRIES bytes
 */
#  define VIA_EEPROM_CUSTOM_CONFIG_SIZE 84
#endif
