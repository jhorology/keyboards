#pragma once

#include <lvgl.h>

void zmk_status_presenter_init(void);
void zmk_status_presenter_register(lv_obj_t *obj, lv_event_code_t event_code);
void zmk_status_presenter_dispatch(lv_obj_t *container);
