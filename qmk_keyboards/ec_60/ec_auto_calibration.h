#pragma once
#include <quantum.h>

void ec_auto_calibration_init(void);
void ec_auto_calibration_task(bool all_released);

extern uint16_t bottoming_update_count;
