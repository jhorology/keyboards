/*
 * Copyright (c) 2020 Rohit Gujarathi
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#define DT_DRV_COMPAT sharp_ls0xx_ya

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/display.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/pm/device.h>
#include <zephyr/pm/device_runtime.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(ls0xx, CONFIG_DISPLAY_LOG_LEVEL);

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

#define LS0XX_BIT_DIRTY 0x01

#define LS0XX_FLAG_SUSPEND 0
#define LS0XX_FLAG_BLANKING 1

#define THREAD_SYNC_TIMEOUT K_MSEC(100)

enum screen_rotated { LS0XX_ROT_0, LS0XX_ROT_90, LS0XX_ROT_180, LS0XX_ROT_270 };

struct ls0xx_config {
  struct spi_dt_spec bus;
  const struct gpio_dt_spec *disp_en_gpio;
  const struct gpio_dt_spec *extcomin_gpio;
  uint16_t width;
  uint16_t height;
  uint8_t refresh_rate;
  enum screen_rotated rotated;
  uint16_t line_size;
  uint16_t num_lines;
};

struct ls0xx_data {
  uint8_t vcom_flag;
  struct k_thread thread;
  struct k_timer timer;
  atomic_t flags;
  struct k_sem sync;
  struct k_mutex lock;
#if IS_ENABLED(CONFIG_PM_DEVICE)
  struct k_sem wakeup;
#endif
  uint8_t *buffer;
  struct spi_buf *spi_cmd_buf;

  K_KERNEL_STACK_MEMBER(thread_stack, CONFIG_LS0XX_YA_THREAD_STACK_SIZE);
};

static int _spi_cmd_write(const struct device *dev, uint8_t cmd) {
  const struct ls0xx_config *config = dev->config;
  struct ls0xx_data *data = dev->data;

  uint8_t cmd_buf[2];
  struct spi_buf_set spi_cmd_buf_set = {.buffers = data->spi_cmd_buf};
  int err = 0;

  data->spi_cmd_buf[0].buf = cmd_buf;
  switch (cmd) {
    case LS0XX_CMD_HOLD:
    case LS0XX_CMD_CLEAR:
      cmd_buf[0] = cmd + data->vcom_flag;
      data->spi_cmd_buf[0].len = 2;
      spi_cmd_buf_set.count = 1;
      err = spi_write_dt(&config->bus, &spi_cmd_buf_set);
      break;

    case LS0XX_CMD_UPDATE: {
      uint8_t(*src)[config->line_size] = (uint8_t(*)[config->line_size])data->buffer;
      uint8_t num_blocks = 0;
      uint8_t num_lines = 0;
      uint8_t block_start_line;
      /* lock buffer  */
      k_mutex_lock(&data->lock, K_FOREVER);

      for (uint8_t line = 0; line < config->num_lines; line++) {
        if (src[line][config->line_size - 1] & LS0XX_BIT_DIRTY) {
          LOG_DBG("cmd update line: %d", line);
          if (num_lines == 0) {
            block_start_line = line;
          }
          num_lines++;
          /* flags  */
          src[line][config->line_size - 1] &= ~LS0XX_BIT_DIRTY;
        } else if (num_lines > 0) {
          num_blocks++;
          /* gate-line address + pixel data + dymmy data */
          data->spi_cmd_buf[num_blocks].buf = src[block_start_line];
          data->spi_cmd_buf[num_blocks].len = config->line_size * num_lines;
          num_lines = 0;
        }
      }
      if (num_lines > 0) {
        num_blocks++;
        /* gate-line address + pixel data + dymmy data */
        data->spi_cmd_buf[num_blocks].buf = src[block_start_line];
        data->spi_cmd_buf[num_blocks].len = config->line_size * num_lines;
      }

      if (num_blocks > 0) {
        cmd_buf[0] = cmd + data->vcom_flag;
        data->spi_cmd_buf[0].len = 1;
        spi_cmd_buf_set.count = num_blocks + 1;
        err = spi_write_dt(&config->bus, &spi_cmd_buf_set);
      }

      /* unlock buffer  */
      k_mutex_unlock(&data->lock);

      /* send hold command instead of update command */
      if (num_blocks == 0) {
        cmd_buf[0] = LS0XX_CMD_HOLD + data->vcom_flag;
        data->spi_cmd_buf[0].len = 2;
        spi_cmd_buf_set.count = 1;
        err = spi_write_dt(&config->bus, &spi_cmd_buf_set);
      }
      break;
    }
  }
  spi_release_dt(&config->bus);

  return err;
}

static void _timer_start(const struct device *dev) {
  const struct ls0xx_config *config = dev->config;
  struct ls0xx_data *data = dev->data;
  k_timer_start(&data->timer, K_USEC(USEC_PER_SEC / config->refresh_rate),
                K_USEC(USEC_PER_SEC / config->refresh_rate));
}

static inline int _buffer_rot_0_write(const struct device *dev, const uint16_t x, const uint16_t y,
                                      const struct display_buffer_descriptor *desc,
                                      const void *buf) {
  const struct ls0xx_config *config = dev->config;
  struct ls0xx_data *data = dev->data;

  uint16_t src_x_len = DIV_ROUND_UP(desc->pitch, LS0XX_PIXELS_PER_BYTE);
  uint8_t(*src)[src_x_len] = (uint8_t(*)[src_x_len])buf;
  uint8_t(*dst)[config->line_size] = (uint8_t(*)[config->line_size])data->buffer;

  /* +1 for gate-line address  */
  uint16_t dst_x_offset = x / LS0XX_PIXELS_PER_BYTE + 1;
  uint16_t dst_y = y;
  for (int16_t src_y = 0; src_y < desc->height; src_y++, dst_y++) {
    memcpy(&dst[dst_y][dst_x_offset], src[src_y], src_x_len);
    /* flags */
    dst[dst_y][config->line_size - 1] |= LS0XX_BIT_DIRTY;
  }
  return 0;
}

/*
 * caps->screen_info = SCREEN_INFO_MONO_VTILED
 */
static inline int _buffer_rot_90_write(const struct device *dev, const uint16_t x, const uint16_t y,
                                       const struct display_buffer_descriptor *desc,
                                       const void *buf) {
  const struct ls0xx_config *config = dev->config;
  struct ls0xx_data *data = dev->data;

  uint16_t src_y_len = DIV_ROUND_UP(desc->height, LS0XX_PIXELS_PER_BYTE);
  uint8_t(*src)[desc->width] = (uint8_t(*)[desc->width])buf;
  uint8_t(*dst)[config->line_size] = (uint8_t(*)[config->line_size])data->buffer;

  /* +1 for gate-line address  */
  uint16_t dst_x_offset = y / LS0XX_PIXELS_PER_BYTE + 1;
  uint16_t dst_y = config->width - x - 1;
  for (uint16_t src_x = 0; src_x < desc->width; src_x++, dst_y--) {
    uint16_t dst_x = dst_x_offset;
    for (uint8_t src_y = 0; src_y < src_y_len; src_y++, dst_x++) {
      dst[dst_y][dst_x] = src[src_y][src_x];
    }
    /* flags  */
    dst[dst_y][config->line_size - 1] |= LS0XX_BIT_DIRTY;
  }
  return 0;
}

/*
 * caps->screen_info = SCREEN_INFO_MONO_MSB_FIRST
 */
static inline int _buffer_rot_180_write(const struct device *dev, const uint16_t x,
                                        const uint16_t y,
                                        const struct display_buffer_descriptor *desc,
                                        const void *buf) {
  const struct ls0xx_config *config = dev->config;
  struct ls0xx_data *data = dev->data;

  LOG_DBG("X:%d Y:%d W:%d H:%d P:%d", x, y, desc->width, desc->height, desc->pitch);

  uint16_t src_x_len = DIV_ROUND_UP(desc->pitch, LS0XX_PIXELS_PER_BYTE);
  uint8_t(*src)[src_x_len] = (uint8_t(*)[src_x_len])buf;
  uint8_t(*dst)[config->line_size] = (uint8_t(*)[config->line_size])data->buffer;

  /* -2 for gate-line address and flags, +1 for gate-line address
     (config->line_size - 2) - x / LS0XX_PIXELS_PER_BYTE - 1 + 1
   */
  uint16_t dst_x_offset = config->line_size - x / LS0XX_PIXELS_PER_BYTE - 2;
  uint16_t dst_y = config->height - y - 1;
  for (uint16_t src_y = 0; src_y < desc->height; src_y++, dst_y--) {
    uint16_t dst_x = dst_x_offset;
    for (uint16_t src_x = 0; src_x < src_x_len; src_x++, dst_x--) {
      dst[dst_y][dst_x] = src[src_y][src_x];
    }
    /* flags  */
    dst[dst_y][config->line_size - 1] |= LS0XX_BIT_DIRTY;
  }
  return 0;
}

/*
 *  caps->screen_info = SCREEN_INFO_MONO_VTILED + SCREEN_INFO_MONO_MSB_FIRST;
 */
static inline int _buffer_rot_270_write(const struct device *dev, const uint16_t x,
                                        const uint16_t y,
                                        const struct display_buffer_descriptor *desc,
                                        const void *buf) {
  const struct ls0xx_config *config = dev->config;
  struct ls0xx_data *data = dev->data;

  uint16_t src_y_len = DIV_ROUND_UP(desc->height, LS0XX_PIXELS_PER_BYTE);
  uint8_t(*src)[desc->width] = (uint8_t(*)[desc->width])buf;
  uint8_t(*dst)[config->line_size] = (uint8_t(*)[config->line_size])data->buffer;

  /* -2 for gate-line address and flags, +1 for gate-line address
     (config->line_size - 2) - x / LS0XX_PIXELS_PER_BYTE - 1 + 1
   */
  uint16_t dst_x_offset = config->line_size - y / LS0XX_PIXELS_PER_BYTE - 2;
  uint16_t dst_y = x;
  for (uint16_t src_x = 0; src_x < desc->width; src_x++, dst_y++) {
    uint16_t dst_x = dst_x_offset;
    for (uint8_t src_y = 0; src_y < src_y_len; src_y++, dst_x--) {
      dst[dst_y][dst_x] = src[src_y][src_x];
    }
    /* flags  */
    dst[dst_y][config->line_size - 1] |= LS0XX_BIT_DIRTY;
  }

  return 0;
}

static inline void _buffer_write(const struct device *dev, const uint16_t x, const uint16_t y,
                                 const struct display_buffer_descriptor *desc, const void *buf) {
  const struct ls0xx_config *config = dev->config;
  struct ls0xx_data *data = dev->data;

  /* lock buffer */
  k_mutex_lock(&data->lock, K_FOREVER);

  switch (config->rotated) {
    case LS0XX_ROT_0:
      _buffer_rot_0_write(dev, x, y, desc, buf);
      break;
    case LS0XX_ROT_90:
      _buffer_rot_90_write(dev, x, y, desc, buf);
      break;
    case LS0XX_ROT_180:
      _buffer_rot_180_write(dev, x, y, desc, buf);
      break;
    case LS0XX_ROT_270:
      _buffer_rot_270_write(dev, x, y, desc, buf);
      break;
  }

  /* unlock buffer */
  k_mutex_unlock(&data->lock);
}

static void _buffer_clear(const struct device *dev, bool flush) {
  const struct ls0xx_config *config = dev->config;
  struct ls0xx_data *data = dev->data;

  uint8_t(*dst)[config->line_size] = (uint8_t(*)[config->line_size])data->buffer;

  /* lock buffer  */
  k_mutex_lock(&data->lock, K_FOREVER);

  for (size_t line = 0; line < config->num_lines; line++) {
    /* gate-line address  */
    dst[line][0] = line + 1;

    /*
      PANEL_PIXEL_FORMAT_MONO01  0=Black 1=White
       white should be the default color for LCD
    */
    memset(&dst[line][1], 0xff, config->line_size - 2);

    /* flags  */
    if (flush) {
      dst[line][config->line_size - 1] |= LS0XX_BIT_DIRTY;
    } else {
      dst[line][config->line_size - 1] &= ~LS0XX_BIT_DIRTY;
    }
  }

  /* unlock buffer  */
  k_mutex_unlock(&data->lock);
}

static void _buffer_flush(const struct device *dev) {
  const struct ls0xx_config *config = dev->config;
  struct ls0xx_data *data = dev->data;

  uint8_t(*dst)[config->line_size] = (uint8_t(*)[config->line_size])data->buffer;

  /* lock buffer  */
  k_mutex_lock(&data->lock, K_FOREVER);

  for (int line = 0; line < config->num_lines; line++) {
    /* flags  */
    dst[line][config->line_size - 1] |= LS0XX_BIT_DIRTY;
  }

  /* unlock buffer  */
  k_mutex_unlock(&data->lock);
}

#if IS_ENABLED(CONFIG_PM_DEVICE)

static inline int _suspend(const struct device *dev) {
  const struct ls0xx_config *config = dev->config;
  int err = 0;

  _buffer_clear(dev, false);

  if (config->disp_en_gpio != NULL) {
    err = gpio_pin_set_dt(config->disp_en_gpio, GPIO_OUTPUT_LOW);
    if (err < 0) {
      LOG_ERR("Failed to set disp-en-gpio. err: %d", err);
    }
  } else {
    err = _spi_cmd_write(dev, LS0XX_CMD_CLEAR);
    if (err < 0) {
      LOG_ERR("SPI communication Failed. err: %d", err);
    }
  }
  return err;
}

#endif  // IS_ENABLED(CONFIG_PM_DEVICE)

static int _resume(const struct device *dev) {
  const struct ls0xx_config *config = dev->config;
  int err = 0;

  _buffer_clear(dev, true);

  if (config->disp_en_gpio != NULL) {
    gpio_pin_set_dt(config->disp_en_gpio, GPIO_OUTPUT_HIGH);
    if (err < 0) {
      LOG_ERR("Failed to set disp-en-gpio. err: %d", err);
    }
  }
  return err;
}

// driver API -->

static int ls0xx_blanking_on(const struct device *dev) {
  const struct ls0xx_config *config = dev->config;
  struct ls0xx_data *data = dev->data;

  LOG_DBG("start");

  if (!atomic_test_and_set_bit(&data->flags, LS0XX_FLAG_BLANKING)) {
    if (config->disp_en_gpio != NULL) {
      return gpio_pin_set_dt(config->disp_en_gpio, 0);
    }
  }

  LOG_DBG("end");

  return 0;
}

static int ls0xx_blanking_off(const struct device *dev) {
  const struct ls0xx_config *config = dev->config;
  struct ls0xx_data *data = dev->data;

  LOG_DBG("start");

  if (atomic_test_and_clear_bit(&data->flags, LS0XX_FLAG_BLANKING)) {
    if (config->disp_en_gpio != NULL) {
      return gpio_pin_set_dt(config->disp_en_gpio, 1);
    }
  }

  LOG_DBG("end");

  return 0;
}

/* Buffer width should be equal to display width */
static int ls0xx_write(const struct device *dev, const uint16_t x, const uint16_t y,
                       const struct display_buffer_descriptor *desc, const void *buf) {
  const struct ls0xx_config *config = dev->config;
#if IS_ENABLED(CONFIG_PM_DEVICE)
  struct ls0xx_data *data = dev->data;
  if (atomic_test_bit(&data->flags, LS0XX_FLAG_SUSPEND)) {
    LOG_WRN("Display has been suspended");
    return -EINVAL;
  }
#endif

  LOG_DBG("start X:%d, Y:%d, W:%d, H:%d, L:%d, P:%d, R:%d", x, y, desc->width, desc->height,
          desc->buf_size, desc->pitch, config->rotated);

  _buffer_write(dev, x, y, desc, buf);

/* check ZMK display thread  */
#if CONFIG_DISPLAY_LOG_LEVEL >= LOG_LEVEL_DBG
  size_t unused_stack_space;
  if (k_thread_stack_space_get(k_current_get(), &unused_stack_space) == 0) {
    LOG_DBG("end unused stack space: %zu bytes", unused_stack_space);
  }
#endif
  return 0;
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
  /*
    PANEL_PIXEL_FORMAT_MONO01  0=Black 1=White
    PANEL_PIXEL_FORMAT_MONO10  1=Black 0=White
  */
  caps->current_pixel_format = PIXEL_FORMAT_MONO01;
  switch (config->rotated) {
    case LS0XX_ROT_90:
      caps->screen_info = SCREEN_INFO_MONO_VTILED;
      break;
    case LS0XX_ROT_180:
      caps->screen_info = SCREEN_INFO_MONO_MSB_FIRST;
      break;
    case LS0XX_ROT_270:
      caps->screen_info = SCREEN_INFO_MONO_VTILED + SCREEN_INFO_MONO_MSB_FIRST;
      break;
    default:
      break;
  }
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

/*
   all spi transactions are executed in this thread.
 */
static void ls0xx_thread(void *arg1, void *arg2, void *arg3) {
  const struct device *dev = arg1;
  const struct ls0xx_config *config = dev->config;
  struct ls0xx_data *data = dev->data;

  bool prev_blanking = false;

  if (!atomic_test_bit(&data->flags, LS0XX_FLAG_SUSPEND)) {
    _resume(dev);
  }
  k_sem_give(&data->sync);

  while (1) {
#if IS_ENABLED(CONFIG_PM_DEVICE)
    if (atomic_test_bit(&data->flags, LS0XX_FLAG_SUSPEND)) {
      _suspend(dev);
      k_sem_give(&data->sync);

      LOG_DBG("Suspended");
      k_sem_take(&data->wakeup, K_FOREVER);
      LOG_DBG("Wakeup");

      _resume(dev);
      k_sem_give(&data->sync);
    }
#endif
    if (config->extcomin_gpio != NULL) {
      // EXTMODE

      gpio_pin_toggle_dt(config->extcomin_gpio);
      k_usleep(3);
      gpio_pin_toggle_dt(config->extcomin_gpio);

    } else {
      // none EXTMODE

      data->vcom_flag ^= LS0XX_BIT_VCOM;
    }

    uint8_t cmd = LS0XX_CMD_UPDATE;

    if (config->disp_en_gpio == NULL) {
      if (atomic_test_bit(&data->flags, LS0XX_FLAG_BLANKING)) {
        if (!prev_blanking) {
          LOG_DBG("clear command for blanking");
          cmd = LS0XX_CMD_CLEAR;
        } else {
          cmd = LS0XX_CMD_HOLD;
        }
        prev_blanking = true;
      } else {
        if (prev_blanking) {
          LOG_DBG("buffer full flush for un-blanking");
          _buffer_flush(dev);
        }
        prev_blanking = false;
      }
    }
    int err = _spi_cmd_write(dev, cmd);
    if (err) {
      LOG_ERR("SPI communication Failed. err: %d", err);
    }

#if CONFIG_DISPLAY_LOG_LEVEL >= LOG_LEVEL_DBG
    static uint8_t log_suppress_counter = 0;
    if (log_suppress_counter == 0) {
      size_t unused_stack_space;
      if (k_thread_stack_space_get(k_current_get(), &unused_stack_space) == 0) {
        LOG_DBG("Unused stack space: %zu bytes", unused_stack_space);
      }
    }
    log_suppress_counter++;
#endif

    k_timer_status_sync(&data->timer);
  }
}

static int ls0xx_init(const struct device *dev) {
  const struct ls0xx_config *config = dev->config;
  struct ls0xx_data *data = dev->data;
  int err;

  err = spi_is_ready_dt(&config->bus);
  if (err < 0) {
    LOG_ERR("SPI bus %s not ready. err: %d", config->bus.bus->name, err);
    return err;
  }

  if (config->disp_en_gpio != NULL) {
    err = gpio_is_ready_dt(config->disp_en_gpio);
    if (err < 0) {
      LOG_ERR("DISP port device not ready. err: %d", err);
      return err;
    }
  }

  if (config->extcomin_gpio != NULL) {
    err = gpio_is_ready_dt(config->extcomin_gpio);
    if (err < 0) {
      LOG_ERR("EXTCOMIN port device not ready. err: %d", err);
      return err;
    }
    LOG_INF("Configuring EXTCOMIN pin");
    err = gpio_pin_configure_dt(config->extcomin_gpio, GPIO_OUTPUT_LOW);
    if (err < 0) {
      LOG_ERR("Failed to configure EXTCOMIN port device. err: %d", err);
      return err;
    }
  }

  err = k_mutex_init(&data->lock);
  if (err < 0) {
    LOG_ERR("Failed to initialize mutex. err: %d", err);
    return err;
  }

  k_timer_init(&data->timer, NULL, NULL);

  err = k_sem_init(&data->sync, 0, 1);
  if (err < 0) {
    LOG_ERR("Failed to initialize semaphore. err: %d", err);
    return err;
  }

#if IS_ENABLED(CONFIG_PM_DEVICE)
  err = k_sem_init(&data->wakeup, 0, 1);
  if (err < 0) {
    LOG_ERR("Failed to initialize semaphore. err: %d", err);
    return err;
  }
#endif

#if IS_ENABLED(CONFIG_PM_DEVICE_RUNTIM)

  atomic_set_bit(&data->flags, LS0XX_FLAG_SUSPEND1);
  pm_device_init_suspended(dev);
  err = pm_device_runtime_enable(dev);
  if (err < 0) {
    LOG_ERR("Failed to enable runtime power management");
  }
#else
  _timer_start(dev);
#endif

  /* Start thread for toggling VCOM */
  k_tid_t tid = k_thread_create(
    &data->thread, data->thread_stack, K_KERNEL_STACK_SIZEOF(data->thread_stack), ls0xx_thread,
    (void *)dev, NULL, NULL, CONFIG_LS0XX_YA_THREAD_PRIORITY, 0, K_NO_WAIT);
  k_thread_name_set(tid, "ls0xx_vcom");

  return k_sem_take(&data->sync, THREAD_SYNC_TIMEOUT);
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

#if IS_ENABLED(CONFIG_PM_DEVICE)

// TODO for the device that always powered,
// investigate whether porlarity inversion can be stopped if the
// display is cleared

static int ls0xx_pm_action(const struct device *dev, enum pm_device_action action) {
  struct ls0xx_data *data = dev->data;
  int err = 0;
  switch (action) {
    case PM_DEVICE_ACTION_SUSPEND:
      LOG_DBG("action suspend");
      if (!atomic_test_and_set_bit(&data->flags, LS0XX_FLAG_SUSPEND)) {
        k_timer_stop(&data->timer);
        /*
           At deep-sleep and soft-off, ZMK immediately execute sys_poweroff() after suspend devices.
            So wait here until ls0xx_thread is suspended.
         */
        err = k_sem_take(&data->sync, THREAD_SYNC_TIMEOUT);
      }
      break;
    case PM_DEVICE_ACTION_RESUME:
      LOG_DBG("action resume");
      if (atomic_test_and_clear_bit(&data->flags, LS0XX_FLAG_SUSPEND)) {
        _timer_start(dev);
        k_sem_give(&data->wakeup);
        err = k_sem_take(&data->sync, THREAD_SYNC_TIMEOUT);
      }
      break;
    default:
      return -ENOTSUP;
  }

  return err;
}
#endif

/* if roatated 90 or 270 degrees */
#define IS_WH_SWAPPED(n) \
  UTIL_OR(IS_EQ(DT_INST_ENUM_IDX(n, rotated), 1), IS_EQ(DT_INST_ENUM_IDX(n, rotated), 3))

/*
 *  +2 for gate-line address and dummy data
 *  +0                +1                       +DIV_ROUND_UP(pixel_size, 8) + 1
 *   |gate-line address|<-----pixel data------>|flags(dummy data for LS0XX)|
 */
#define BUFFER_LINE_SIZE(n)                                                                      \
  (COND_CODE_1(IS_WH_SWAPPED(n), (DIV_ROUND_UP(DT_INST_PROP(n, height), LS0XX_PIXELS_PER_BYTE)), \
               (DIV_ROUND_UP(DT_INST_PROP(n, width), LS0XX_PIXELS_PER_BYTE))) +                  \
   2)

#define BUFFER_NUM_LINES(n) \
  COND_CODE_1(IS_WH_SWAPPED(n), (DT_INST_PROP(n, width)), (DT_INST_PROP(n, height)))

#define LS0XX_GPIO_DEF(n)                                                                        \
  COND_CODE_1(                                                                                   \
    DT_INST_NODE_HAS_PROP(n, disp_en_gpios),                                                     \
    (const struct gpio_dt_spec disp_en_gpio_##n = GPIO_DT_SPEC_INST_GET(n, disp_en_gpios);), ()) \
  COND_CODE_1(                                                                                   \
    DT_INST_NODE_HAS_PROP(n, extcomin_gpio),                                                     \
    (const struct gpio_dt_spec extcomin_gpio_##n = GPIO_DT_SPEC_INST_GET(n, extcomin_gpio);), ())

#define LS0XX_CONFIG_DEF(n)                                                               \
  static const struct ls0xx_config ls0xx_config_##n = {                                   \
    .width = DT_INST_PROP(n, width),                                                      \
    .height = DT_INST_PROP(n, height),                                                    \
    .bus = SPI_DT_SPEC_INST_GET(n,                                                        \
                                SPI_OP_MODE_MASTER | SPI_WORD_SET(8) | SPI_TRANSFER_LSB | \
                                  SPI_CS_ACTIVE_HIGH | SPI_HOLD_ON_CS | SPI_LOCK_ON,      \
                                0),                                                       \
    .disp_en_gpio =                                                                       \
      COND_CODE_1(DT_INST_NODE_HAS_PROP(n, disp_en_gpios), (&disp_en_gpio_##n), (NULL)),  \
    .extcomin_gpio =                                                                      \
      COND_CODE_1(DT_INST_NODE_HAS_PROP(n, extcomin_gpio), (&extcomin_gpio_##n), (NULL)), \
    .refresh_rate = DT_INST_PROP(n, refresh_rate),                                        \
    .rotated = DT_INST_ENUM_IDX(n, rotated),                                              \
    .line_size = BUFFER_LINE_SIZE(n),                                                     \
    .num_lines = BUFFER_NUM_LINES(n),                                                     \
  }

#define BUFFER_DEF(n)                                                         \
  static uint8_t ls0xx_buffer_##n[BUFFER_NUM_LINES(n) * BUFFER_LINE_SIZE(n)]; \
  static struct spi_buf ls0xx_spi_cmd_buf_##n[DIV_ROUND_UP(BUFFER_NUM_LINES(n), 2) + 1]

#define LS0XX_INIT(n)                                                                 \
  LS0XX_GPIO_DEF(n);                                                                  \
                                                                                      \
  LS0XX_CONFIG_DEF(n);                                                                \
                                                                                      \
  BUFFER_DEF(n);                                                                      \
                                                                                      \
  static struct ls0xx_data ls0xx_data_##n = {                                         \
    .buffer = ls0xx_buffer_##n,                                                       \
    .spi_cmd_buf = ls0xx_spi_cmd_buf_##n,                                             \
  };                                                                                  \
                                                                                      \
  PM_DEVICE_DT_INST_DEFINE(n, ls0xx_pm_action);                                       \
                                                                                      \
  DEVICE_DT_INST_DEFINE(n, ls0xx_init, PM_DEVICE_DT_INST_GET(n), &ls0xx_data_##n,     \
                        &ls0xx_config_##n, POST_KERNEL, CONFIG_DISPLAY_INIT_PRIORITY, \
                        &ls0xx_driver_api);

DT_INST_FOREACH_STATUS_OKAY(LS0XX_INIT)
