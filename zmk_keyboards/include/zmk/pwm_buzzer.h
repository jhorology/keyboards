#pragma once

#include <stdint.h>
#include <zephyr/sys_clock.h>

int zmk_pwm_buzzer_beep(uint32_t cycles, uint32_t pulse, k_timeout_t on_duration,
                        k_timeout_t off_duration);
