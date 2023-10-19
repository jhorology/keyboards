#pragma once

#include <quantum.h>

/* AppleVendor Page Top Case (0x00ff) */
enum {
  kHIDUsage_AV_TopCase_KeyboardFn = 0x0003,  // index 0
  kHIDUsage_AV_TopCase_BrightnessUp = 0x0004,
  kHIDUsage_AV_TopCase_KeyboardLayoutSwitch = 0x0004,
  kHIDUsage_AV_TopCase_BrightnessDown = 0x0005,
  kHIDUsage_AV_TopCase_VideoMirror = 0x0006,
  kHIDUsage_AV_TopCase_IlluminationToggle = 0x0007,
  kHIDUsage_AV_TopCase_IlluminationUp = 0x0008,    // index 1
  kHIDUsage_AV_TopCase_IlluminationDown = 0x0009,  // index 2
  kHIDUsage_AV_TopCase_ClamshellLatched = 0x000A,
  kHIDUsage_AV_TopCase_Reserved_MouseData = 0x00C0
};

/* AppleVendor Keyboard Page (0xff01) */
enum {
  kHIDUsage_AppleVendorKeyboard_Spotlight = 0x0001,
  kHIDUsage_AppleVendorKeyboard_Dashboard = 0x0002,
  kHIDUsage_AppleVendorKeyboard_Function = 0x0003,
  kHIDUsage_AppleVendorKeyboard_Launchpad = 0x0004,
  kHIDUsage_AppleVendorKeyboard_Reserved = 0x000a,
  kHIDUsage_AppleVendorKeyboard_CapsLockDelayEnable = 0x000b,
  kHIDUsage_AppleVendorKeyboard_PowerState = 0x000c,
  kHIDUsage_AppleVendorKeyboard_Expose_All = 0x0010,
  kHIDUsage_AppleVendorKeyboard_Expose_Desktop = 0x0011,
  kHIDUsage_AppleVendorKeyboard_Brightness_Up = 0x0020,
  kHIDUsage_AppleVendorKeyboard_Brightness_Down = 0x0021,
  kHIDUsage_AppleVendorKeyboard_Language = 0x0030
};

/* Bit field order in HID report descriptor */
enum {
  /* AppleVendor Top Case Page (0xff) */
  USAGE_INDEX_AVT_KEYBOARD_FN = 0,
  USAGE_INDEX_AVT_ILLUMINATION_UP,
  USAGE_INDEX_AVT_ILLUMINATION_DOWN,

  /* AppleVendor Keyboard Page (0xff01) */
  USAGE_INDEX_AVK_SPOTLIGHT,
  USAGE_INDEX_AVK_DASHBOARD,
  USAGE_INDEX_AVK_FUNCTION,
  USAGE_INDEX_AVK_LAUNCHPAD,
  USAGE_INDEX_AVK_RESERVED,
  USAGE_INDEX_AVK_CAPS_LOCK_DELAY_ENABLE,
  USAGE_INDEX_AVK_EXPOSE_POWER_STATE,
  USAGE_INDEX_AVK_EXPOSE_ALL,
  USAGE_INDEX_AVK_EXPOSE_DESKTOP,
  USAGE_INDEX_AVK_BRIGHTNESS_UP,
  USAGE_INDEX_AVK_BRIGHTNESS_DOWN,
  USAGE_INDEX_AVK_LANGUAGE
};

typedef enum non_mac_fn_key {
  FN_F1 = 0,
  FN_F2,
  FN_F3,
  FN_F4,
  FN_F5,
  FN_F6,
  FN_F7,
  FN_F8,
  FN_F9,
  FN_F10,
  FN_F11,
  FN_F12,
  FN_SPC,   // Hey Siri
  FN_Q,     // Qick Notes
  FN_E,     // Emoji & Symbols
  FN_A,     // Focus Dock
  FN_D,     // Start Dictation
  FN_F,     // Toggle full screen mode
  FN_H,     // Show Desktop
  FN_C,     // Show Control Cnecter
  FN_N,     // Show Notification
  FN_M,     // Focus Menubar
  FN_BSPC,  // Delete
  FN_UP,
  FN_DOWN,
  FN_LEFT,
  FN_RIGHT,
  NON_MAC_FN_KEY_ENTRIES,  // num of total keys
  FN_UNKNOWN = 0xfff
  // TOTAL 27 keys
} non_mac_fn_key_t;

bool process_apple_fn(uint16_t keycode, keyrecord_t *record);
