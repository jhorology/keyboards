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

#  if IS_ENABLED(CONFIG_ZMK_BEHAVIOR_METADATA)

static const struct behavior_parameter_value_metadata cmd_values[] = {
  {
    .display_name = "Turn On",
    .type = BEHAVIOR_PARAMETER_VALUE_TYPE_VALUE,
    .value = BEEP_ON,
  },
  {
    .display_name = "Turn OFF",
    .type = BEHAVIOR_PARAMETER_VALUE_TYPE_VALUE,
    .value = BEEP_OFF,
  },
  {
    .display_name = "Toggle On/Off",
    .type = BEHAVIOR_PARAMETER_VALUE_TYPE_VALUE,
    .value = BEEP_TOG,
  },
  {
    .display_name = "Beep for Current State",
    .type = BEHAVIOR_PARAMETER_VALUE_TYPE_VALUE,
    .value = BEEP_PLY,
  },
};

static const struct behavior_parameter_metadata_set cmd_set = {
  .param1_values = cmd_values,
  .param1_values_len = ARRAY_SIZE(cmd_values),
};

static const struct behavior_parameter_metadata metadata = {
  .sets_len = 1,
  .sets = &cmd_set,
};

#  endif  // IS_ENABLED(CONFIG_ZMK_BEHAVIOR_METADATA)

static int on_keymap_binding_pressed(struct zmk_behavior_binding *binding,
                                     struct zmk_behavior_binding_event event) {
  switch (binding->param1) {
    case BEEP_ON:
      return zmk_beep_on();
    case BEEP_OFF:
      return zmk_beep_off();
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
#  if IS_ENABLED(CONFIG_ZMK_BEHAVIOR_METADATA)
  .parameter_metadata = &metadata,
#  endif  // IS_ENABLED(CONFIG_ZMK_BEHAVIOR_METADATA)
};

BEHAVIOR_DT_INST_DEFINE(0, behavior_beep_init, NULL, NULL, NULL, POST_KERNEL,
                        CONFIG_KERNEL_INIT_PRIORITY_DEFAULT, &behavior_beep_driver_api);

#endif /* DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT) */
