/* Copyright 2022 zhaqian
 * Modified 2022 masasfumi
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include QMK_KEYBOARD_H

#include "radial_controller.h"

#include "custom_config.h"
#include "custom_keycodes.h"

static report_radial_controller_t radial_controller_report;

bool process_radial_controller(uint16_t keycode, keyrecord_t *record) {
  switch (keycode) {
    case DIAL_BUT:
      radial_controller_report.button = record->event.pressed;
      radial_controller_report.dial = 0;
      host_radial_controller_send(&radial_controller_report);
      return false;
    case DIAL_L:
      if (record->event.pressed) {
        radial_controller_report.dial = -custom_config_get_rc_deg_per_click();
        host_radial_controller_send(&radial_controller_report);
        radial_controller_report.dial = 0;
      }
      return false;
    case DIAL_R:
      if (record->event.pressed) {
        radial_controller_report.dial = custom_config_get_rc_deg_per_click();
        host_radial_controller_send(&radial_controller_report);
        radial_controller_report.dial = 0;
      }
      return false;
  }
  return true;
}
