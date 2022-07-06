/* Copyright 2020 Koichi Katano
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

#include "ciel60.h"

#ifdef APPLE_FN_ENABLE
__attribute__((weak)) bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case APPLE_FN:
            process_apple_fn(keycode, record);
            break;
    }
    return true;
}

void process_apple_fn(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        register_code(KC_APFN);
    } else {
        unregister_code(KC_APFN);
    }
}
#endif
