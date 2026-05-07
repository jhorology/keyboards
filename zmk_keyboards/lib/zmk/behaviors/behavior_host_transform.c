#define DT_DRV_COMPAT zmk_behavior_host_transform

#include <zephyr/device.h>
#include <zephyr/settings/settings.h>
#include <zephyr/kernel.h>

#include <drivers/behavior.h>
#include <zmk/events/keycode_state_changed.h>
#include <zmk/settings.h>
#include <zmk/event_manager.h>
#include <dt-bindings/zmk/host_transform.h>
#include <dt-bindings/zmk/hid_usage_pages.h>
#include <zephyr/logging/log.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#if DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT)

#  define HT_PRIORITY 30

struct ht_config {
  const uint32_t* map;
  size_t map_len;
  bool save_state;
};

struct ht_data {
  uint8_t state;
  bool processing;
  const struct device* dev;
#  if IS_ENABLED(CONFIG_SETTINGS)
  struct k_work_delayable save_work;
#  endif
};

#  if IS_ENABLED(CONFIG_ZMK_BEHAVIOR_METADATA)
static const struct behavior_parameter_value_metadata ht_values[] = {
  {.display_name = "Turn Off", .type = BEHAVIOR_PARAMETER_VALUE_TYPE_VALUE, .value = HT_OFF},
  {.display_name = "Turn On", .type = BEHAVIOR_PARAMETER_VALUE_TYPE_VALUE, .value = HT_ON},
  {.display_name = "Toggle On/Off", .type = BEHAVIOR_PARAMETER_VALUE_TYPE_VALUE, .value = HT_TOG},
};
static const struct behavior_parameter_metadata_set ht_set = {
  .param1_values = ht_values,
  .param1_values_len = ARRAY_SIZE(ht_values),
};
static const struct behavior_parameter_metadata ht_metadata = {
  .sets_len = 1,
  .sets = &ht_set,
};
#  endif

static int ht_remap_keycode(const struct device* dev, const zmk_event_t* eh) {
  struct zmk_keycode_state_changed* ev = as_zmk_keycode_state_changed(eh);
  struct ht_data* data = dev->data;
  if (!ev || data->processing || data->state != HT_ON) return ZMK_EV_EVENT_BUBBLE;

  const struct ht_config* config = dev->config;
  for (size_t i = 0; i < config->map_len; i += 2) {
    if (ev->usage_page == ZMK_HID_USAGE_PAGE(config->map[i]) &&
        ev->keycode == ZMK_HID_USAGE_ID(config->map[i])) {
      data->processing = true;
      struct zmk_keycode_state_changed new_ev = *ev;
      new_ev.usage_page = ZMK_HID_USAGE_PAGE(config->map[i + 1]);
      new_ev.keycode = ZMK_HID_USAGE_ID(config->map[i + 1]);
      raise_zmk_keycode_state_changed(new_ev);
      data->processing = false;
      return ZMK_EV_EVENT_HANDLED;
    }
  }
  return ZMK_EV_EVENT_BUBBLE;
}

static int ht_behavior_pressed(struct zmk_behavior_binding* binding,
                               struct zmk_behavior_binding_event event) {
  const struct device* dev = zmk_behavior_get_binding(binding->behavior_dev);
  struct ht_data* data = dev->data;
  const struct ht_config* config = dev->config;

  uint8_t old_state = data->state;
  switch (binding->param1) {
    case HT_OFF:
      data->state = HT_OFF;
      break;
    case HT_ON:
      data->state = HT_ON;
      break;
    case HT_TOG:
      data->state = data->state == HT_ON ? HT_OFF : HT_ON;
      break;
  }

#  if IS_ENABLED(CONFIG_SETTINGS)
  if (old_state != data->state && config->save_state) {
    int ret = k_work_reschedule(&data->save_work, K_MSEC(CONFIG_ZMK_SETTINGS_SAVE_DEBOUNCE));
    return MIN(ret, 0);
  }
#  endif
  return 0;
}

static const struct behavior_driver_api ht_ctrl_api = {
  .binding_pressed = ht_behavior_pressed,
#  if IS_ENABLED(CONFIG_ZMK_BEHAVIOR_METADATA)
  .parameter_metadata = &ht_metadata,
#  endif
};

#  if IS_ENABLED(CONFIG_SETTINGS)

static void ht_save_state_work(struct k_work* work) {
  struct k_work_delayable* dwork = k_work_delayable_from_work(work);
  struct ht_data* data = CONTAINER_OF(dwork, struct ht_data, save_work);
  char path[64];

  snprintf(path, sizeof(path), "ht/%s", data->dev->name);
  int err = settings_save_one(path, &data->state, sizeof(data->state));
  if (err) {
    LOG_ERR("Failed to save settings [%s]: %d (err %d)", path, data->state, err);
    return;
  }
  LOG_INF("Saved settings [%s]: %d", path, data->state);
}

static int ht_inst_settings_set(const struct device* dev, const char* name, size_t len,
                                settings_read_cb read_cb, void* cb_arg) {
  const struct ht_config* config = dev->config;

  if (!config->save_state) return 0;

  struct ht_data* data = dev->data;
  int rc = read_cb(cb_arg, &data->state, sizeof(data->state));
  if (rc < 0) {
    LOG_ERR("Failed to restore settings [%s]: %d (err %d)", dev->name, data->state, rc);
    return rc;
  }
  LOG_INF("Restored settings [%s]: %d", dev->name, data->state);
  return 0;
}

static int ht_settings_set(const char* name, size_t len, settings_read_cb read_cb, void* cb_arg) {
  const char* next;
  const struct device* dev;

#    define HT_RESTORE_MATCH(n)                                                   \
      IF_ENABLED(DT_INST_PROP(n, save_state),                                     \
                 (dev = DEVICE_DT_GET(DT_DRV_INST(n));                            \
                  if (settings_name_steq(name, dev->name, &next) && !next) {      \
                    return ht_inst_settings_set(dev, name, len, read_cb, cb_arg); \
                  }))
  DT_INST_FOREACH_STATUS_OKAY(HT_RESTORE_MATCH)
  return -ENOENT;
}

SETTINGS_STATIC_HANDLER_DEFINE(ht_settings, "ht", NULL, ht_settings_set, NULL, NULL);

#  endif

static int ht_inst_init(const struct device* dev) {
  const struct ht_config* config = dev->config;
  struct ht_data* data = dev->data;
  data->dev = dev;
  data->state = HT_OFF;
#  if IS_ENABLED(CONFIG_SETTINGS)
  if (config->save_state) {
    k_work_init_delayable(&data->save_work, ht_save_state_work);
  }
#  endif
  return 0;
}

#  define IS_SETTING_ENABLED(n) UTIL_AND(CONFIG_SETTINGS, DT_INST_PROP(n, save_state))
#  define HT_INST(n)                                                                          \
    static const uint32_t ht_map_##n[] = DT_INST_PROP(n, map);                                \
    static const struct ht_config ht_config_##n = {                                           \
      .map = ht_map_##n,                                                                      \
      .map_len = DT_INST_PROP_LEN(n, map),                                                    \
      .save_state = DT_INST_PROP(n, save_state),                                              \
    };                                                                                        \
    static struct ht_data ht_data_##n;                                                        \
    static int ht_remap_keycode_##n(const zmk_event_t* eh) {                                  \
      return ht_remap_keycode(DEVICE_DT_GET(DT_DRV_INST(n)), eh);                             \
    }                                                                                         \
    ZMK_LISTENER(ht_lst_##n, ht_remap_keycode_##n)                                            \
    ZMK_SUBSCRIPTION(ht_lst_##n, zmk_keycode_state_changed)                                   \
                                                                                              \
    BEHAVIOR_DT_INST_DEFINE(n, ht_inst_init, NULL, &ht_data_##n, &ht_config_##n, POST_KERNEL, \
                            HT_PRIORITY, &ht_ctrl_api)

DT_INST_FOREACH_STATUS_OKAY(HT_INST)

#endif
