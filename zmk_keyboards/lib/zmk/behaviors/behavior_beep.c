/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#define DT_DRV_COMPAT zmk_behavior_beep

#include <drivers/behavior.h>

#include <zmk/beep.h>

#include <dt-bindings/zmk/beep.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#if DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT)

static int on_keymap_binding_pressed(struct zmk_behavior_binding *binding,
                                     struct zmk_behavior_binding_event event) {
  switch (binding->param1) {
    case BEEP_OFF:
      return zmk_beep_off();
    case BEEP_ON:
      return zmk_beep_on();
    case BEEP_TOG:
      if (zmk_beep_is_on())
        return zmk_beep_off();
      else
        return zmk_beep_on();
    case BEEP_PLY:
      return zmk_beep_play();
    default:
      LOG_ERR("Unknown beep command: %d", binding->param1);
  }

  return -ENOTSUP;
}

static int on_keymap_binding_released(struct zmk_behavior_binding *binding,
                                      struct zmk_behavior_binding_event event) {
  return ZMK_BEHAVIOR_OPAQUE;
}

static int behavior_beep_init(const struct device *dev) { return 0; };

static const struct behavior_driver_api behavior_beep_driver_api = {
  .binding_pressed = on_keymap_binding_pressed,
  .binding_released = on_keymap_binding_released,
  .locality = BEHAVIOR_LOCALITY_GLOBAL,
};

BEHAVIOR_DT_INST_DEFINE(0, behavior_beep_init, NULL, NULL, NULL, POST_KERNEL,
                        CONFIG_KERNEL_INIT_PRIORITY_DEFAULT, &behavior_beep_driver_api);

#endif /* DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT) */
