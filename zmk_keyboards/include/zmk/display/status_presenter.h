#pragma once

#include <lvgl.h>

void zmk_status_presenter_init(void);
void zmk_status_presenter_dispatch(lv_obj_t *container, uint8_t depth);
