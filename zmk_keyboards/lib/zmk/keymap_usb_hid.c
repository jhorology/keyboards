/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/device.h>
#include <zephyr/init.h>

#include <zephyr/usb/usb_device.h>
#include <zephyr/usb/class/usb_hid.h>

#include <zmk/usb.h>
#include <zmk/hid.h>
#include <zmk/keymap_hid.h>
#include <zmk/keymap.h>
#include <zmk/event_manager.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

static const struct device *hid_dev;

static K_SEM_DEFINE(hid_sem, 1, 1);

static void in_ready_cb(const struct device *dev) {
    LOG_DBG("");
    k_sem_give(&hid_sem);
}

#ifdef CONFIG_ENABLE_HID_INT_OUT_EP
static void out_ready_cb(const struct device *dev) {
    LOG_DBG("");
    uint8_t data[20];
    uint32_t read;

    hid_int_ep_read(dev, data, 20, &read);

    if (read > 0) {
        LOG_DBG("Got %d bytes", read);
        LOG_HEXDUMP_DBG(data, read, "Got data");
    }
}
#endif

static uint8_t payload[] = {0x42, 0x02};

static int get_report_cb(const struct device *dev, struct usb_setup_packet *setup, int32_t *len,
                         uint8_t **data) {
    LOG_DBG("");
    *data = payload;
    *len = sizeof(payload);
    return 0;
}

static int set_report_cb(const struct device *dev, struct usb_setup_packet *setup, int32_t *len,
                         uint8_t **data) {
    LOG_HEXDUMP_DBG(*data, *len, "REPORT SET");
    return 0;
}

static const struct hid_ops ops = {.get_report = get_report_cb,
                                   .set_report = set_report_cb,
                                   .int_in_ready = in_ready_cb,
#ifdef CONFIG_ENABLE_HID_INT_OUT_EP
                                   .int_out_ready = out_ready_cb
#endif
};

int zmk_keymap_hid_usb_hid_send_report(const uint8_t *report, size_t len) {
    switch (zmk_usb_get_status()) {
    case USB_DC_SUSPEND:
        return usb_wakeup_request();
    case USB_DC_ERROR:
    case USB_DC_RESET:
    case USB_DC_DISCONNECTED:
    case USB_DC_UNKNOWN:
        return -ENODEV;
    default:
        k_sem_take(&hid_sem, K_MSEC(30));
        uint32_t written;
        int err = hid_int_ep_write(hid_dev, report, len, &written);

        if (err) {
            k_sem_give(&hid_sem);
        }

        return err;
    }
}

static void send_work_cb(struct k_work *_work) {
    uint8_t data[] = {KEYMAP_HID_REPORT_ID, 42};

    zmk_keymap_hid_usb_hid_send_report(data, sizeof(data));
}

K_WORK_DEFINE(send_work, send_work_cb);

static void tick_cb() { k_work_submit(&send_work); }

K_TIMER_DEFINE(send_timer, tick_cb, NULL);

static int zmk_keymap_hid_usb_init(void) {
    int ret;

    hid_dev = device_get_binding("HID_1");
    if (hid_dev == NULL) {
        LOG_ERR("Unable to locate HID device");
        return -EINVAL;
    }

    usb_hid_register_device(hid_dev, zmk_keymap_hid_report_desc, sizeof(zmk_keymap_hid_report_desc),
                            &ops);

    ret = usb_hid_init(hid_dev);
    if (ret < 0) {
        LOG_ERR("Failed to init hid device: %d", ret);
        return ret;
    }

    k_timer_start(&send_timer, K_SECONDS(1), K_SECONDS(1));

    return 0;
}

SYS_INIT(zmk_keymap_hid_usb_init, APPLICATION, CONFIG_ZMK_USB_INIT_PRIORITY);
