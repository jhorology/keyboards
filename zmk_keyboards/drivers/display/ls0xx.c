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

enum screen_rotated { LS0XX_ROT_0, LS0XX_ROT_90, LS0XX_ROT_180, LS0XX_ROT_270 };

struct ls0xx_config {
  struct spi_dt_spec bus;
  const struct gpio_dt_spec *disp_en_gpio;
  const struct gpio_dt_spec *extcomin_gpio;
  uint16_t width;
  uint16_t height;
  uint8_t com_frequency;
  enum screen_rotated rotated;
  uint16_t line_size;
  uint16_t num_lines;
};

struct ls0xx_data {
  uint8_t vcom_flag;
  struct k_thread thread;
  struct k_timer timer;
  atomic_t blanking;
#if IS_ENABLED(CONFIG_PM_DEVICE)
  atomic_t suspended;
  struct k_sem wakeup;
#endif
  struct k_mutex lock;
  uint8_t *buffer;
  bool *dirty;
  K_KERNEL_STACK_MEMBER(thread_stack, CONFIG_LS0XX_THREAD_STACK_SIZE);
};

static inline uint8_t _reverse_bits(uint8_t d) {
  d = ((d & 0x0f) << 4) | ((d >> 4) & 0x0f);
  d = ((d & 0x33) << 2) | ((d >> 2) & 0x33);
  d = ((d & 0x55) << 1) | ((d >> 1) & 0x55);
  return d;
}

static int _spi_cmd_write(const struct device *dev, uint8_t cmd) {
  const struct ls0xx_config *config = dev->config;
  struct ls0xx_data *data = dev->data;

  uint8_t cmd_buf[2];
  /* TODO maybe better to move data structure or use heap memory,
     pay attenstion to stack size
  */
  struct spi_buf spi_cmd_buf[DIV_ROUND_UP(config->num_lines, 2) + 1];
  struct spi_buf_set spi_cmd_buf_set = {.buffers = spi_cmd_buf};
  int err = 0;

  spi_cmd_buf[0].buf = cmd_buf;

  switch (cmd) {
    case LS0XX_CMD_HOLD:
    case LS0XX_CMD_CLEAR:
      cmd_buf[0] = cmd + data->vcom_flag;
      spi_cmd_buf[0].len = 2;
      spi_cmd_buf_set.count = 1;
      err = spi_write_dt(&config->bus, &spi_cmd_buf_set);
      break;

    case LS0XX_CMD_UPDATE: {
      uint8_t num_blocks = 0;
      uint8_t num_lines = 0;
      uint8_t block_start_line;

      /* lock buffer  */
      k_mutex_lock(&data->lock, K_FOREVER);

      for (uint8_t line = 0; line < config->num_lines; line++) {
        if (data->dirty[line]) {
          LOG_DBG("cmd update line: %d", line);
          if (num_lines == 0) {
            block_start_line = line;
          }
          num_lines++;
          data->dirty[line] = false;
        } else if (num_lines > 0) {
          num_blocks++;
          /* gate-line address + pixel data + dymmy data */
          spi_cmd_buf[num_blocks].buf = &data->buffer[config->line_size * block_start_line];
          spi_cmd_buf[num_blocks].len = config->line_size * num_lines;
          num_lines = 0;
        }
      }
      if (num_lines > 0) {
        num_blocks++;
        /* gate-line address + pixel data + dymmy data */
        spi_cmd_buf[num_blocks].buf = &data->buffer[config->line_size * block_start_line];
        spi_cmd_buf[num_blocks].len = config->line_size * num_lines;
      }

      if (num_blocks > 0) {
        cmd_buf[0] = cmd + data->vcom_flag;
        spi_cmd_buf[0].len = 1;
        spi_cmd_buf_set.count = num_blocks + 1;
        err = spi_write_dt(&config->bus, &spi_cmd_buf_set);
      }

      /* unlock buffer  */
      k_mutex_unlock(&data->lock);

      /* send hold command instead of update command */
      if (num_blocks == 0) {
        cmd_buf[0] = LS0XX_CMD_HOLD + data->vcom_flag;
        spi_cmd_buf[0].len = 2;
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
  k_timer_start(&data->timer, K_USEC(USEC_PER_SEC / config->com_frequency / 2),
                K_USEC(USEC_PER_SEC / config->com_frequency / 2));
}

static inline int _buffer_rot_0_write(const struct device *dev, const uint16_t x, const uint16_t y,
                                      const struct display_buffer_descriptor *desc,
                                      const void *buf) {
  const struct ls0xx_config *config = dev->config;
  struct ls0xx_data *data = dev->data;

  uint8_t *src = (uint8_t *)buf;
  uint16_t src_line_size = DIV_ROUND_UP(desc->pitch, LS0XX_PIXELS_PER_BYTE);
  /* +1 for gate-line address  */
  uint8_t *dst = &data->buffer[config->line_size * y + x / LS0XX_PIXELS_PER_BYTE + 1];
  int16_t line_max_exclusive = y + desc->height;
  for (int16_t line = y; line < line_max_exclusive; line++) {
    memcpy(dst, src, src_line_size);
    data->dirty[line] = true;
    src += src_line_size;
    dst += config->line_size;
  }
  return 0;
}

static inline int _buffer_rot_90_write(const struct device *dev, const uint16_t x, const uint16_t y,
                                       const struct display_buffer_descriptor *desc,
                                       const void *buf) {
  const struct ls0xx_config *config = dev->config;
  struct ls0xx_data *data = dev->data;

  uint8_t *src = (uint8_t *)buf;
  uint16_t src_line_size = DIV_ROUND_UP(desc->pitch, LS0XX_PIXELS_PER_BYTE);
  uint16_t x_max_exclusive = MIN(x + desc->width, config->width) - x;
  uint16_t y_max_exclusive = MIN(y + desc->height, config->height) - y;

  for (uint16_t src_y = 0; src_y < y_max_exclusive; src_y++) {
    uint16_t dst_x = y + src_y;
    /* +1 for gate-line address  */
    uint8_t *dst =
      &data
         ->buffer[(config->width - x - 1) * config->line_size + dst_x / LS0XX_PIXELS_PER_BYTE + 1];
    uint8_t dst_bit = dst_x % LS0XX_PIXELS_PER_BYTE;
    for (uint16_t src_x = 0; src_x < x_max_exclusive; src_x++) {
      if (src[src_x / LS0XX_PIXELS_PER_BYTE] & (1 << (src_x % LS0XX_PIXELS_PER_BYTE))) {
        *dst |= 1 << dst_bit;
      } else {
        *dst &= ~(1 << dst_bit);
      }
      dst -= config->line_size;
      data->dirty[config->width - src_x - 1] = true;
    }
    src += src_line_size;
  }
  return 0;
}

static inline int _buffer_rot_180_write(const struct device *dev, const uint16_t x,
                                        const uint16_t y,
                                        const struct display_buffer_descriptor *desc,
                                        const void *buf) {
  const struct ls0xx_config *config = dev->config;
  struct ls0xx_data *data = dev->data;
  uint16_t src_line_size = DIV_ROUND_UP(desc->pitch, LS0XX_PIXELS_PER_BYTE);
  uint8_t *src = (uint8_t *)buf;
  int16_t line = config->height - y - 1;
  int16_t line_min_inclusive = config->height - desc->height - y;
  /* +1 for gate-line address  */
  uint8_t *dst =
    &data->buffer[config->line_size * line + src_line_size + x / LS0XX_PIXELS_PER_BYTE];
  for (; line >= line_min_inclusive; line--) {
    for (uint8_t i = 0; i < src_line_size; i++) {
      dst[-i] = _reverse_bits(src[i]);
    }
    data->dirty[line] = true;
    dst -= config->line_size;
    src += src_line_size;
  }
  return 0;
}

static inline int _buffer_rot_270_write(const struct device *dev, const uint16_t x,
                                        const uint16_t y,
                                        const struct display_buffer_descriptor *desc,
                                        const void *buf) {
  const struct ls0xx_config *config = dev->config;
  struct ls0xx_data *data = dev->data;

  uint8_t *src = (uint8_t *)buf;
  uint16_t src_line_size = DIV_ROUND_UP(desc->pitch, LS0XX_PIXELS_PER_BYTE);
  uint16_t x_max_exclusive = MIN(x + desc->width, config->width) - x;
  uint16_t y_max_exclusive = MIN(y + desc->height, config->height) - y;

  for (uint16_t src_y = 0; src_y < y_max_exclusive; src_y++) {
    uint16_t dst_x = config->height - y - src_y - 1;
    /* +1 for gate-line address  */
    uint8_t *dst = &data->buffer[x * config->line_size + dst_x / LS0XX_PIXELS_PER_BYTE + 1];

    uint8_t dst_bit = dst_x % LS0XX_PIXELS_PER_BYTE;
    for (uint16_t src_x = 0; src_x < x_max_exclusive; src_x++) {
      if (src[src_x / LS0XX_PIXELS_PER_BYTE] & (1 << (src_x % LS0XX_PIXELS_PER_BYTE))) {
        *dst |= 1 << dst_bit;
      } else {
        *dst &= ~(1 << dst_bit);
      }
      dst += config->line_size;
      data->dirty[src_x] = true;
    }
    src += src_line_size;
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

static void _buffer_clear(const struct device *dev) {
  const struct ls0xx_config *config = dev->config;
  struct ls0xx_data *data = dev->data;

  /* lock buffer  */
  k_mutex_lock(&data->lock, K_FOREVER);

  uint8_t *dst = data->buffer;
  for (size_t line = 0; line < config->num_lines; line++) {
    /* gate-line address  */
    dst[0] = line + 1;
    memset(&dst[1], 0, config->line_size - 1);
    data->dirty[line] = false;

    dst += config->line_size;
  }

  /* unlock buffer  */
  k_mutex_unlock(&data->lock);
}

static void _buffer_full_flush(const struct device *dev) {
  const struct ls0xx_config *config = dev->config;
  struct ls0xx_data *data = dev->data;

  /* lock buffer  */
  k_mutex_lock(&data->lock, K_FOREVER);

  for (int line = 0; line < config->num_lines; line++) {
    data->dirty[line] = true;
  }

  /* unlock buffer  */
  k_mutex_unlock(&data->lock);
}

// driver API -->

static int ls0xx_blanking_on(const struct device *dev) {
  const struct ls0xx_config *config = dev->config;
  struct ls0xx_data *data = dev->data;

  LOG_DBG("start");

  atomic_set(&data->blanking, 1);
  if (config->disp_en_gpio != NULL) {
    return gpio_pin_set_dt(config->disp_en_gpio, 0);
  }

  LOG_DBG("end");

  return 0;
}

static int ls0xx_blanking_off(const struct device *dev) {
  const struct ls0xx_config *config = dev->config;
  struct ls0xx_data *data = dev->data;

  LOG_DBG("start");

  atomic_set(&data->blanking, 0);
  if (config->disp_en_gpio != NULL) {
    return gpio_pin_set_dt(config->disp_en_gpio, 1);
  }

  LOG_DBG("end");

  return 0;
}

/* Buffer width should be equal to display width */
static int ls0xx_write(const struct device *dev, const uint16_t x, const uint16_t y,
                       const struct display_buffer_descriptor *desc, const void *buf) {
  const struct ls0xx_config *config = dev->config;
  struct ls0xx_data *data = dev->data;

  LOG_DBG("start X:%d, Y:%d, W:%d, H:%d, L:%d, P:%d, R:%d", x, y, desc->width, desc->height,
          desc->buf_size, desc->pitch, config->rotated);

#if IS_ENABLED(CONFIG_PM_DEVICE)
  if (atomic_get(&data->suspended) == 1) {
    LOG_WRN("Display has been suspended");
    return -EINVAL;
  }
#endif

  _buffer_write(dev, x, y, desc, buf);

  LOG_DBG("end");
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
  caps->current_pixel_format = PIXEL_FORMAT_MONO01;
  // caps->screen_info = SCREEN_INFO_X_ALIGNMENT_WIDTH;
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

static void ls0xx_thread(void *arg1, void *arg2, void *arg3) {
  const struct device *dev = arg1;
  const struct ls0xx_config *config = dev->config;
  struct ls0xx_data *data = dev->data;

  bool prev_blanking = false;

  _buffer_clear(dev);
  _buffer_full_flush(dev);

  while (1) {
#if IS_ENABLED(CONFIG_PM_DEVICE)
    if (atomic_get(&data->suspended) == 1) {
      _buffer_clear(dev);

      /* for the devcie that are always powered */
      int err = _spi_cmd_write(dev, LS0XX_CMD_CLEAR);
      if (err) {
        LOG_ERR("SPI communication Failed. err: %d", err);
      }

      LOG_DBG("Suspended");

      k_sem_take(&data->wakeup, K_FOREVER);

      LOG_DBG("Wakeup");
      _buffer_full_flush(dev);
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
      if (atomic_get(&data->blanking) == 1) {
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
          _buffer_full_flush(dev);
        }
        prev_blanking = false;
      }
    }
    int err = _spi_cmd_write(dev, cmd);
    if (err) {
      LOG_ERR("SPI communication Failed. err: %d", err);
    }

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
    LOG_INF("Configuring DISP pin to OUTPUT_HIGH");
    err = gpio_pin_configure_dt(config->disp_en_gpio, GPIO_OUTPUT_HIGH);
    if (err < 0) {
      LOG_ERR("Failed to configure DISP port device. err: %d", err);
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

  atomic_set(&data->blanking, 0);

  err = k_mutex_init(&data->lock);
  if (err < 0) {
    LOG_ERR("Failed to initialize mutex. err: %d", err);
    return err;
  }

  k_timer_init(&data->timer, NULL, NULL);

#if IS_ENABLED(CONFIG_PM_DEVICE)
  err = k_sem_init(&data->wakeup, 0, 1);
  if (err < 0) {
    LOG_ERR("Failed to initialize semaphore. err: %d", err);
    return err;
  }
#endif

#if IS_ENABLED(CONFIG_PM_DEVICE_RUNTIM)

  atomic_set(&data->suspended, 1);

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
    (void *)dev, NULL, NULL, CONFIG_LS0XX_THREAD_PRIORITY, 0, K_NO_WAIT);
  k_thread_name_set(tid, "ls0xx_vcom");

  return 0;
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

  switch (action) {
    case PM_DEVICE_ACTION_SUSPEND:
      LOG_DBG("action suspend");
      atomic_set(&data->suspended, 1);
      k_timer_stop(&data->timer);
      break;
    case PM_DEVICE_ACTION_RESUME:
      LOG_DBG("action resume");
      _timer_start(dev);
      atomic_set(&data->suspended, 0);
      k_sem_give(&data->wakeup);
      break;
    default:
      return -ENOTSUP;
  }

  return 0;
}
#endif

/* if roatated 90 or 270 degrees */
#define IS_WH_SWAPPED(n) \
  UTIL_OR(IS_EQ(DT_INST_ENUM_IDX(n, rotated), 1), IS_EQ(DT_INST_ENUM_IDX(n, rotated), 3))

/*
 *  +2 for gate-line address and dummy data
 *  +0                +1                       +DIV_ROUND_UP(pixel_size, 8)
 *   |gate-line address|<-----pixel data------>|dummy|
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
    .com_frequency = DT_INST_PROP(n, com_frequency),                                      \
    .rotated = DT_INST_ENUM_IDX(n, rotated),                                              \
    .line_size = BUFFER_LINE_SIZE(n),                                                     \
    .num_lines = BUFFER_NUM_LINES(n),                                                     \
  }

#define BUFFER_DEF(n)                                                         \
  static uint8_t ls0xx_buffer_##n[BUFFER_NUM_LINES(n) * BUFFER_LINE_SIZE(n)]; \
  static bool ls0xx_dirty_##n[BUFFER_NUM_LINES(n)]

#define LS0XX_INIT(n)                                                                 \
  LS0XX_GPIO_DEF(n);                                                                  \
                                                                                      \
  LS0XX_CONFIG_DEF(n);                                                                \
                                                                                      \
  BUFFER_DEF(n);                                                                      \
                                                                                      \
  static struct ls0xx_data ls0xx_data_##n = {.buffer = ls0xx_buffer_##n,              \
                                             .dirty = ls0xx_dirty_##n};               \
                                                                                      \
  PM_DEVICE_DT_INST_DEFINE(n, ls0xx_pm_action);                                       \
                                                                                      \
  DEVICE_DT_INST_DEFINE(n, ls0xx_init, PM_DEVICE_DT_INST_GET(n), &ls0xx_data_##n,     \
                        &ls0xx_config_##n, POST_KERNEL, CONFIG_DISPLAY_INIT_PRIORITY, \
                        &ls0xx_driver_api);

DT_INST_FOREACH_STATUS_OKAY(LS0XX_INIT)
