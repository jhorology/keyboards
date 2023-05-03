#pragma once

#include <quantum.h>

/* AppleVendor Page Top Case (0x00ff) */
enum {
  kHIDUsage_AV_TopCase_KeyboardFn = 0x0003,  // index 0
  kHIDUsage_AV_TopCase_BrightnessUp = 0x0004,
  kHIDUsage_AV_TopCase_KeyboardLayoutSwitch = 0x0004,  // use in output report on DFR virtual keyboard
  kHIDUsage_AV_TopCase_BrightnessDown = 0x0005,
  kHIDUsage_AV_TopCase_VideoMirror = 0x0006,  // index 1
  kHIDUsage_AV_TopCase_IlluminationToggle = 0x0007,
  kHIDUsage_AV_TopCase_IlluminationUp = 0x0008,
  kHIDUsage_AV_TopCase_IlluminationDown = 0x0009,
  kHIDUsage_AV_TopCase_ClamshellLatched = 0x000A,  // index 2
  kHIDUsage_AV_TopCase_Reserved_MouseData = 0x00C0
};

/* AppleVendor Keyboard Page (0xff01) */
enum {
  kHIDUsage_AppleVendorKeyboard_Spotlight = 0x0001,  // index 3
  kHIDUsage_AppleVendorKeyboard_Dashboard = 0x0002,  // index 4
  kHIDUsage_AppleVendorKeyboard_Function = 0x0003,   // index 5
  kHIDUsage_AppleVendorKeyboard_Launchpad = 0x0004,  // index 6
  kHIDUsage_AppleVendorKeyboard_Reserved = 0x000A,
  kHIDUsage_AppleVendorKeyboard_CapsLockDelayEnable = 0x000B,
  kHIDUsage_AppleVendorKeyboard_PowerState = 0x000C,
  kHIDUsage_AppleVendorKeyboard_Expose_All = 0x0010,      // index 7
  kHIDUsage_AppleVendorKeyboard_Expose_Desktop = 0x0011,  // index 8
  kHIDUsage_AppleVendorKeyboard_Brightness_Up = 0x0020,
  kHIDUsage_AppleVendorKeyboard_Brightness_Down = 0x0021,
  kHIDUsage_AppleVendorKeyboard_Language = 0x0030,  // index 9

  /* Synthesized Keyboard Events */
  kHIDUsage_AppleVendorKeyboard_WillReset = 0x0040,
  kHIDUsage_AppleVendorKeyboard_Reset = 0x0041,
  kHIDUsage_AppleVendorKeyboard_WillFactoryReset = 0x0042,
  kHIDUsage_AppleVendorKeyboard_AccessibilityToggle = 0x0043,
};

/* Bit field order in HID report descriptor */
enum {
  /* AppleVendor Top Case Page (0xff) */
  USAGE_INDEX_APPLE_KEYBOARD_FN = 0,
  USAGE_INDEX_APPLE_VIDEO_MIRROR,
  USAGE_INDEX_APPLE_CLAMSHELL_LATCHED,

  /* AppleVendor Keyboard Page (0xff01) */
  USAGE_INDEX_APPLE_SPOTLIGHT,
  USAGE_INDEX_APPLE_DASHBOARD,
  USAGE_INDEX_APPLE_FUNCTION,
  USAGE_INDEX_APPLE_LAUHCNPAD,
  USAGE_INDEX_APPLE_EXPOSE_ALL,
  USAGE_INDEX_APPLE_EXPOSE_DESKTOP,
  USAGE_INDEX_APPLE_LANGUAGE,
};

#define REPORT_MASK_APPLE_KEYBOARD_FN (1 << USAGE_INDEX_APPLE_KEYBOARD_FN)
#define REPORT_MASK_APPLE_VIDEO_MIRROR (1 << USAGE_INDEX_APPLE_BIDEO_MIRROR)
#define REPORT_MASK_APPLE_CLAMSHELL_LATCHED (1 << USAGE_INDEX_CLAM_SHELL_LATCHED)
#define REPORT_MASK_APPLE_SPOTLIGHT (1 << USAGE_INDEX_APPLE_SPOTLIGHT)
#define REPORT_MASK_APPLE_DASHBOARD (1 << USAGE_INDEX_APPLE_DASHBOARD)
#define REPORT_MASK_APPLE_FUNCTION (1 << USAGE_INDEX_APPLE_FUNCTION)
#define REPORT_MASK_APPLE_LAUHCNPAD (1 << USAGE_INDEX_APPLE_LAUHCNPAD)
#define REPORT_MASK_APPLE_EXPOSE_ALL (1 << USAGE_INDEX_APPLE_EXPOSE_ALL)
#define REPORT_MASK_APPLE_EXPOSE_DESKTOP (1 << USAGE_INDEX_APPLE_EXPOSE_DESKTOP)
#define REPORT_MASK_APPLE_LANGUAGE (1 << USAGE_INDEX_APPLE_LANGUAGE)

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
  FN_SPC,                  // Hey Siri
  FN_Q,                    // Qick Notes
  FN_E,                    // Emoji & Symbols
  FN_A,                    // Focus Dock
  FN_D,                    // Start Dictation
  FN_F,                    // Toggle full screen mode
  FN_H,                    // Show Desktop
  FN_C,                    // Show Control Cnecter
  FN_N,                    // Show Notification
  FN_M,                    // Focus Menubar
  FN_BSPC,                 // Delete
  NON_MAC_FN_KEY_ENTRIES,  // num of total keys
  FN_UNKNOWN = 0xfff
  // TOTAL 22 keys
} non_mac_fn_key_t;

bool process_apple_fn(uint16_t keycode, keyrecord_t *record);
