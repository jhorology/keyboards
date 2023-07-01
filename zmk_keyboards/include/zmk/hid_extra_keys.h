#pragma once

#include <dt-bindings/zmk/hid_usage_pages.h>
#include <zephyr/sys/util_macro.h>
#include <zephyr/usb/class/usb_hid.h>

/*
 * CONFIG_ZMK_EXTRA_KEY_n (n=0...7)
 *  value : 0xaabbcccc
 *   aa: usage page for collection
 *   bb: usage for collection if aa != 0
 *       usage page if aa == 0
 *   cccc: usage id
 *
 *  keycode in keymap -> 0x00bbcccc
 *
 *  TODO 16bit usage/usage page is not supported.
 *
 * Example:
 *
 *   CONFIG_EXTRA_KEY_0=0x00ff0003
 *   CONFIG_EXTRA_KEY_1=0x00ff0004
 *   CONFIG_EXTRA_KEY_2=0x0190009b
 *   CONFIG_EXTRA_KEY_3=0x0190009c
 *   CONFIG_EXTRA_KEY_4=0x0
 *   CONFIG_EXTRA_KEY_5=0x0
 *   CONFIG_EXTRA_KEY_6=0x0
 *   CONFIG_EXTRA_KEY_7=0x0
 *
 * above definitions expand into report descriptor as a subsititute for reserved 8bit input part:
 *
 *   0x05, 0xFF,        //   Usage Page (Reserved 0xFF)
 *   0x09, 0x03,        //   Usage (0x03)
 *   0x09, 0x03,        //   Usage (0x04)
 *   0x75, 0x01,        //   Report Size (1)
 *   0x95, 0x01,        //   Report Count (2)
 *   0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
 *   0x05, 0x01,        //   Usage Page (Generic Desktop Ctrls)
 *   0x09, 0x80,        //   Usage (Sys Control)
 *   0xA1, 0x02,        //   Collection (Logical)
 *   0x09, 0x9B,        //     Usage (0x9B)
 *   0x09, 0x9B,        //     Usage (0x9C)
 *   0x15, 0x00,        //     Logical Minimum (0)
 *   0x25, 0x01,        //     Logical Maximum (1)
 *   0x75, 0x01,        //     Report Size (1)
 *   0x95, 0x01,        //     Report Count (2)
 *   0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
 *   0xC0,              //   End Collection
 *   0x75, 0x06,        //   Report Size (4)
 *   0x95, 0x01,        //   Report Count (1)
 *   0x81, 0x03,        //   Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
 */

#if !CONFIG_ZMK_HID_EXTRA_KEY_0
#  error CONFIG_ZMK_HID_EXTRA_KEY_0 msut be defined.
#endif

// key definition
#define _USAGE_PAGE(idx) (CONFIG_ZMK_HID_EXTRA_KEY_##idx >> 16)
#define _USAGE_ID(idx) ZMK_HID_USAGE_ID(CONFIG_ZMK_HID_EXTRA_KEY_##idx)
// Usage Page for Colletion(Logical)
#define _C_USAGE_PAGE(idx) (_USAGE_PAGE(idx) >> 8)
// Usage for Colletion(Logical)
#define _C_USAGE_ID(idx) (_USAGE_PAGE(idx) & 0xff)
// USAGE_PAGE descriptor
#define _HID_USAGE_PAGE(idx) HID_USAGE_PAGE(_C_USAGE_ID(idx))
// USAGE descriptor
#define _HID_USAGE(idx) HID_USAGE(_USAGE_ID(idx))
// USAGE_PAGE descriptor for Colletion(Logical)
#define _HID_C_USAGE_PAGE(idx) HID_USAGE_PAGE(_C_USAGE_PAGE(idx))
// USAGE descriptor for Colletion(Logical)
#define _HID_C_USAGE(idx) HID_USAGE(_C_USAGE_ID(idx))
// keycode
#define _EXTRA_KEY_CODE(idx) (CONFIG_ZMK_HID_EXTRA_KEY_##idx & 0x00ffffff)
#define _EXTRA_KEY_USAGE_PAGE(idx) (_EXTRA_KEY_CODE(idx) >> 16)

/* TODO cleanup */

#define _NUM_EXTRA_KEYS 1
#if CONFIG_ZMK_HID_EXTRA_KEY_1
#  undef _NUM_EXTRA_KEYS
#  define _NUM_EXTRA_KEYS 2
#  if CONFIG_ZMK_HID_EXTRA_KEY_2
#    undef _NUM_EXTRA_KEYS
#    define _NUM_EXTRA_KEYS 3
#    if CONFIG_ZMK_HID_EXTRA_KEY_3
#      undef _NUM_EXTRA_KEYS
#      define _NUM_EXTRA_KEYS 4
#      if CONFIG_ZMK_HID_EXTRA_KEY_4
#        undef _NUM_EXTRA_KEYS
#        define _NUM_EXTRA_KEYS 5
#        if CONFIG_ZMK_HID_EXTRA_KEY_5
#          undef _NUM_EXTRA_KEYS
#          define _NUM_EXTRA_KEYS 6
#          if CONFIG_ZMK_HID_EXTRA_KEY_6
#            undef _NUM_EXTRA_KEYS
#            define _NUM_EXTRA_KEYS 7
#            if CONFIG_ZMK_HID_EXTRA_KEY_7
#              undef _NUM_EXTRA_KEYS
#              define _NUM_EXTRA_KEYS 8
#            endif  // CONFIG_ZMK_HID_EXTRA_KEY_7
#          endif    // CONFIG_ZMK_HID_EXTRA_KEY_6
#        endif      // CONFIG_ZMK_HID_EXTRA_KEY_5
#      endif        // CONFIG_ZMK_HID_EXTRA_KEY_4
#    endif          // CONFIG_ZMK_HID_EXTRA_KEY_3
#  endif            // CONFIG_ZMK_HID_EXTRA_KEY_2
#endif              // CONFIG_ZMK_HID_EXTRA_KEY_1

#define _START_COLLECTION(idx) \
  _HID_C_USAGE_PAGE(idx), _HID_C_USAGE(idx), HID_COLLECTION(HID_COLLECTION_LOGICAL), _HID_USAGE(idx)
#define _START_PAGE(idx) _HID_USAGE_PAGE(idx), _HID_USAGE(idx)

#define _END_COLLECTION(c)                                                                                     \
  HID_LOGICAL_MIN8(0x00), HID_LOGICAL_MAX8(0x01), HID_REPORT_SIZE(0x01), HID_REPORT_COUNT(c), HID_INPUT(0x02), \
    HID_END_COLLECTION
#define _END_PAGE(c) HID_REPORT_SIZE(0x01), HID_REPORT_COUNT(c), HID_INPUT(0x02)

// EXTRA_KEY_0
#if _C_USAGE_PAGE(0)
#  define _HID_EXTRA_KEY_0_s _START_COLLECTION(0)
#else
#  define _HID_EXTRA_KEY_0_s _START_PAGE(0)
#endif
#if (_NUM_EXTRA_KEYS > 1 && _USAGE_PAGE(0) == _USAGE_PAGE(1))
#  define _HID_EXTRA_KEY_0 _HID_EXTRA_KEY_0_s
#else
#  if _C_USAGE_PAGE(0)
#    define _HID_EXTRA_KEY_0 _HID_EXTRA_KEY_0_s, _END_COLLECTION(1)
#  else
#    define _HID_EXTRA_KEY_0 _HID_EXTRA_KEY_0_s, _END_PAGE(1)
#  endif
#  if _NUM_EXTRA_KEYS == 1
#    define _HID_EXTRA_KEYS _HID_EXTRA_KEY_0
#  endif
#endif

// EXTRA_KEY_1
#if _NUM_EXTRA_KEYS > 1
#  if _USAGE_PAGE(0) == _USAGE_PAGE(1)
#    define _HID_EXTRA_KEY_1_S _HID_EXTRA_KEY_0, _HID_USAGE(1)
#    define _REPORT_COUNT_1 2
#  else
#    if _C_USAGE_PAGE(1)
#      define _HID_EXTRA_KEY_1_S _HID_EXTRA_KEY_0, _START_COLLECTION(1)
#    else
#      define _HID_EXTRA_KEY_1_S _HID_EXTRA_KEY_0, _START_PAGE(1)
#    endif
#    define _REPORT_COUNT_1 1
#  endif
#  if _NUM_EXTRA_KEYS > 2 && _USAGE_PAGE(1) == _USAGE_PAGE(2)
#    define _HID_EXTRA_KEY_1 _HID_EXTRA_KEY_1_S
#  else
#    if _C_USAGE_PAGE(1)
#      define _HID_EXTRA_KEY_1 _HID_EXTRA_KEY_1_S, _END_COLLECTION(_REPORT_COUNT_1)
#    else
#      define _HID_EXTRA_KEY_1 _HID_EXTRA_KEY_1_S, _END_PAGE(_REPORT_COUNT_1)
#    endif
#    if _NUM_EXTRA_KEYS == 2
#      define _HID_EXTRA_KEYS _HID_EXTRA_KEY_1
#    endif
#  endif
#endif

// EXTRA_KEY_2
#if _NUM_EXTRA_KEYS > 2
#  if _USAGE_PAGE(1) == _USAGE_PAGE(2)
#    define _HID_EXTRA_KEY_2_S _HID_EXTRA_KEY_1, _HID_USAGE(2)
#    define _REPORT_COUNT_2 (_REPORT_COUNT_1 + 1)
#  else
#    if _C_USAGE_PAGE(2) != 0
#      define _HID_EXTRA_KEY_2_S _HID_EXTRA_KEY_1, _START_COLLECTION(2)
#    else
#      define _HID_EXTRA_KEY_2_S _HID_EXTRA_KEY_1, _START_PAGE(2)
#    endif
#    define _REPORT_COUNT_2 1
#  endif
#  if _NUM_EXTRA_KEYS > 3 && _USAGE_PAGE(2) == _USAGE_PAGE(3)
#    define _HID_EXTRA_KEY_2 _HID_EXTRA_KEY_2_S
#  else
#    if _C_USAGE_PAGE(2)
#      define _HID_EXTRA_KEY_2 _HID_EXTRA_KEY_2_S, _END_COLLECTION(_REPORT_COUNT_2)
#    else
#      define _HID_EXTRA_KEY_2 _HID_EXTRA_KEY_2_S, _END_PAGE(_REPORT_COUNT_2)
#    endif
#    if _NUM_EXTRA_KEYS == 3
#      define _HID_EXTRA_KEYS _HID_EXTRA_KEY_2
#    endif
#  endif
#endif

// EXTRA_KEY_3
#if _NUM_EXTRA_KEYS > 3
#  if _USAGE_PAGE(2) == _USAGE_PAGE(3)
#    define _HID_EXTRA_KEY_3_S _HID_EXTRA_KEY_2, _HID_USAGE(3)
#    define _REPORT_COUNT_3 (_REPORT_COUNT_2 + 1)
#  else
#    if _C_USAGE_PAGE(3)
#      define _HID_EXTRA_KEY_3_S _HID_EXTRA_KEY_2, _START_COLLECTION(3)
#    else
#      define _HID_EXTRA_KEY_3_S _HID_EXTRA_KEY_2, _START_PAGE(3)
#    endif
#    define _REPORT_COUNT_3 1
#  endif
#  if _NUM_EXTRA_KEYS > 4 && _USAGE_PAGE(3) == _USAGE_PAGE(4)
#    define _HID_EXTRA_KEY_3 _HID_EXTRA_KEY_3_S
#  else
#    if _C_USAGE_PAGE(3)
#      define _HID_EXTRA_KEY_3 _HID_EXTRA_KEY_3_S, _END_COLLECTION(_REPORT_COUNT_3)
#    else
#      define _HID_EXTRA_KEY_3 _HID_EXTRA_KEY_3_S, _END_PAGE(_REPORT_COUNT_3)
#    endif
#    if _NUM_EXTRA_KEYS == 4
#      define _HID_EXTRA_KEYS _HID_EXTRA_KEY_3
#    endif
#  endif
#endif

// EXTRA_KEY_4
#if _NUM_EXTRA_KEYS > 4
#  if _USAGE_PAGE(3) == _USAGE_PAGE(4)
#    define _HID_EXTRA_KEY_4_S _HID_EXTRA_KEY_3, _HID_USAGE(4)
#    define _REPORT_COUNT_4 (_REPORT_COUNT_3 + 1)
#  else
#    if _C_USAGE_PAGE(4)
#      define _HID_EXTRA_KEY_4_S _HID_EXTRA_KEY_3, _START_COLLECTION(4)
#    else
#      define _HID_EXTRA_KEY_4_S _HID_EXTRA_KEY_3, _START_PAGE(4)
#    endif
#    define _REPORT_COUNT_4 1
#  endif
#  if _NUM_EXTRA_KEYS > 5 && _USAGE_PAGE(4) == _USAGE_PAGE(5)
#    define _HID_EXTRA_KEY_4 _HID_EXTRA_KEY_4_S
#  else
#    if _C_USAGE_PAGE(4)
#      define _HID_EXTRA_KEY_4 _HID_EXTRA_KEY_4_S, _END_COLLECTION(_REPORT_COUNT_4)
#    else
#      define _HID_EXTRA_KEY_4 _HID_EXTRA_KEY_4_S, _END_PAGE(_REPORT_COUNT_4)
#    endif
#    if _NUM_EXTRA_KEYS == 5
#      define _HID_EXTRA_KEYS _HID_EXTRA_KEY_4
#    endif
#  endif
#endif

// EXTRA_KEY_5
#if _NUM_EXTRA_KEYS > 5
#  if _USAGE_PAGE(4) == _USAGE_PAGE(5)
#    define _HID_EXTRA_KEY_5_S _HID_EXTRA_KEY_4, _HID_USAGE(5)
#    define _REPORT_COUNT_5 (_REPORT_COUNT_4 + 1)
#  else
#    if _C_USAGE_PAGE(5)
#      define _HID_EXTRA_KEY_5_S _HID_EXTRA_KEY_4, _START_COLLECTION(5)
#    else
#      define _HID_EXTRA_KEY_5_S _HID_EXTRA_KEY_4, _START_PAGE(5)
#    endif
#    define _REPORT_COUNT_5 1
#  endif
#  if _NUM_EXTRA_KEYS > 6 && _USAGE_PAGE(5) == _USAGE_PAGE(6)
#    define _HID_EXTRA_KEY_5 _HID_EXTRA_KEY_5_S
#  else
#    if _C_USAGE_PAGE(5)
#      define _HID_EXTRA_KEY_5 _HID_EXTRA_KEY_5_S, _END_COLLECTION(_REPORT_COUNT_5)
#    else
#      define _HID_EXTRA_KEY_5 _HID_EXTRA_KEY_5_S, _END_PAGE(_REPORT_COUNT_5)
#    endif
#    if _NUM_EXTRA_KEYS == 6
#      define _HID_EXTRA_KEYS _HID_EXTRA_KEY_5
#    endif
#  endif
#endif

// EXTRA_KEY_6
#if _NUM_EXTRA_KEYS > 6
#  if _USAGE_PAGE(5) == _USAGE_PAGE(6)
#    define _HID_EXTRA_KEY_6_S _HID_EXTRA_KEY_5, _HID_USAGE(6)
#    define _REPORT_COUNT_6 (_REPORT_COUNT_5 + 1)
#  else
#    if _C_USAGE_PAGE(6)
#      define _HID_EXTRA_KEY_6_S _HID_EXTRA_KEY_5, _START_COLLECTION(6)
#    else
#      define _HID_EXTRA_KEY_6_S _HID_EXTRA_KEY_5, _START_PAGE(6)
#    endif
#    define _REPORT_COUNT_6 1
#  endif
#  if _NUM_EXTRA_KEYS > 7 && _USAGE_PAGE(6) == _USAGE_PAGE(7)
#    define _HID_EXTRA_KEY_6 _HID_EXTRA_KEY_6_S
#  else
#    if _C_USAGE_PAGE(7)
#      define _HID_EXTRA_KEY_6 _HID_EXTRA_KEY_6_S, _END_COLLECTION(_REPORT_COUNT_6)
#    else
#      define _HID_EXTRA_KEY_6 _HID_EXTRA_KEY_6_S, _END_PAGE(_REPORT_COUNT_6)
#    endif
#    if _NUM_EXTRA_KEYS == 7
#      define _HID_EXTRA_KEYS _HID_EXTRA_KEY_6
#    endif
#  endif
#endif

// EXTRA_KEY_7
#if _NUM_EXTRA_KEYS > 7
#  if _USAGE_PAGE(6) == _USAGE_PAGE(7)
#    define _HID_EXTRA_KEY_7_S _HID_EXTRA_KEY_6, _HID_USAGE(7)
#    define _REPORT_COUNT_7 (_REPORT_COUNT_6 + 1)
#  else
#    if _C_USAGE_PAGE(7)
#      define _HID_EXTRA_KEY_7_S _HID_EXTRA_KEY_6, _START_COLLECTION(7)
#    else
#      define _HID_EXTRA_KEY_7_S _HID_EXTRA_KEY_6, _START_PAGE(7)
#    endif
#    define _REPORT_COUNT_7 1
#  endif
#  if _C_USAGE_PAGE(7)
#    define _HID_EXTRA_KEYS _HID_EXTRA_KEY_7_S, _END_COLLECTION(_REPORT_COUNT_7)
#  else
#    define _HID_EXTRA_KEYS _HID_EXTRA_KEY_7_S, _END_PAGE(_REPORT_COUNT_7)
#  endif
#endif

#if _NUM_EXTRA_KEYS < 8
#  define HID_EXTRA_KEYS_DESC \
    _HID_EXTRA_KEYS, HID_REPORT_SIZE(8 - _NUM_EXTRA_KEYS), HID_REPORT_COUNT(0x01), HID_INPUT(0x03)
#else
#  define HID_EXTRA_KEYS_DESC _HID_EXTRA_KEYS
#endif

#define _EXTRA_KEY_MATCH_CODE(idx, keycode) \
  if (_EXTRA_KEY_CODE(idx) == keycode) return idx
static inline int zmk_hid_extra_keys_find(uint32_t keycode) {
  LISTIFY(_NUM_EXTRA_KEYS, _EXTRA_KEY_MATCH_CODE, (;), keycode);
  return -1;
}

#define _EXTRA_KEY_ANY_USAGE_PAGE(idx, usage_page) (_EXTRA_KEY_USAGE_PAGE(idx) == usage_page)
static inline bool zmk_hid_extra_keys_contains_usage_page(uint16_t usage_page) {
  return LISTIFY(_NUM_EXTRA_KEYS, _EXTRA_KEY_ANY_USAGE_PAGE, (||), usage_page);
}
