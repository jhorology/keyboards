/*
 * Copyright (c) 2020 Rohit Gujarathi
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT sharp_ls0xx_ya

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(ls0xx, CONFIG_DISPLAY_LOG_LEVEL);

#include <string.h>
#include <zephyr/device.h>
#include <zephyr/drivers/display.h>
#include <zephyr/init.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/pm/device.h>
#include <zephyr/pm/device_runtime.h>

/* Supports LS012B7DD01, LS012B7DD06, LS013B7DH03, LS013B7DH05
 * LS013B7DH06, LS027B7DH01A, LS032B7DD02, LS044Q7DH01
 */

/* Note:
 * -> high/1 means white, low/0 means black
 * -> Display expects LSB first
 */

#define LS0XX_PIXELS_PER_BYTE 8U
#define LS0XX_CMD_HOLD 0x0
#define LS0XX_CMD_UPDATE 0x01
#define LS0XX_CMD_CLEAR 0x04

#define LS0XX_BIT_VCOM 0x02

struct ls0xx_config {
  struct spi_dt_spec bus;
  const struct gpio_dt_spec *disp_en_gpio;
  const struct gpio_dt_spec *extcomin_gpio;
  uint16_t width;
  uint16_t height;
  uint8_t com_frequency;
};

struct ls0xx_data {
  bool com_state;
  struct k_mutex lock;
  struct k_thread thread;
  K_KERNEL_STACK_MEMBER(thread_stack, CONFIG_LS0XX_VCOM_THREAD_STACK_SIZE);
};

static inline int _spi_line_data(const struct device *dev, uint16_t start_line, uint16_t num_lines,
                                 const uint8_t *line_data) {
  const struct ls0xx_config *config = dev->config;
  uint8_t ln = start_line;
  uint8_t dummy = 27;
  struct spi_buf line_buf[3] = {
    {
      .len = sizeof(ln),
      .buf = &ln,
    },
    {.len = config->width / LS0XX_PIXELS_PER_BYTE},
    {
      .len = sizeof(dummy),
      .buf = &dummy,
    },
  };
  struct spi_buf_set line_set = {
    .buffers = line_buf,
    .count = ARRAY_SIZE(line_buf),
  };
  int err = 0;

  /* Send each line to the screen including
   * the line number and dummy bits
   */
  for (; ln <= start_line + num_lines - 1; ln++) {
    line_buf[1].buf = (uint8_t *)line_data;
    err |= spi_write_dt(&config->bus, &line_set);
    line_data += config->width / LS0XX_PIXELS_PER_BYTE;
  }

  return err;
}

static int _spi_cmd_data(const struct device *dev, uint8_t cmd, uint16_t start_line,
                         uint16_t num_lines, const uint8_t *line_data) {
  const struct ls0xx_config *config = dev->config;
  struct ls0xx_data *data = dev->data;

  uint8_t cmd_buf[2] = {cmd, 0};
  struct spi_buf spi_cmd_buf = {.buf = cmd_buf};
  struct spi_buf_set spi_cmd_buf_set = {.buffers = &spi_cmd_buf, .count = 1};
  int err = 0;

  if (data->com_state) {
    cmd_buf[0] += LS0XX_BIT_VCOM;
  }

  k_mutex_lock(&data->lock, K_FOREVER);
  switch (cmd) {
    case LS0XX_CMD_HOLD:
      spi_cmd_buf.len = 2;
      err = spi_write_dt(&config->bus, &spi_cmd_buf_set);
      break;
    case LS0XX_CMD_CLEAR:
      spi_cmd_buf.len = 2;
      err = spi_write_dt(&config->bus, &spi_cmd_buf_set);
      break;
    case LS0XX_CMD_UPDATE:
      spi_cmd_buf.len = 1;
      err = spi_write_dt(&config->bus, &spi_cmd_buf_set);

      err |= _spi_line_data(dev, start_line, num_lines, line_data);

      /* Send another trailing 8 bits for the last line
       * These can be any bits, it does not matter
       * just reusing the write_cmd buffer
       */
      err |= spi_write_dt(&config->bus, &spi_cmd_buf_set);
      break;
  }
  spi_release_dt(&config->bus);
  k_mutex_unlock(&data->lock);

  return err;
}

static inline int _spi_cmd(const struct device *dev, uint8_t cmd) {
  return _spi_cmd_data(dev, cmd, 0, 0, NULL);
}

/* Driver will handle VCOM toggling */
static void vcom_toggle_thread(void *arg1, void *arg2, void *arg3) {
  const struct device *dev = arg1;
  const struct ls0xx_config *config = dev->config;
  struct ls0xx_data *data = dev->data;

  if (config->extcomin_gpio != NULL) {
    // EXTMODE
    while (1) {
      gpio_pin_toggle_dt(config->extcomin_gpio);
      k_usleep(3);
      gpio_pin_toggle_dt(config->extcomin_gpio);
      k_msleep(1000 / config->com_frequency);
    }
  } else {
    // none EXTMODE
    while (1) {
      data->com_state = !data->com_state;
      _spi_cmd(dev, LS0XX_CMD_HOLD);
      k_msleep(1000 / config->com_frequency / 2);
    }
  }
}

// driver API -->

static int ls0xx_blanking_on(const struct device *dev) {
  const struct ls0xx_config *config = dev->config;
  if (config->disp_en_gpio != NULL) {
    return gpio_pin_set_dt(config->disp_en_gpio, 0);
  }
  LOG_WRN("Unsupported");
  return -ENOTSUP;
}

static int ls0xx_blanking_off(const struct device *dev) {
  const struct ls0xx_config *config = dev->config;
  if (config->disp_en_gpio != NULL) {
    return gpio_pin_set_dt(config->disp_en_gpio, 1);
  }
  LOG_WRN("Unsupported");
  return -ENOTSUP;
}

/* Buffer width should be equal to display width */
static int ls0xx_write(const struct device *dev, const uint16_t x, const uint16_t y,
                       const struct display_buffer_descriptor *desc, const void *buf) {
  const struct ls0xx_config *config = dev->config;
  LOG_DBG("X: %d, Y: %d, W: %d, H: %d", x, y, desc->width, desc->height);

  if (buf == NULL) {
    LOG_WRN("Display buffer is not available");
    return -EINVAL;
  }

  if (desc->width != config->width) {
    LOG_ERR("Width not a multiple of %d", config->width);
    return -EINVAL;
  }

  if (desc->pitch != desc->width) {
    LOG_ERR("Unsupported mode");
    return -ENOTSUP;
  }

  if ((y + desc->height) > config->height) {
    LOG_ERR("Buffer out of bounds (height)");
    return -EINVAL;
  }

  if (x != 0) {
    LOG_ERR("X-coordinate has to be 0");
    return -EINVAL;
  }

  /* Adding 1 since line numbering on the display starts with 1 */
  return _spi_cmd_data(dev, LS0XX_CMD_UPDATE, y + 1, desc->height, buf);
}

static int ls0xx_read(const struct device *dev, const uint16_t x, const uint16_t y,
                      const struct display_buffer_descriptor *desc, void *buf) {
  LOG_ERR("not supported");
  return -ENOTSUP;
}

static void *ls0xx_get_framebuffer(const struct device *dev) {
  LOG_ERR("not supported");
  return NULL;
}

static int ls0xx_set_brightness(const struct device *dev, const uint8_t brightness) {
  LOG_WRN("not supported");
  return -ENOTSUP;
}

static int ls0xx_set_contrast(const struct device *dev, uint8_t contrast) {
  LOG_WRN("not supported");
  return -ENOTSUP;
}

static void ls0xx_get_capabilities(const struct device *dev, struct display_capabilities *caps) {
  const struct ls0xx_config *config = dev->config;
  memset(caps, 0, sizeof(struct display_capabilities));
  caps->x_resolution = config->width;
  caps->y_resolution = config->height;
  caps->supported_pixel_formats = PIXEL_FORMAT_MONO01;
  caps->current_pixel_format = PIXEL_FORMAT_MONO01;
  caps->screen_info = SCREEN_INFO_X_ALIGNMENT_WIDTH;
}

static int ls0xx_set_orientation(const struct device *dev,
                                 const enum display_orientation orientation) {
  LOG_ERR("Unsupported");
  return -ENOTSUP;
}

static int ls0xx_set_pixel_format(const struct device *dev, const enum display_pixel_format pf) {
  if (pf == PIXEL_FORMAT_MONO01) {
    return 0;
  }

  LOG_ERR("not supported");
  return -ENOTSUP;
}

// <-- driver API

static int ls0xx_init(const struct device *dev) {
  const struct ls0xx_config *config = dev->config;
  struct ls0xx_data *data = dev->data;

  if (!spi_is_ready_dt(&config->bus)) {
    LOG_ERR("SPI bus %s not ready", config->bus.bus->name);
    return -ENODEV;
  }

  if (config->disp_en_gpio != NULL) {
    if (!gpio_is_ready_dt(config->disp_en_gpio)) {
      LOG_ERR("DISP port device not ready");
      return -ENODEV;
    }
    LOG_INF("Configuring DISP pin to OUTPUT_HIGH");
    gpio_pin_configure_dt(config->disp_en_gpio, GPIO_OUTPUT_HIGH);
  }

  if (config->extcomin_gpio != NULL) {
    if (!gpio_is_ready_dt(config->extcomin_gpio)) {
      LOG_ERR("EXTCOMIN port device not ready");
      return -ENODEV;
    }
    LOG_INF("Configuring EXTCOMIN pin");
    gpio_pin_configure_dt(config->extcomin_gpio, GPIO_OUTPUT_LOW);
  }

  k_mutex_init(&data->lock);

  /* Start thread for toggling VCOM */
  k_tid_t tid = k_thread_create(
    &data->thread, data->thread_stack, K_KERNEL_STACK_SIZEOF(data->thread_stack),
    vcom_toggle_thread, (void *)dev, NULL, NULL, CONFIG_LS0XX_VCOM_THREAD_PRIORITY, 0, K_NO_WAIT);
  k_thread_name_set(tid, "ls0xx_vcom");

  /* Clear display else it shows random data */
  return _spi_cmd(dev, LS0XX_CMD_CLEAR);
}

static struct display_driver_api ls0xx_driver_api = {
  .blanking_on = ls0xx_blanking_on,
  .blanking_off = ls0xx_blanking_off,
  .write = ls0xx_write,
  .read = ls0xx_read,
  .get_framebuffer = ls0xx_get_framebuffer,
  .set_brightness = ls0xx_set_brightness,
  .set_contrast = ls0xx_set_contrast,
  .get_capabilities = ls0xx_get_capabilities,
  .set_pixel_format = ls0xx_set_pixel_format,
  .set_orientation = ls0xx_set_orientation,
};

#ifdef CONFIG_PM_DEVICE
static int ls0xx_pm_action(const struct device *dev, enum pm_device_action action) {
  const struct ls0xx_config *config = dev->config;
  struct lx0xx_data *data = dev->data;

  switch (action) {
    case PM_DEVICE_ACTION_SUSPEND:
      // TODO
      break;
    case PM_DEVICE_ACTION_RESUME:
      // TODO
      break;
    default:
      return -ENOTSUP;
  }

  return 0;
}
#endif

#define LS0XX_GPIO_DEF(n)                                                                       \
  COND_CODE_1(                                                                                  \
    DT_INST_NODE_HAS_PROP(n, disp_en_gpios),                                                    \
    (const struct gpio_dt_spec disp_en_gpio_##n = GPIO_DT_SPEC_INST_GET(n, disp_en_gpios)), ()) \
  COND_CODE_1(                                                                                  \
    DT_INST_NODE_HAS_PROP(n, extcomin_gpio),                                                    \
    (const struct gpio_dt_spec extcomin_gpio_##n = GPIO_DT_SPEC_INST_GET(n, extcomin_gpio)), ())

#define LS0XX_CONFIG_DEF(n)                                                               \
  static const struct ls0xx_config ls0xx_config_##n = {                                   \
    .width = DT_INST_PROP(n, width),                                                      \
    .height = DT_INST_PROP(n, height),                                                    \
    .bus = SPI_DT_SPEC_INST_GET(0,                                                        \
                                SPI_OP_MODE_MASTER | SPI_WORD_SET(8) | SPI_TRANSFER_LSB | \
                                  SPI_CS_ACTIVE_HIGH | SPI_HOLD_ON_CS | SPI_LOCK_ON,      \
                                0),                                                       \
    .disp_en_gpio =                                                                       \
      COND_CODE_1(DT_INST_NODE_HAS_PROP(n, disp_en_gpios), (&disp_en_gpio_##n), (NULL)),  \
    .extcomin_gpio =                                                                      \
      COND_CODE_1(DT_INST_NODE_HAS_PROP(n, extcomin_gpio), (&extcomin_gpio_##n), (NULL)), \
    .com_frequency = DT_INST_PROP(n, com_frequency)}

#define LS0XX_INIT(n)                                                                 \
  LS0XX_GPIO_DEF(n);                                                                  \
                                                                                      \
  LS0XX_CONFIG_DEF(n);                                                                \
                                                                                      \
  static struct ls0xx_data ls0xx_data_##n = {};                                       \
                                                                                      \
  PM_DEVICE_DT_INST_DEFINE(n, ls0xx_pm_action);                                       \
                                                                                      \
  DEVICE_DT_INST_DEFINE(n, ls0xx_init, PM_DEVICE_DT_INST_GET(n), &ls0xx_data_##n,     \
                        &ls0xx_config_##n, POST_KERNEL, CONFIG_DISPLAY_INIT_PRIORITY, \
                        &ls0xx_driver_api);

DT_INST_FOREACH_STATUS_OKAY(LS0XX_INIT)
