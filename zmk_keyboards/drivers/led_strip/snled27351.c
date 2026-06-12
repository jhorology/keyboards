// Copyright 2021 @ Keychron (https://www.keychron.com)
// SPDX-License-Identifier: GPL-2.0-or-later
// This file has been modified from the original keychron's QMK source code.

#define DT_DRV_COMPAT sonix_snled27351

#include <string.h>

#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/led_strip.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "snled27351_regs.h"

LOG_MODULE_REGISTER(snled27351, CONFIG_LED_STRIP_LOG_LEVEL);

struct snled27351_config {
  struct i2c_dt_spec i2c;
  size_t chain_length;
  const uint8_t* pixel_channels;
  size_t pixel_channels_len;
};

struct snled27351_data {
  const struct device* dev;
  struct k_work_delayable init_work;
  uint8_t pwm_buffer[SNLED27351_PWM_REGISTER_COUNT];
  bool pwm_buffer_dirty;
  bool initialized;
};

static int snled27351_write_u8(const struct i2c_dt_spec* i2c, uint8_t reg, uint8_t val) {
  return i2c_reg_write_byte_dt(i2c, reg, val);
}

static int snled27351_select_page(const struct i2c_dt_spec* i2c, uint8_t page) {
  return snled27351_write_u8(i2c, SNLED27351_REG_COMMAND, page);
}

static int snled27351_write_pwm_buffer(const struct i2c_dt_spec* i2c, uint8_t* pwm_buffer) {
  int ret;

  for (uint8_t i = 0; i < SNLED27351_PWM_REGISTER_COUNT; i += 16) {
    ret = i2c_burst_write_dt(i2c, i, &pwm_buffer[i], 16);
    if (ret < 0) {
      return ret;
    }
  }

  return 0;
}

static int snled27351_init_chip(const struct device* dev) {
  const struct snled27351_config* cfg = dev->config;
  struct snled27351_data* data = dev->data;
  const struct i2c_dt_spec* i2c = &cfg->i2c;
  int ret;

  if (!i2c_is_ready_dt(i2c)) {
    LOG_ERR("I2C bus not ready");
    return -ENODEV;
  }

  ret = snled27351_select_page(i2c, SNLED27351_COMMAND_FUNCTION);
  if (ret < 0) {
    return ret;
  }

  ret = snled27351_write_u8(i2c, SNLED27351_FUNCTION_REG_SOFTWARE_SHUTDOWN,
                            SNLED27351_SOFTWARE_SHUTDOWN_SSD_SHUTDOWN);
  if (ret < 0) {
    return ret;
  }

  ret =
    snled27351_write_u8(i2c, SNLED27351_FUNCTION_REG_PULLDOWNUP, SNLED27351_PULLDOWNUP_ALL_ENABLED);
  if (ret < 0) {
    return ret;
  }

  ret =
    snled27351_write_u8(i2c, SNLED27351_FUNCTION_REG_SCAN_PHASE, SNLED27351_SCAN_PHASE_12_CHANNEL);
  if (ret < 0) {
    return ret;
  }

  ret = snled27351_write_u8(i2c, SNLED27351_FUNCTION_REG_SLEW_RATE_CONTROL_MODE_1,
                            SNLED27351_SLEW_RATE_CONTROL_MODE_1_PDP_ENABLE);
  if (ret < 0) {
    return ret;
  }

  ret = snled27351_write_u8(i2c, SNLED27351_FUNCTION_REG_SLEW_RATE_CONTROL_MODE_2,
                            SNLED27351_SLEW_RATE_CONTROL_MODE_2_DSL_ENABLE |
                              SNLED27351_SLEW_RATE_CONTROL_MODE_2_SSL_ENABLE);
  if (ret < 0) {
    return ret;
  }

  ret = snled27351_write_u8(i2c, SNLED27351_FUNCTION_REG_SOFTWARE_SLEEP, 0);
  if (ret < 0) {
    return ret;
  }

  ret = snled27351_select_page(i2c, SNLED27351_COMMAND_LED_CONTROL);
  if (ret < 0) {
    return ret;
  }

  for (uint8_t i = 0; i < SNLED27351_LED_CONTROL_REGISTER_COUNT; i++) {
    ret = snled27351_write_u8(i2c, i, 0x00);
    if (ret < 0) {
      return ret;
    }
  }

  ret = snled27351_select_page(i2c, SNLED27351_COMMAND_PWM);
  if (ret < 0) {
    return ret;
  }

  memset(data->pwm_buffer, 0, sizeof(data->pwm_buffer));
  for (uint8_t i = 0; i < SNLED27351_PWM_REGISTER_COUNT; i++) {
    ret = snled27351_write_u8(i2c, i, 0x00);
    if (ret < 0) {
      return ret;
    }
  }

  ret = snled27351_select_page(i2c, SNLED27351_COMMAND_CURRENT_TUNE);
  if (ret < 0) {
    return ret;
  }

  for (uint8_t i = 0; i < SNLED27351_LED_CURRENT_TUNE_LENGTH; i++) {
    ret = snled27351_write_u8(i2c, i, 0xFF);
    if (ret < 0) {
      return ret;
    }
  }

  ret = snled27351_select_page(i2c, SNLED27351_COMMAND_LED_CONTROL);
  if (ret < 0) {
    return ret;
  }

  for (uint8_t i = 0; i < SNLED27351_LED_CONTROL_REGISTER_COUNT; i++) {
    ret = snled27351_write_u8(i2c, i, 0xFF);
    if (ret < 0) {
      return ret;
    }
  }

  ret = snled27351_select_page(i2c, SNLED27351_COMMAND_FUNCTION);
  if (ret < 0) {
    return ret;
  }

  ret = snled27351_write_u8(i2c, SNLED27351_FUNCTION_REG_SOFTWARE_SHUTDOWN,
                            SNLED27351_SOFTWARE_SHUTDOWN_SSD_NORMAL);
  if (ret < 0) {
    return ret;
  }

  data->initialized = true;
  return 0;
}

static int snled27351_flush_pwm(const struct device* dev) {
  const struct snled27351_config* cfg = dev->config;
  struct snled27351_data* data = dev->data;
  const struct i2c_dt_spec* i2c = &cfg->i2c;
  int ret;

  if (!data->pwm_buffer_dirty) {
    return 0;
  }

  ret = snled27351_select_page(i2c, SNLED27351_COMMAND_PWM);
  if (ret < 0) {
    return ret;
  }

  ret = snled27351_write_pwm_buffer(i2c, data->pwm_buffer);
  if (ret < 0) {
    return ret;
  }

  data->pwm_buffer_dirty = false;
  return 0;
}

static int snled27351_update_rgb(const struct device* dev, struct led_rgb* pixels,
                                 size_t num_pixels) {
  const struct snled27351_config* cfg = dev->config;
  struct snled27351_data* data = dev->data;

  if (num_pixels > cfg->chain_length) {
    return -ERANGE;
  }

  if (!data->initialized) {
    return -ENODEV;
  }

  for (size_t i = 0; i < num_pixels; i++) {
    const size_t base = i * 3;
    const uint8_t r_addr = cfg->pixel_channels[base + 0];
    const uint8_t g_addr = cfg->pixel_channels[base + 1];
    const uint8_t b_addr = cfg->pixel_channels[base + 2];

    if (data->pwm_buffer[r_addr] != pixels[i].r || data->pwm_buffer[g_addr] != pixels[i].g ||
        data->pwm_buffer[b_addr] != pixels[i].b) {
      data->pwm_buffer[r_addr] = pixels[i].r;
      data->pwm_buffer[g_addr] = pixels[i].g;
      data->pwm_buffer[b_addr] = pixels[i].b;
      data->pwm_buffer_dirty = true;
    }
  }

  return snled27351_flush_pwm(dev);
}

static size_t snled27351_length(const struct device* dev) {
  const struct snled27351_config* cfg = dev->config;

  return cfg->chain_length;
}

static DEVICE_API(led_strip, snled27351_api) = {
  .update_rgb = snled27351_update_rgb,
  .length = snled27351_length,
};

static void snled27351_init_work_handler(struct k_work* work) {
  struct k_work_delayable* dwork = k_work_delayable_from_work(work);
  struct snled27351_data* data = CONTAINER_OF(dwork, struct snled27351_data, init_work);
  int ret = snled27351_init_chip(data->dev);

  if (ret < 0) {
    LOG_ERR("Deferred chip init failed (%d)", ret);
  }
}

static int snled27351_device_init(const struct device* dev) {
  struct snled27351_data* data = dev->data;

  data->dev = dev;
  k_work_init_delayable(&data->init_work, snled27351_init_work_handler);
  k_work_schedule(&data->init_work, K_SECONDS(2));
  return 0;
}

#define SNLED27351_DEVICE(inst)                                                                  \
  BUILD_ASSERT(DT_INST_PROP_LEN(inst, pixel_channels) == (DT_INST_PROP(inst, chain_length) * 3), \
               "pixel-channels length must equal chain-length * 3");                             \
  static const uint8_t snled27351_pixel_channels_##inst[] = DT_INST_PROP(inst, pixel_channels);  \
  static const struct snled27351_config snled27351_config_##inst = {                             \
    .i2c = I2C_DT_SPEC_INST_GET(inst),                                                           \
    .chain_length = DT_INST_PROP(inst, chain_length),                                            \
    .pixel_channels = snled27351_pixel_channels_##inst,                                          \
    .pixel_channels_len = DT_INST_PROP_LEN(inst, pixel_channels),                                \
  };                                                                                             \
  static struct snled27351_data snled27351_data_##inst;                                          \
  DEVICE_DT_INST_DEFINE(inst, snled27351_device_init, NULL, &snled27351_data_##inst,             \
                        &snled27351_config_##inst, POST_KERNEL, CONFIG_LED_STRIP_INIT_PRIORITY,  \
                        &snled27351_api);

DT_INST_FOREACH_STATUS_OKAY(SNLED27351_DEVICE)
