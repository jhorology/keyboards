#include "lvgl.h"

#ifndef LV_ATTRIBUTE_MEM_ALIGN
#  define LV_ATTRIBUTE_MEM_ALIGN
#endif

#ifndef LV_ATTRIBUTE_IMG_ZMKLOGO
#  define LV_ATTRIBUTE_IMG_ZMKLOGO
#endif

const LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST LV_ATTRIBUTE_IMG_ZMKLOGO uint8_t
  zmklogo_map[] = {
    0xff, 0xff, 0xff, 0xff, /*Color of index 0*/
    0x00, 0x00, 0x00, 0xfe, /*Color of index 1*/

    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xf0, 0x0e, 0x7e, 0x73, 0xc7, 0xe0, 0x04, 0x3c, 0x31, 0x87, 0xf0, 0x0c, 0x18, 0x31, 0x0f,
    0xff, 0x0c, 0x18, 0x30, 0x1f, 0xff, 0x1c, 0x00, 0x30, 0x3f, 0xfe, 0x3c, 0x00, 0x30, 0x7f, 0xfe,
    0x3c, 0x42, 0x30, 0xff, 0xfc, 0x7c, 0x66, 0x30, 0xff, 0xf8, 0x7c, 0x7e, 0x30, 0x7f, 0xf8, 0xfc,
    0x7e, 0x30, 0x3f, 0xf0, 0xfc, 0x7e, 0x30, 0x1f, 0xf0, 0x0c, 0x7e, 0x31, 0x0f, 0xe0, 0x04, 0x7e,
    0x31, 0x87, 0xf0, 0x0e, 0xff, 0x73, 0xc7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
};

const lv_img_dsc_t zmklogo = {
  .header.cf = LV_IMG_CF_INDEXED_1BIT,
  .header.always_zero = 0,
  .header.reserved = 0,
  .header.w = 40,
  .header.h = 40,
  .data_size = 208,
  .data = zmklogo_map,
};