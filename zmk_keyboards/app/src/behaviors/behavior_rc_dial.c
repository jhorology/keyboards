/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#define DT_DRV_COMPAT zmk_behavior_rc_dial

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <drivers/behavior.h>

#include <zmk/behavior.h>
#include <zmk/hid.h>
#include <zmk/endpoints.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#if DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT)
struct behavior_rc_dial_config {
  int degrees_per_click_x10;
};

static int behavior_rc_dial_init(const struct device *dev) { return 0; }

static int on_keymap_binding_pressed(struct zmk_behavior_binding *binding,
                                     struct zmk_behavior_binding_event event) {
  const struct device *dev = device_get_binding(binding->behavior_dev);
  const struct behavior_rc_dial_config *cfg = dev->config;
  LOG_DBG("radial controller dial rotate %d (x10)degrees", cfg->degrees_per_click_x10);
  zmk_hid_radial_controller_dial_rotate(cfg->degrees_per_click_x10);
  int ret = zmk_endpoints_send_radial_controller_report();
  zmk_hid_radial_controller_dial_rotate(0);
  return ret;
}

static const struct behavior_driver_api behavior_rc_dial_driver_api = {.binding_pressed =
                                                                         on_keymap_binding_pressed};

#  define RC_DIAL_INST(n)                                                                       \
    static const struct behavior_rc_dial_config behavior_rc_dial_config_##n = {                 \
      .degrees_per_click_x10 = DT_INST_PROP(n, degrees_per_click_x10)};                         \
    BEHAVIOR_DT_INST_DEFINE(n, behavior_rc_dial_init, NULL, NULL, &behavior_rc_dial_config_##n, \
                            POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT,                   \
                            &behavior_rc_dial_driver_api);

DT_INST_FOREACH_STATUS_OKAY(RC_DIAL_INST)

#endif /* DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT) */
