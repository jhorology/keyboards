/*
  qmk config options
  see https://github.com/qmk/qmk_firmware/blob/master/docs/config_options.md
*/
#define LOCKING_SUPPORT_ENABLE
#define LOCKING_RESYNC_ENABLE
#define USB_POLLING_INTERVAL_MS 1
#define FORCE_NKRO
#define TAPPING_TERM_PER_KEY

// The default behavior of mod-taps will change to mimic IGNORE_MOD_TAP_INTERRUPT in the future.
#define HOLD_ON_OTHER_KEY_PRESS

/* custom options */

// Apple Aluminium Keyboard (ANSI)
#define ALTERNATE_VENDOR_ID 0x05Ac   // Apple
#define ALTERNATE_PRODUCT_ID 0x024f  // ANSI

// Magic Keyboard with touch id
// #define ALTERNATE_VENDOR_ID 0x004c   // Apple
// #define ALTERNATE_PRODUCT_ID 0x029a  // ANSI
#ifndef TAP_DANCE_ENTRIES
#  define TAP_DANCE_ENTRIES 8
#endif
// #define CUSTOM_CONFIG_FORCE_RHID false
// #define CUSTOM_CONFIG_FORCE_USJ false

#define MAC_BASE_LAYER 0
#define NON_MAC_BASE_LAYER 1

// use modern mac function keys
// F4 : spotlight
// F5 : dictation
// F6 : Do not disturb
#define APPLE_FN_OVERRIDE_F456

/* eeprom address */

#define VIA_RADIAL_CONTROLLER_EEPROM_OFFSET 0
#ifdef RADIAL_CONTROLLER_ENABLE
#  define VIA_RADIAL_CONTROLLER_EEPROM_SIZE 4
#else
#  define VIA_RADIAL_CONTROLLER_EEPROM_SIZE 0
#endif

#define VIA_DYNAMIC_NON_MAC_FN_EEPROM_OFFSET \
  (VIA_RADIAL_CONTROLLER_EEPROM_OFFSET + VIA_RADIAL_CONTROLLER_EEPROM_SIZE)
#define VIA_DYNAMIC_NON_MAC_FN_EEPROM_SIZE 56  // 27 keycodes + 2 bytes

#define VIA_DYNAMIC_TAP_DANCE_EEPROM_OFFSET \
  (VIA_DYNAMIC_NON_MAC_FN_EEPROM_OFFSET + VIA_DYNAMIC_NON_MAC_FN_EEPROM_SIZE)
#define VIA_DYNAMIC_TAP_DANCE_EEPROM_SIZE (TAP_DANCE_ENTRIES * 10)  // 10 bytes per tap dance entr

// reserved
#define VIA_EEPROM_CUSTOM_CONFIG_COMMON_SIZE                                \
  (VIA_RADIAL_CONTROLLER_EEPROM_SIZE + VIA_DYNAMIC_NON_MAC_FN_EEPROM_SIZE + \
   VIA_DYNAMIC_TAP_DANCE_EEPROM_SIZE)
#define VIA_EEPROM_CUSTOM_CONFIG_USER_OFFSET VIA_EEPROM_CUSTOM_CONFIG_COMMON_SIZE

/* maybe needed to re-define in each keyboard  */
#ifndef VIA_EEPROM_CUSTOM_CONFIG_SIZE
#  define VIA_EEPROM_CUSTOM_CONFIG_SIZE VIA_EEPROM_CUSTOM_CONFIG_COMMON_SIZE
#endif
