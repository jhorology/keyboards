#ifdef __has_include
#  if __has_include("lvgl.h")
#    ifndef LV_LVGL_H_INCLUDE_SIMPLE
#      define LV_LVGL_H_INCLUDE_SIMPLE
#    endif
#  endif
#endif

#if defined(LV_LVGL_H_INCLUDE_SIMPLE)
#  include "lvgl.h"
#else
#  include "lvgl/lvgl.h"
#endif

#ifndef LV_ATTRIBUTE_MEM_ALIGN
#  define LV_ATTRIBUTE_MEM_ALIGN
#endif

#ifndef LV_ATTRIBUTE_IMG_STARMAN_80X80
#  define LV_ATTRIBUTE_IMG_STARMAN_80X80
#endif

const LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST LV_ATTRIBUTE_IMG_STARMAN_80X80 uint8_t
  starman_80x80_map[] = {
    0x00, 0x00, 0x00, 0xfe, /*Color of index 0*/
    0xff, 0xff, 0xff, 0xff, /*Color of index 1*/

    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x05, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xfe, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xeb, 0xdf, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f,
    0x06, 0x80, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x02, 0xc0, 0x3e, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x01, 0xc0, 0x04, 0x40, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x80, 0x04, 0x60,
    0x03, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x0c, 0x60, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x00,
    0x1c, 0x00, 0x08, 0x20, 0x00, 0x70, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x08, 0x20, 0x00, 0x38,
    0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 0x18, 0x30, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x00,
    0x10, 0x10, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x01, 0xc0, 0x00, 0x10, 0x18, 0x00, 0x07, 0x00, 0x00,
    0x00, 0x01, 0x80, 0x00, 0x30, 0x10, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x20, 0x08,
    0x00, 0x01, 0x80, 0x00, 0x00, 0x06, 0x00, 0x00, 0x20, 0x08, 0x00, 0x01, 0xc0, 0x00, 0x00, 0x06,
    0x00, 0x02, 0x60, 0x0c, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x0c, 0x00, 0x0e, 0x40, 0x04, 0x00, 0x00,
    0x60, 0x00, 0x00, 0x0c, 0x00, 0x06, 0x40, 0x04, 0x00, 0x00, 0x60, 0x00, 0x00, 0x18, 0x00, 0x0a,
    0xc0, 0x06, 0x00, 0x00, 0x30, 0x00, 0x00, 0x3d, 0x55, 0x5b, 0x80, 0x07, 0x55, 0x55, 0x7a, 0x00,
    0x00, 0x3f, 0xff, 0xfd, 0x84, 0x03, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x3c, 0x00, 0x0c, 0x04, 0x04,
    0x00, 0x00, 0x38, 0x00, 0x00, 0x36, 0x00, 0x0e, 0x04, 0x04, 0x00, 0x00, 0xf8, 0x00, 0x00, 0x33,
    0x80, 0x06, 0x04, 0x00, 0x00, 0x01, 0x98, 0x00, 0x00, 0x60, 0xc0, 0x07, 0xc4, 0x08, 0x20, 0x03,
    0x08, 0x00, 0x00, 0x30, 0x70, 0x07, 0x84, 0x00, 0x70, 0x06, 0x0c, 0x00, 0x00, 0x60, 0x38, 0x03,
    0xc2, 0x09, 0x70, 0x18, 0x08, 0x00, 0x00, 0x60, 0x0c, 0x01, 0xef, 0x50, 0xc0, 0x30, 0x0c, 0x00,
    0x00, 0x60, 0x06, 0x00, 0xce, 0x20, 0x40, 0xe0, 0x0c, 0x00, 0x00, 0x60, 0x03, 0x80, 0xe4, 0x40,
    0x41, 0x80, 0x0c, 0x00, 0x00, 0x60, 0x00, 0xc1, 0xe0, 0x00, 0x43, 0x00, 0x0c, 0x00, 0x00, 0x60,
    0x00, 0x70, 0xfc, 0x20, 0xa6, 0x00, 0x0c, 0x00, 0x00, 0x60, 0x00, 0x39, 0xfc, 0x08, 0xdc, 0x00,
    0x0c, 0x00, 0x00, 0x20, 0x00, 0x18, 0xfe, 0x02, 0x90, 0x00, 0x0c, 0x00, 0x00, 0x60, 0x00, 0x10,
    0xfe, 0x00, 0x10, 0x00, 0x18, 0x00, 0x00, 0x30, 0x00, 0x10, 0xfe, 0x00, 0x30, 0x00, 0x0c, 0x00,
    0x00, 0x30, 0x00, 0x30, 0xfe, 0x00, 0x18, 0x00, 0x18, 0x00, 0x00, 0x30, 0x00, 0x20, 0xfe, 0x00,
    0x18, 0x00, 0x18, 0x00, 0x00, 0x30, 0x00, 0x20, 0x5e, 0x00, 0x08, 0x00, 0x10, 0x00, 0x00, 0x18,
    0x00, 0x20, 0x52, 0x00, 0x0c, 0x00, 0x38, 0x00, 0x00, 0x18, 0x00, 0x60, 0xf8, 0x00, 0x0c, 0x00,
    0x30, 0x00, 0x00, 0x0c, 0x00, 0x40, 0xf0, 0x00, 0x04, 0x00, 0x30, 0x00, 0x00, 0x0c, 0x00, 0x40,
    0xfc, 0x00, 0x04, 0x00, 0x60, 0x00, 0x00, 0x0c, 0x00, 0xc0, 0xfe, 0x20, 0x06, 0x00, 0x60, 0x00,
    0x00, 0x06, 0x00, 0x80, 0xfe, 0x18, 0x02, 0x00, 0xc0, 0x00, 0x00, 0x03, 0x00, 0x80, 0xfc, 0x1c,
    0x03, 0x01, 0xc0, 0x00, 0x00, 0x03, 0x01, 0x80, 0xdc, 0x06, 0x03, 0x01, 0x80, 0x00, 0x00, 0x01,
    0x81, 0x81, 0xcc, 0x03, 0x81, 0x03, 0x00, 0x00, 0x00, 0x01, 0xc1, 0x07, 0xc8, 0x00, 0xc1, 0x06,
    0x00, 0x00, 0x00, 0x00, 0xe1, 0x0e, 0x67, 0x80, 0x71, 0x8e, 0x00, 0x00, 0x00, 0x00, 0x73, 0x18,
    0x43, 0x00, 0x18, 0x9c, 0x00, 0x00, 0x00, 0x00, 0x3a, 0x70, 0x43, 0x80, 0x0e, 0xf8, 0x00, 0x00,
    0x00, 0x00, 0x1f, 0xc0, 0xc3, 0x00, 0x07, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x07, 0x80, 0xc3, 0x80,
    0x01, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x07, 0x81, 0x83, 0x00, 0x03, 0xc0, 0x00, 0x00, 0x00, 0x00,
    0x05, 0xe1, 0x81, 0x80, 0x0f, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7b, 0x01, 0x80, 0x3c, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x41, 0x83, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06,
    0x7b, 0x9f, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0xbf, 0xdc, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x02, 0x01, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0xc0,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x04, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x01, 0x80, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x30, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

const lv_img_dsc_t starman_80x80 = {
  .header.cf = LV_IMG_CF_INDEXED_1BIT,
  .header.always_zero = 0,
  .header.reserved = 0,
  .header.w = 80,
  .header.h = 80,
  .data_size = 808,
  .data = starman_80x80_map,
};
