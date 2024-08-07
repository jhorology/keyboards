/* Copyright (c) 2021 Megamind (megamind4089)
 * Copyright (c) 2021 The ZMK Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/pwm.h>

#include <zephyr/bluetooth/services/bas.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/ble.h>
#include <zmk/event_manager.h>
#include <zmk/endpoints.h>
#include <zmk/endpoints_types.h>
#include <zmk/events/ble_active_profile_changed.h>
#include <zmk/events/endpoint_changed.h>

#define BUZZER_NODE DT_ALIAS(buzzer)

#if !DT_NODE_HAS_STATUS(BUZZER_NODE, okay)
#error "Unsupported board: buzzer devicetree alias is not defined"
#else

#endif

#define BEEP_DURATION K_MSEC(60)

static const struct pwm_dt_spec pwm = PWM_DT_SPEC_GET(BUZZER_NODE);

static void _play(uint32_t period) {
    if (!device_is_ready(pwm.dev)) {
        printk("Error: PWM device %s is not ready\n", pwm.dev->name);
        return;
    }
    pwm_set_dt(&pwm, period, period / 2U); // attempt at new implementation
    k_sleep(BEEP_DURATION);
    pwm_set_dt(&pwm, 0, 0); // attempt at new implementation
    k_sleep(K_MSEC(50));
}

static void play_sound_ble_0() {
    _play(1000000);
    _play(500000);
    _play(250000);
    _play(100000);
    _play(50000);
}

static void play_sound_ble_1() {
    _play(1500000);
    _play(3900000);
    _play(1500000);
    _play(1500000);
}

static void play_sound_ble_2() {
    _play(1500000);
    _play(3900000);
}

static void play_sound_ble_3() {
    _play(2000000);
    _play(3900000);
}

static void play_sound_ble_4() {
    _play(2500000);
    _play(3900000);
}

static void play_sound_usb() {
    _play(3000000);
    _play(1500000);
    _play(750000);
}

static void play_sound(uint8_t index) {
    switch (index) {
    case 0:
        play_sound_ble_0();
        break;
    case 1:
        play_sound_ble_1();
        break;
    case 2:
        play_sound_ble_2();
        break;
    case 3:
        play_sound_ble_3();
        break;
    case 4:
        play_sound_ble_4();
        break;
    case 5:
        play_sound_usb();
        break;
    default:
        break;
    }
}

int buzzer_listener(const zmk_event_t *eh) {
    static int index = -1;
    int new_index = -1;
    switch (zmk_endpoints_selected().transport) {
    case ZMK_TRANSPORT_USB:
        new_index = 5;
        break;
    case ZMK_TRANSPORT_BLE:
        new_index = zmk_ble_active_profile_index();
        break;
    }
    if (new_index != index) {
        play_sound(new_index);
        index = new_index;
    }
    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(buzzer_output_status, buzzer_listener)
ZMK_SUBSCRIPTION(buzzer_output_status, zmk_ble_active_profile_changed);
ZMK_SUBSCRIPTION(buzzer_output_status, zmk_endpoint_changed);
