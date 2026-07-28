#pragma once

#include <lvgl.h>

#ifndef LV_IMG_CF_ALPHA_1BIT
#define LV_IMG_CF_ALPHA_1BIT LV_COLOR_FORMAT_A1
#endif
#ifndef LV_IMG_CF_INDEXED_1BIT
#define LV_IMG_CF_INDEXED_1BIT LV_COLOR_FORMAT_I1
#endif

void zmk_status_presenter_init(void);
void zmk_status_presenter_dispatch(lv_obj_t *container, uint8_t depth);

