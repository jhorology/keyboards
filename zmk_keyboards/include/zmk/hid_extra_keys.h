#pragma once

#include <dt-bindings/zmk/hid_usage_pages.h>
#include <zephyr/usb/class/usb_hid.h>

/*
 * CONFIG_ZMK_EXTRA_KEY_n (n=0...7)
 *  value : 0xaaabbbbcccccccc
 *   aaaa: usage page for collection
 *   bbbb: usage for collection if aa != 0
 *         usage page if aa == 0
 *   cccccccc: usage id

 *  keycode in keymap -> 0x00bbcccc
 *
 *  TODO 16bit usage/usage page is not supported
 */
#define _USAGE_PAGE(idx) ZMK_HID_USAGE_PAGE(CONFIG_ZMK_HID_EXTRA_KEY_##idx)
#define _USAGE_ID(idx) ZMK_HID_USAGE_ID(CONFIG_ZMK_HID_EXTRA_KEY_##idx)
#define _C_USAGE_PAGE(idx) (CONFIG_ZMK_HID_EXTRA_KEY_##idx >> 24)
#define _C_USAGE_ID(idx) (_USAGE_PAGE(idx) & 0xff)
#define _HID_USAGE_PAGE(idx) HID_USAGE_PAGE(_USAGE_PAGE(idx))
#define _HID_USAGE(idx) HID_USAGE(_USAGE_ID(idx))
#define _HID_C_USAGE_PAGE(idx) HID_USAGE_PAGE(_C_USAGE_PAGE(idx))
#define _HID_C_USAGE(idx) HID_USAGE(_C_USAGE_ID(idx))
// key defintion -> keycode
#define _KC(idx) (CONFIG_ZMK_HID_EXTRA_KEY_##idx & 0x00ffffff)
#if !CONFIG_ZMK_HID_EXTRA_KEY_0
#  error CONFIG_ZMK_HID_EXTRA_KEY_0 msut be defined.
#endif

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
#          efine _NUM_EXTRA_KEYS 6
#          if CONFIG_ZMK_HID_EXTRA_KEY_6
#            undef _NUM_EXTRA_KEYS
#            define _NUM_EXTRA_KEYS 7
#            if CONFIG_ZMK_HID_EXTRA_KEY_7
#              undef _NUM_EXTRA_KEYS
#              define _NUM_EXTRA_KEYS 8
#            endif
#          endif
#        endif
#      endif
#    endif
#  endif
#endif

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

static inline int zmk_hid_extra_keys_find(uint32_t usage) {
  if (_KC(0) == usage) return 0;
#if _NUM_EXTRA_KEYS > 1
  if (_KC(1) == usage) return 1;
#endif
#if _NUM_EXTRA_KEYS > 2
  if (_KC(2) == usage) return 2;
#endif
#if _NUM_EXTRA_KEYS > 3
  if (_KC(3) == usage) return 3;
#endif
#if _NUM_EXTRA_KEYS > 4
  if (_KC(4) == usage) return 4;
#endif
#if _NUM_EXTRA_KEYS > 5
  if (_KC(5) == usage) return 5;
#endif
#if _NUM_EXTRA_KEYS > 6
  if (_KC(6) == usage) return 6;
#endif
#if _NUM_EXTRA_KEYS > 7
  if (_KC(7) == usage) return 7;
#endif
  return -1;
}

static inline bool zmk_hid_extra_keys_contains_usage_page(uint16_t usage_page) {
  return (_USAGE_PAGE(0) == usage_page)
#if _NUM_EXTRA_KEYS > 1
         || (_USAGE_PAGE(1) == usage_page)
#endif
#if _NUM_EXTRA_KEYS > 2
         || (_USAGE_PAGE(2) == usage_page)
#endif
#if _NUM_EXTRA_KEYS > 3
         || (_USAGE_PAGE(3) == usage_page)
#endif
#if _NUM_EXTRA_KEYS > 4
         || (_USAGE_PAGE(4) == usage_page)
#endif
#if _NUM_EXTRA_KEYS > 5
         || (_USAGE_PAGE(5) == usage_page)
#endif
#if _NUM_EXTRA_KEYS > 6
         || (_USAGE_PAGE(6) == usage_page)
#endif
#if _NUM_EXTRA_KEYS > 7
         || (_USAGE_PAGE(7) == usage_page)
#endif
    ;
}
