#pragma once

enum joystick_mode { JOYSTICK, MOUSE, SCROLL };

void zmk_joystick_mode_set(enum joystick_mode);
