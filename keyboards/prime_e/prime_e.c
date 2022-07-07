/* Copyright 2022 Holten Campbell
 * Modified 2922 jhorology
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
#include "prime_e.h"

void matrix_init_user(void) {
    // set CapsLock LED to output and low
    setPinOutput(B1);
    writePinLow(B1);
    // #layer LED
    setPinOutput(B2);
    writePinLow(B2);
    setPinOutput(B3);
    writePinLow(B3);
}

void led_set_user(uint8_t usb_led) {
    writePin(B1, IS_LED_ON(usb_led, USB_LED_CAPS_LOCK));
}

// function for layer indicator LED
layer_state_t layer_state_set_user(layer_state_t state) {
    // indicate #layer
    layer_state_t highestLayer = get_highest_layer(state);
    writePin(B2, (highestLayer & 0x01) != 0);
    writePin(B3, (highestLayer & 0x02) != 0);
    return state;
}
