/*
 * Copyright (c) 2024 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#define DT_DRV_COMPAT zmk_input_processor_temp_layer

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <drivers/input_processor.h>
#include <zephyr/logging/log.h>
#include <zmk/keymap.h>
#include <zmk/behavior.h>
#include <zmk/events/position_state_changed.h>
#include <zmk/events/keycode_state_changed.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#define _LAYER_ITEM(node) EMPTY
#define _NUM_LAYERS(...) UTIL_INC(NUM_VA_ARGS_LESS_1(__VA_ARGS__))
#define NUM_LAYERS _NUM_LAYERS(DT_FOREACH_CHILD_SEP(DT_INST(0, zmk_keymap), _LAYER_ITEM, (, )))

struct temp_layer_config {
  int16_t require_prior_idle_ms;
  size_t num_positions;
  const uint16_t excluded_positions[];
};

struct temp_layer_data {
  int64_t last_tapped_timestamp;
  struct k_work_delayable layer_deactivate_works[NUM_LAYERS];
  bool is_activated[NUM_LAYERS];
};

static K_MUTEX_DEFINE(lock);

/* Position Search */
static inline bool position_is_excluded(const struct temp_layer_config *config, uint32_t position) {
  if (config->num_positions == 0) {
    return false;
  }

  for (size_t i = 0; i < config->num_positions; i++) {
    if (config->excluded_positions[i] == position) {
      return true;
    }
  }

  return false;
}

/* Timing Check */
static inline bool should_quick_tap(const struct temp_layer_config *config, int64_t last_tapped,
                                    int64_t current_time) {
  return (last_tapped + config->require_prior_idle_ms) > current_time;
}

/* Layer State Management */
static void layer_activate(const struct device *dev, uint8_t layer_index) {
  struct temp_layer_data *data = dev->data;

  int err = k_mutex_lock(&lock, K_FOREVER);
  if (err < 0) {
    LOG_ERR("Failed to lock mutex. err: %d", err);
    return;
  }

  bool is_active = zmk_keymap_layer_active(layer_index);
  if (!is_active) {
    err = zmk_keymap_layer_activate(layer_index);
    if (err < 0) {
      LOG_ERR("Failed to activate layer %d. err: %d", layer_index, err);
    } else {
      LOG_DBG("Layer %d activated", layer_index);
    }
  }
  data->is_activated[layer_index] = true;

  err = k_mutex_unlock(&lock);
  if (err < 0) {
    LOG_ERR("Failed to unlock mutex. err: %d", err);
  }
}

static inline void layer_deactivate_all(const struct device *dev) {
  struct temp_layer_data *data = dev->data;

  int err = k_mutex_lock(&lock, K_FOREVER);
  if (err < 0) {
    LOG_ERR("Failed to lock mutex. err: %d", err);
    return;
  }

  for (size_t i = 0; i < NUM_LAYERS; i++) {
    if (data->is_activated[i]) {
      bool is_active = zmk_keymap_layer_active(i);
      if (is_active) {
        err = zmk_keymap_layer_deactivate(i);
        if (err < 0) {
          LOG_ERR("Failed to deactivate layer %d. err: %d", i, err);
        } else {
          LOG_DBG("Layer %d deactivated", i);
        }
      }
      data->is_activated[i] = false;
    }
  }

  err = k_mutex_unlock(&lock);
  if (err < 0) {
    LOG_ERR("Failed to unlock mutex. err: %d", err);
  }
}

/* Work Queue Callback */
static void layer_deactivate_work_handler(const struct device *dev, uint8_t layer_index,
                                          struct k_work *work) {
  struct temp_layer_data *data = dev->data;

  int err = k_mutex_lock(&lock, K_FOREVER);
  if (err < 0) {
    LOG_ERR("Failed to lock mutex. err: %d", err);
    return;
  }

  bool is_active = zmk_keymap_layer_active(layer_index);
  if (is_active) {
    err = zmk_keymap_layer_deactivate(layer_index);
    if (err < 0) {
      LOG_ERR("Failed to deactivate layer %d. err: %d", layer_index, err);
    } else {
      LOG_DBG("Layer %d deactivated", layer_index);
    }
  }
  data->is_activated[layer_index] = false;

  err = k_mutex_unlock(&lock);
  if (err < 0) {
    LOG_ERR("Failed to unlock mutex. err: %d", err);
  }
}

/* Driver Implementation */
static int temp_layer_handle_event(const struct device *dev, struct input_event *event,
                                   uint32_t param1, uint32_t param2,
                                   struct zmk_input_processor_state *state) {
  if (param1 >= NUM_LAYERS) {
    LOG_ERR("Invalid layer index: %d", param1);
    return -EINVAL;
  }

  struct temp_layer_data *data = dev->data;
  const struct temp_layer_config *cfg = dev->config;

  if (!should_quick_tap(cfg, data->last_tapped_timestamp, k_uptime_get())) {
    layer_activate(dev, param1);
  }

  if (param2 > 0) {
    k_work_reschedule(&data->layer_deactivate_works[param1], K_MSEC(param2));
  }

  return 0;
}

static int temp_layer_init(const struct device *dev) { return 0; }

/* Driver API */
static const struct zmk_input_processor_driver_api temp_layer_driver_api = {
  .handle_event = temp_layer_handle_event,
};

/* Event Handlers Registration */
#define DT_HAS_EXCLUDED_POSITIONS(n) UTIL_NOT(IS_EQ(DT_INST_PROP_LEN(n, excluded_positions), 0))

#define DT_HAS_REQUIRE_PRIOR_IDLE_MS(n) UTIL_NOT(IS_EQ(DT_INST_PROP(n, require_prior_idle_ms), 0))

#define TEMP_LAYER_EVENT_INIT(n)                                                                   \
  COND_CODE_1(UTIL_OR(DT_HAS_EXCLUDED_POSITIONS(n), DT_HAS_REQUIRE_PRIOR_IDLE_MS(n)),              \
              (static int event_listener_##n(const zmk_event_t *eh) {                              \
                const struct device *dev = DEVICE_DT_INST_GET(n);                                  \
                const struct temp_layer_config *cfg = dev->config;                                 \
                const struct zmk_position_state_changed *psev = as_zmk_position_state_changed(eh); \
                if (psev != NULL) {                                                                \
                  if (position_is_excluded(cfg, psev->position)) {                                 \
                    return ZMK_EV_EVENT_BUBBLE;                                                    \
                  }                                                                                \
                  layer_deactivate_all(dev);                                                       \
                  return ZMK_EV_EVENT_BUBBLE;                                                      \
                }                                                                                  \
                struct temp_layer_data *data = dev->data;                                          \
                const struct zmk_keycode_state_changed *kcev = as_zmk_keycode_state_changed(eh);   \
                if (kcev != NULL) {                                                                \
                  data->last_tapped_timestamp = kcev->timestamp;                                   \
                  return ZMK_EV_EVENT_BUBBLE;                                                      \
                }                                                                                  \
                return ZMK_EV_EVENT_BUBBLE;                                                        \
              }                                                                                    \
                                                                                                   \
               ZMK_LISTENER(processor_temp_layer_##n, event_listener_##n);),                       \
              ())                                                                                  \
                                                                                                   \
  COND_CODE_1(DT_HAS_EXCLUDED_POSITIONS(n),                                                        \
              (ZMK_SUBSCRIPTION(processor_temp_layer_##n, zmk_position_state_changed);), ())       \
                                                                                                   \
  COND_CODE_1(DT_HAS_REQUIRE_PRIOR_IDLE_MS(n),                                                     \
              (ZMK_SUBSCRIPTION(processor_temp_layer_##n, zmk_keycode_state_changed);), ())

#define TEMP_LAYER_WORK_HANDLER(i, n)                       \
  static void work_handler_##n##_##i(struct k_work *work) { \
    const struct device *dev = DEVICE_DT_INST_GET(n);       \
    layer_deactivate_work_handler(dev, i, work);            \
  }

#define TEMP_LAYER_WORK_HANDLERS(n) LISTIFY(NUM_LAYERS, TEMP_LAYER_WORK_HANDLER, (), n)

#define TEMP_LAYER_WORK(i, n) [i] = Z_WORK_DELAYABLE_INITIALIZER(work_handler_##n##_##i)

#define TEMP_LAYER_WORKS(n) LISTIFY(NUM_LAYERS, TEMP_LAYER_WORK, (, ), n)

/* Device Instantiation */
#define TEMP_LAYER_INST(n)                                                                    \
                                                                                              \
  TEMP_LAYER_EVENT_INIT(n)                                                                    \
                                                                                              \
  TEMP_LAYER_WORK_HANDLERS(n)                                                                 \
                                                                                              \
  static struct temp_layer_data data_##n = {.layer_deactivate_works = {TEMP_LAYER_WORKS(n)}}; \
                                                                                              \
  static const struct temp_layer_config config_##n = {                                        \
    .require_prior_idle_ms = DT_INST_PROP(n, require_prior_idle_ms),                          \
    .num_positions = DT_INST_PROP_LEN(n, excluded_positions),                                 \
    .excluded_positions = DT_INST_PROP(n, excluded_positions),                                \
  };                                                                                          \
                                                                                              \
  DEVICE_DT_INST_DEFINE(n, temp_layer_init, NULL, &data_##n, &config_##n, POST_KERNEL,        \
                        CONFIG_KERNEL_INIT_PRIORITY_DEFAULT, &temp_layer_driver_api);

DT_INST_FOREACH_STATUS_OKAY(TEMP_LAYER_INST)
