#pragma once

#include "lib/my_keyboard_common.h"

// custoim keycodes
enum ec_keycodes {
  EC_DBG = CUSTOM_KEYCODES_SAFE_RANGE,  // send calibration data as keystrokes
  RT_TOGG,                              // Toggle Rapd Trigger mode on/off
  RT_ON,                                // Turn on Rapid Trigger Mode, when shfit is held, turn off
  RT_OFF                                // Turn off Rapid Trigger Mode, when shfit is held turn on
};
