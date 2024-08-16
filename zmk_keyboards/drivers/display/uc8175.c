/*
 * Copyright (c) 2020 PHYTEC Messtechnik GmbHH, Peter Johanson
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sys/_stdint.h>
#define DT_DRV_COMPAT ultrachip_uc8175

#include <string.h>
#include <zephyr/device.h>
#include <zephyr/init.h>
#include <zephyr/drivers/display.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/logging/log.h>

#include "uc8175_regs.h"

LOG_MODULE_REGISTER(uc8175, CONFIG_DISPLAY_LOG_LEVEL);

/**
 * UC8175 compatible EPD controller driver.
 */

#define UC8175_PIXELS_PER_BYTE 8U
// #define UC8175_CDI_REVERSE_MASK (BIT(7) + BIT(6) + BIT(4))
#define UC8175_CDI_MASK_INVERT BIT(4)
#define UC8175_CDI_MASK_REFRESH_MODE BIT(5)
#define BLANKING_KEEP_CONTENT 0U
#define BLANKING_WHITE 1U
#define BLANKING_BLACK 2U
#define BLANKING_INVERT 3U

// CDI bit5 DDX[1]
//   0: update all pixel
//   1: update only changed peixl
#define IS_XOR_REFRESH(config) ((config->cdi & UC8175_CDI_MASK_REFRESH_MODE) != 0)

// clang-format off
/*
  power_saving mode

   0: none
   1: POF (power-off) on blanking_on()
   2: POF (power-off) on blanking_on() and display_write()
   3: POF (power-off) on blanking_on() and display_write()
      DSLP (deep sleep) on blanking_on()
   4: POF (power-off) on blanking_on() and write()
      DSLP (deep sleep) on blanking_on() and write()

       init       blanking_off            blanking_on             write
    0  wake,PON   DRF                     DRF                     DRF
    1  wake       PON,DRF                 DRF,POF                 DRF
    2  wake       PON,DRF,POF             PON,DRF,POF             PON,DRF,POF
    3  ___        wake,PON,DRF,POF        PON,DRF,POF,DSLP        PON,DRF,POF
    4  ___        wake,PON,DRF,POF,DSLP   wake,PON,DRF,POF,DSLP   wake,PON,DRF,POF,DSLP

      default 3
 */
// clang-format on
enum {
  POWER_SAVING_NONE,
  POWER_SAVING_POF_ON_BLANKING,
  POWER_SAVING_POF_ON_WRITE,
  POWER_SAVING_DSLP_ON_BLANKING,
  POWER_SAVING_DSLP_ON_WRITE,
};

struct uc8175_config {
  // include: [spi-device.yaml]
  struct spi_dt_spec spi;

  // include: [display-controller.yaml]
  uint16_t height;
  uint16_t width;

  struct gpio_dt_spec reset;
  struct gpio_dt_spec dc;
  struct gpio_dt_spec busy;

  uint8_t blanking;

  uint8_t power_saving;
  uint8_t psr;
  uint8_t pwr[UC8175_PWR_REG_LENGTH];
  uint8_t cpset;
  uint8_t lutopt[UC8175_LUTOPT_REG_LENGTH];
  uint8_t pll;
  uint8_t cdi;
  uint8_t tcon;
  uint8_t vdcs;
  uint8_t pws;
  uint8_t lutw[UC8175_LUT_REG_LENGTH];
  uint8_t lutb[UC8175_LUT_REG_LENGTH];
};

struct uc8175_data {
  bool blanking_on;
  bool sleep;
  bool initialized;
};

static inline void _busy_wait(const struct device *dev) {
  const struct uc8175_config *config = dev->config;
  int pin = gpio_pin_get_dt(&config->busy);

  while (pin > 0) {
    __ASSERT(pin >= 0, "Failed to get pin level");
    // LOG_DBG("wait %u", pin);
    k_msleep(UC8175_BUSY_DELAY);
    pin = gpio_pin_get_dt(&config->busy);
  }
}

static inline void _reset(const struct device *dev) {
  const struct uc8175_config *config = dev->config;
  gpio_pin_set_dt(&config->reset, 1);
  k_msleep(UC8175_RESET_DELAY);
  gpio_pin_set_dt(&config->reset, 0);
  k_msleep(UC8175_RESET_DELAY);
  _busy_wait(dev);
}

static inline int _write_cmd_data(const struct device *dev, uint8_t cmd, void *data, size_t len,
                                  uint8_t fill_data, size_t fill_len) {
  const struct uc8175_config *config = dev->config;
  struct spi_buf buf = {.buf = &cmd, .len = sizeof(cmd)};
  struct spi_buf_set buf_set = {.buffers = &buf, .count = 1};
  int err;
  uint8_t fill_buf[64];

  err = gpio_pin_set_dt(&config->dc, 1);
  if (err < 0) {
    LOG_ERR("Failed to set DC GPIO to command mode. cmd: %02x err: %d", cmd, err);
    return err;
  }

  err = spi_write_dt(&config->spi, &buf_set);
  if (err < 0) {
    LOG_ERR("Failed to send command: %02x err: %d", cmd, err);
    return err;
  }

  LOG_DBG("Succeed to write cmd: 0x%02x: data length: %d", cmd, len);

  if (data != NULL && len > 0) {
    buf.buf = data;
    buf.len = len;

    err = gpio_pin_set_dt(&config->dc, 0);
    if (err < 0) {
      LOG_ERR("Failed to set DC GPIO to data mode. cmd: %02x err: %d", cmd, err);
      goto spi_out;
    }

    err = spi_write_dt(&config->spi, &buf_set);
    if (err < 0) {
      LOG_ERR("Failed to send data: 0x%02x len: %d err: %d", cmd, len, err);
      goto spi_out;
    }
    LOG_HEXDUMP_DBG(data, len, "Succeed to write data:");
  }

  while (fill_len > 0) {
    buf.buf = fill_buf;
    buf.len = MIN(fill_len, sizeof(fill_buf));
    memset(fill_buf, fill_data, buf.len);

    err = spi_write_dt(&config->spi, &buf_set);
    if (err < 0) {
      LOG_ERR("Failed to send fill data: %02x err: %d", cmd, err);
      goto spi_out;
    }

    fill_len -= buf.len;
  }

spi_out:
  spi_release_dt(&config->spi);
  return err;
}

static inline int _write_cmd(const struct device *dev, uint8_t cmd) {
  return _write_cmd_data(dev, cmd, NULL, 0, 0, 0);
}

static inline int _write_cmd_uint8_data(const struct device *dev, uint8_t cmd, uint8_t data) {
  return _write_cmd_data(dev, cmd, &data, 1, 0, 0);
}

static inline int _write_cmd_block_data(const struct device *dev, uint8_t cmd, void *data,
                                        size_t len) {
  return _write_cmd_data(dev, cmd, data, len, 0, 0);
}

static inline int _write_cmd_fill_data(const struct device *dev, uint8_t cmd, uint8_t fill_data,
                                       size_t fill_len) {
  return _write_cmd_data(dev, cmd, NULL, 0, fill_data, fill_len);
}

/**
 *  x0  top left horizontal position of partial window
 *  x1  bottom right horizontal position of partial window
 *  y0  top left vertical position of partial window
 *  y1  bottom right vertical position of wpartial window
 *  ptl_scan   true:  Gates scan only inside of the partial window
 *             false: Gates scan both inside and outside of the partial window.
 */
static int _window(const struct device *dev, uint16_t x0, uint16_t x1, uint16_t y0, uint16_t y1,
                   bool ptl_scan) {
  uint8_t ptl[UC8175_PTL_REG_LENGTH] = {x0, x1, y0, y1, ptl_scan ? 0 : 1};
  int err;
  // set partial window
  err = _write_cmd_block_data(dev, UC8175_CMD_PTL, ptl, sizeof(ptl));
  if (err < 0) {
    return err;
  }

  return 0;
}

static int _window_partial(const struct device *dev, uint16_t x0, uint16_t x1, uint16_t y0,
                           uint16_t y1) {
  /* TODO why ? */
  // return _window(dev, x0, x1, y0, y1, true);
  return _window(dev, x0, x1, y0, y1, false);
}

static int _window_full(const struct device *dev) {
  const struct uc8175_config *config = dev->config;
  int err;

  err = _window(dev, 0, config->width - 1, 0, config->height - 1, false);
  if (err < 0) {
    return err;
  }

  return 0;
}

static int _set_cdi_lut(const struct device *dev, bool restore, bool force, bool blanking) {
  const struct uc8175_config *config = dev->config;
  uint8_t cdi = config->cdi;
  void *lutw = (void *)config->lutw;
  void *lutb = (void *)config->lutb;
  int err;

  if (IS_XOR_REFRESH(config)) {
    cdi &= ~UC8175_CDI_MASK_REFRESH_MODE;
  }

  if (blanking) {
    switch (config->blanking) {
      case BLANKING_WHITE:
        lutw = lutb;
        break;
      case BLANKING_BLACK:
        lutb = lutw;
        break;
      case BLANKING_INVERT:
        cdi ^= UC8175_CDI_MASK_INVERT;
        break;
    }
  }

  if (force || cdi != config->cdi) {
    err = _write_cmd_uint8_data(dev, UC8175_CMD_CDI, restore ? config->cdi : cdi);
    if (err < 0) {
      return err;
    }
  }

  if (force || config->lutw != lutw) {
    err = _write_cmd_block_data(dev, UC8175_CMD_LUTW, restore ? (void *)config->lutw : lutw,
                                UC8175_LUTOPT_REG_LENGTH);
    if (err < 0) {
      return err;
    }
  }

  if (force || config->lutb != lutb) {
    err = _write_cmd_block_data(dev, UC8175_CMD_LUTB, restore ? (void *)config->lutb : lutb,
                                UC8175_LUTOPT_REG_LENGTH);
    if (err < 0) {
      return err;
    }
  }

  return 0;
}

static inline int _override_cdi_lut(const struct device *dev, bool force, bool blanking) {
  return _set_cdi_lut(dev, false, force, blanking);
}

static inline int _restore_cdi_lut(const struct device *dev, bool force, bool blanking) {
  return _set_cdi_lut(dev, true, force, blanking);
}

/**
 * @param full for fullscreen refresh
 * @param blanking for blanking_on()
 * @param post_process need post process
 */
static int _refresh(const struct device *dev, bool full, bool blanking, bool post_process) {
  const struct uc8175_config *config = dev->config;
  struct uc8175_data *data = dev->data;
  int err;

  switch (config->power_saving) {
    case POWER_SAVING_NONE:
    case POWER_SAVING_POF_ON_BLANKING:
      if (full) {
        err = _override_cdi_lut(dev, false, blanking);
        if (err < 0) {
          return err;
        }
      }

      err = _write_cmd(dev, UC8175_CMD_DRF);
      if (err < 0) {
        return err;
      }
      _busy_wait(dev);

      if (full) {
        err = _restore_cdi_lut(dev, false, blanking);
        if (err < 0) {
          return err;
        }
      }
      break;

    case POWER_SAVING_POF_ON_WRITE:
      if (full) {
        err = _override_cdi_lut(dev, false, blanking);
        if (err < 0) {
          return err;
        }
      }

      // 0xa5 = PON -> DRF -> POF
      err = _write_cmd_uint8_data(dev, UC8175_CMD_AUTO, 0xa5);
      if (err < 0) {
        return err;
      }
      _busy_wait(dev);

      if (full) {
        err = _restore_cdi_lut(dev, false, blanking);
        if (err < 0) {
          return err;
        }
      }
      break;

    case POWER_SAVING_DSLP_ON_BLANKING:
      if (full) {
        err = _override_cdi_lut(dev, true, blanking);
        if (err < 0) {
          return err;
        }
      }
      // 0xa5 = PON -> DRF -> POF
      // 0xa7 = PON -> DRF -> POF -> DSLP
      err = _write_cmd_uint8_data(dev, UC8175_CMD_AUTO, blanking ? 0xa7 : 0xa5);
      if (err < 0) {
        return err;
      }
      _busy_wait(dev);
      if (blanking) {
        data->sleep = true;
      }

      // blanking_off()
      if (full && !blanking) {
        err = _restore_cdi_lut(dev, false, true);
        if (err < 0) {
          return err;
        }
      }
      break;

    case POWER_SAVING_DSLP_ON_WRITE:
      if (full) {
        err = _override_cdi_lut(dev, true, blanking);
      } else {
        err = _restore_cdi_lut(dev, true, false);
      }
      if (err < 0) {
        return err;
      }

      // 0xa5 = PON -> DRF -> POF
      // 0xa7 = PON -> DRF -> POF -> DSLP
      err = _write_cmd_uint8_data(dev, UC8175_CMD_AUTO, post_process ? 0xa5 : 0xa7);
      if (err < 0) {
        return err;
      }
      _busy_wait(dev);

      if (!post_process) {
        data->sleep = true;
      }
      break;
  }

  return 0;
}

static inline int _refresh_full(const struct device *dev, bool blanking) {
  return _refresh(dev, true, blanking, false);
}

static inline int _refresh_partial(const struct device *dev, bool post_process) {
  return _refresh(dev, false, false, post_process);
}

static int _clear_frame_buffer(const struct device *dev) {
  const struct uc8175_config *config = dev->config;
  size_t buf_len = config->width * config->height / UC8175_PIXELS_PER_BYTE;
  int err;

  err = _window_full(dev);
  if (err < 0) {
    return err;
  }

  err = _write_cmd_fill_data(dev, UC8175_CMD_DTM1, 0xff, buf_len);
  if (err < 0) {
    return err;
  }

  err = _write_cmd_fill_data(dev, UC8175_CMD_DTM2, 0xff, buf_len);
  if (err < 0) {
    return err;
  }

  return 0;
}

static int _wake(const struct device *dev) {
  const struct uc8175_config *config = dev->config;
  struct uc8175_data *data = dev->data;
  uint8_t tmp[UC8175_TRES_REG_LENGTH];
  int err;

  _reset(dev);

  // unknown cmd 0xd2
  err = _write_cmd_uint8_data(dev, 0xd2, 0x3f);
  if (err < 0) {
    return err;
  }

  // PSR Panerl Setting
  err = _write_cmd_uint8_data(dev, UC8175_CMD_PSR, config->psr);
  if (err < 0) {
    return err;
  }

  // PWR Power setting
  err = _write_cmd_block_data(dev, UC8175_CMD_PWR, (void *)config->pwr, UC8175_PWR_REG_LENGTH);
  if (err < 0) {
    return err;
  }

  err = _write_cmd_uint8_data(dev, UC8175_CMD_CPSET, config->cpset);
  if (err < 0) {
    return err;
  }

  err =
    _write_cmd_block_data(dev, UC8175_CMD_LUTOPT, (void *)config->lutopt, UC8175_LUTOPT_REG_LENGTH);
  if (err < 0) {
    return err;
  }

  err = _write_cmd_uint8_data(dev, UC8175_CMD_PLL, config->pll);
  if (err < 0) {
    return err;
  }

  err = _write_cmd_uint8_data(dev, UC8175_CMD_TCON, config->tcon);
  if (err < 0) {
    return err;
  }

  tmp[0] = config->width;
  tmp[1] = config->height;
  err = _write_cmd_block_data(dev, UC8175_CMD_TRES, tmp, 2);

  err = _write_cmd_uint8_data(dev, UC8175_CMD_VDCS, config->vdcs);
  if (err < 0) {
    return err;
  }

  err = _write_cmd_uint8_data(dev, UC8175_CMD_PWS, config->pws);
  if (err < 0) {
    return err;
  }

  if (config->power_saving == POWER_SAVING_NONE ||
      config->power_saving == POWER_SAVING_POF_ON_BLANKING ||
      config->power_saving == POWER_SAVING_POF_ON_WRITE ||
      config->power_saving == POWER_SAVING_DSLP_ON_BLANKING) {
    err = _restore_cdi_lut(dev, true, false);
    if (err < 0) {
      return err;
    }
  }

  err = _write_cmd(dev, UC8175_CMD_PIN);
  if (err < 0) {
    return err;
  }

  if (!data->initialized) {
    _clear_frame_buffer(dev);
    data->initialized = true;
  }

  return 0;
}

static inline int _power_on(const struct device *dev) {
  int err = _write_cmd(dev, UC8175_CMD_PON);
  if (err < 0) {
    return err;
  }
  _busy_wait(dev);
  return 0;
}

static inline int _power_off(const struct device *dev) {
  int err = _write_cmd(dev, UC8175_CMD_POF);
  if (err < 0) {
    return err;
  }
  _busy_wait(dev);
  return 0;
}

static inline int _sleep(const struct device *dev) {
  struct uc8175_data *data = dev->data;
  int err;

  // deep sleep A5 = check code
  err = _write_cmd_uint8_data(dev, UC8175_CMD_DSLP, 0xa5);
  if (err) {
    return err;
  }
  _busy_wait(dev);

  data->sleep = true;

  return 0;
}

/**
 * @brief Write data to display
 *
 * @param dev Pointer to device structure
 * @param x x Coordinate of the upper left corner where to write the buffer
 * @param y y Coordinate of the upper left corner where to write the buffer
 * @param desc Pointer to a structure describing the buffer layout
 * @param buf Pointer to buffer array
 *
 * @retval 0 on success else negative errno code.
 */
static int uc8175_write(const struct device *dev, const uint16_t x, const uint16_t y,
                        const struct display_buffer_descriptor *desc, const void *buf) {
  const struct uc8175_config *config = dev->config;
  struct uc8175_data *data = dev->data;
  size_t buf_len = MIN(desc->buf_size, config->height * config->width / UC8175_PIXELS_PER_BYTE);

  int err;

  if (data->sleep) {
    err = _wake(dev);
    if (err < 0) {
      return err;
    }
  }

  LOG_DBG("start x %u, y %u, height %u, width %u, buf_size %u, pitch %u", x, y, desc->height,
          desc->width, desc->buf_size, desc->pitch);

  __ASSERT(desc->width <= desc->pitch, "Pitch is smaller then width");
  __ASSERT(buf != NULL, "Buffer is not available");
  __ASSERT(buf_len != 0U, "Buffer of length zero");
  __ASSERT(!(desc->width % UC8175_PIXELS_PER_BYTE), "Buffer width not multiple of %d",
           UC8175_PIXELS_PER_BYTE);

  LOG_DBG("buf_len %d", buf_len);
  if (((y + desc->height - 1) >= config->height) || ((x + desc->width - 1) >= config->width)) {
    LOG_ERR("Position out of bounds");
    return -EINVAL;
  }

  // NEW data
  err = _window_partial(dev, x, x + desc->width - 1, y, y + desc->height - 1);
  if (err < 0) {
    return err;
  }

  err = _write_cmd_block_data(dev, UC8175_CMD_DTM2, (void *)buf, buf_len);
  if (err < 0) {
    return err;
  }

  if (data->blanking_on) {
    if (config->power_saving == POWER_SAVING_DSLP_ON_BLANKING ||
        config->power_saving == POWER_SAVING_DSLP_ON_WRITE) {
      err = _sleep(dev);
      if (err < 0) {
        return err;
      }
    }
    return 0;
  }

  if (IS_XOR_REFRESH(config)) {
    err = _refresh_partial(dev, true);
    if (err < 0) {
      return err;
    }

    // OLD data

    // PTL setting is required after refresh
    err = _window_partial(dev, x, x + desc->width - 1, y, y + desc->height - 1);
    if (err < 0) {
      return err;
    }

    err = _write_cmd_block_data(dev, UC8175_CMD_DTM1, (void *)buf, buf_len);
    if (err < 0) {
      return err;
    }

    if (config->power_saving == POWER_SAVING_DSLP_ON_WRITE) {
      err = _sleep(dev);
      if (err < 0) {
        return err;
      }
    }
  } else {
    err = _refresh_partial(dev, false);
  }

  LOG_DBG("end");

  return 0;
}

/**
 * @brief Read data from display
 *
 * @param dev Pointer to device structure
 * @param x x Coordinate of the upper left corner where to read from
 * @param y y Coordinate of the upper left corner where to read from
 * @param desc Pointer to a structure describing the buffer layout
 * @param buf Pointer to buffer array
 *
 * @retval 0 on success else negative errno code.
 */
static int uc8175_read(const struct device *dev, const uint16_t x, const uint16_t y,
                       const struct display_buffer_descriptor *desc, void *buf) {
  LOG_ERR("not supported");
  return -ENOTSUP;
}

/**
 * @brief Get pointer to framebuffer for direct access
 *
 * @param dev Pointer to device structure
 *
 * @retval Pointer to frame buffer or NULL if direct framebuffer access
 * is not supported
 *
 */
static void *uc8175_get_framebuffer(const struct device *dev) {
  LOG_ERR("not supported");
  return NULL;
}

/**
 * @brief Turn display blanking on
 *
 * This function blanks the complete display.
 * The content of the frame buffer will be retained while blanking is enabled
 * and the frame buffer will be accessible for read and write operations.
 *
 * In case backlight control is supported by the driver the backlight is
 * turned off. The backlight configuration is retained and accessible for
 * configuration.
 *
 * In case the driver supports display blanking the initial state of the driver
 * would be the same as if this function was called.
 *
 * @param dev Pointer to device structure
 *
 * @retval 0 on success else negative errno code.
 */
static int uc8175_blanking_on(const struct device *dev) {
  struct uc8175_data *data = dev->data;
  int err;

  LOG_DBG("start");

  /* avoid flooding calls */
  if (data->blanking_on) {
    return 0;
  }

  if (data->sleep) {
    err = _wake(dev);
    if (err < 0) {
      return err;
    }
  }

  err = _window_full(dev);
  if (err < 0) {
    return err;
  }

  err = _refresh_full(dev, true);
  if (err < 0) {
    return err;
  }

  data->blanking_on = true;

  LOG_DBG("end");

  return 0;
}

/**
 * @brief Turn display blanking off
 *
 * Restore the frame buffer content to the display.
 * In case backlight control is supported by the driver the backlight
 * configuration is restored.
 *
 * @param dev Pointer to device structure
 *
 * @retval 0 on success else negative errno code.
 */
static int uc8175_blanking_off(const struct device *dev) {
  struct uc8175_data *data = dev->data;
  int err;

  LOG_DBG("start");

  if (data->sleep) {
    err = _wake(dev);
    if (err < 0) {
      return err;
    }
  }

  err = _window_full(dev);
  if (err < 0) {
    return err;
  }

  err = _refresh_full(dev, false);
  if (err < 0) {
    return err;
  }

  data->blanking_on = false;

  LOG_DBG("end");

  return 0;
}

/**
 * @brief Set the brightness of the display
 *
 * Set the brightness of the display in steps of 1/256, where 255 is full
 * brightness and 0 is minimal.
 *
 * @param dev Pointer to device structure
 * @param brightness Brightness in steps of 1/256
 *
 * @retval 0 on success else negative errno code.
 */
static int uc8175_set_brightness(const struct device *dev, const uint8_t brightness) {
  LOG_WRN("not supported");
  return -ENOTSUP;
}

/**
 * @brief Set the contrast of the display
 *
 * Set the contrast of the display in steps of 1/256, where 255 is maximum
 * difference and 0 is minimal.
 *
 * @param dev Pointer to device structure
 * @param contrast Contrast in steps of 1/256
 *
 * @retval 0 on success else negative errno code.
 */
static int uc8175_set_contrast(const struct device *dev, uint8_t contrast) {
  LOG_WRN("not supported");
  return -ENOTSUP;
}

/**
 * @brief Get display capabilities
 *
 * @param dev Pointer to device structure
 * @param capabilities Pointer to capabilities structure to populate
 */
static void uc8175_get_capabilities(const struct device *dev, struct display_capabilities *caps) {
  const struct uc8175_config *config = dev->config;

  memset(caps, 0, sizeof(struct display_capabilities));
  caps->x_resolution = config->width;
  caps->y_resolution = config->height;
  caps->supported_pixel_formats = PIXEL_FORMAT_MONO10;
  caps->current_pixel_format = PIXEL_FORMAT_MONO10;
  caps->screen_info = SCREEN_INFO_MONO_MSB_FIRST | SCREEN_INFO_EPD;
}

/**
 * @brief Set pixel format used by the display
 *
 * @param dev Pointer to device structure
 * @param pixel_format Pixel format to be used by display
 *
 * @retval 0 on success else negative errno code.
 */
static int uc8175_set_pixel_format(const struct device *dev, const enum display_pixel_format pf) {
  if (pf == PIXEL_FORMAT_MONO10) {
    return 0;
  }

  LOG_ERR("not supported pixel_format: %d", pf);
  return -ENOTSUP;
}

/**
 * @brief Set display orientation
 *
 * @param dev Pointer to device structure
 * @param orientation Orientation to be used by display
 *
 * @retval 0 on success else negative errno code.
 */
static int uc8175_set_orientation(const struct device *dev,
                                  const enum display_orientation orientation) {
  LOG_ERR("Unsupported");
  return -ENOTSUP;
}

static int uc8175_init(const struct device *dev) {
  const struct uc8175_config *config = dev->config;
  struct uc8175_data *data = dev->data;
  int err;

  LOG_DBG("start");

  if (!spi_is_ready_dt(&config->spi)) {
    LOG_ERR("SPI device not ready for UC8175");
    return -EIO;
  }

  if (!device_is_ready(config->reset.port)) {
    LOG_ERR("Could not get GPIO port for UC8175 reset");
    return -EIO;
  }

  err = gpio_pin_configure_dt(&config->reset, GPIO_OUTPUT_INACTIVE);
  if (err < 0) {
    LOG_ERR("Could not configure GPIO port for UC8175 reset");
    return err;
  }

  if (!device_is_ready(config->dc.port)) {
    LOG_ERR("Could not get GPIO port for UC8175 DC signal");
    return -EIO;
  }

  err = gpio_pin_configure_dt(&config->dc, GPIO_OUTPUT_INACTIVE);
  if (err < 0) {
    LOG_ERR("Could not configure GPIO port for UC8175 DC signal");
    return err;
  }

  if (!device_is_ready(config->busy.port)) {
    LOG_ERR("Could not get GPIO port for UC8175 busy signal");
    return -EIO;
  }

  err = gpio_pin_configure_dt(&config->busy, GPIO_INPUT);
  if (err < 0) {
    LOG_ERR("Could not configure GPIO port for UC8175 busy signal");
    return err;
  }
  switch (config->power_saving) {
    case POWER_SAVING_NONE:
      err = _wake(dev);
      if (err) {
        return err;
      }
      err = _power_on(dev);
      if (err) {
        return err;
      }
    case POWER_SAVING_POF_ON_BLANKING:
    case POWER_SAVING_POF_ON_WRITE:
      err = _wake(dev);
      if (err) {
        return err;
      }
      err = _power_off(dev);
      if (err) {
        return err;
      }
    case POWER_SAVING_DSLP_ON_BLANKING:
    case POWER_SAVING_DSLP_ON_WRITE:
      data->sleep = true;
      break;
  }

  LOG_DBG("end");

  return 0;
}

static const struct display_driver_api uc8175_display_api = {
  .blanking_on = uc8175_blanking_on,
  .blanking_off = uc8175_blanking_off,
  .write = uc8175_write,
  .read = uc8175_read,
  .get_framebuffer = uc8175_get_framebuffer,
  .set_brightness = uc8175_set_brightness,
  .set_contrast = uc8175_set_contrast,
  .get_capabilities = uc8175_get_capabilities,
  .set_pixel_format = uc8175_set_pixel_format,
  .set_orientation = uc8175_set_orientation,
};

#define UC8175_INIT(n)                                                                           \
  static struct uc8175_config uc8175_config_##n = {                                              \
    .spi = SPI_DT_SPEC_INST_GET(n, SPI_OP_MODE_MASTER | SPI_WORD_SET(8), 0),                     \
    .width = DT_INST_PROP(n, width),                                                             \
    .height = DT_INST_PROP(n, height),                                                           \
    .reset = GPIO_DT_SPEC_INST_GET(n, reset_gpios),                                              \
    .busy = GPIO_DT_SPEC_INST_GET(n, busy_gpios),                                                \
    .dc = GPIO_DT_SPEC_INST_GET(n, dc_gpios),                                                    \
    .blanking = DT_INST_PROP(n, blanking),                                                       \
    .power_saving = DT_INST_PROP(n, power_saving),                                               \
    .psr = DT_INST_PROP(n, psr),                                                                 \
    .pwr = DT_INST_PROP(n, pwr),                                                                 \
    .cpset = DT_INST_PROP(n, cpset),                                                             \
    .lutopt = DT_INST_PROP(n, lutopt),                                                           \
    .pll = DT_INST_PROP(n, pll),                                                                 \
    .cdi = DT_INST_PROP(n, cdi),                                                                 \
    .tcon = DT_INST_PROP(n, tcon),                                                               \
    .vdcs = DT_INST_PROP(n, vdcs),                                                               \
    .pws = DT_INST_PROP(n, pws),                                                                 \
    .lutw = DT_INST_PROP(n, lutw),                                                               \
    .lutb = DT_INST_PROP(n, lutb),                                                               \
  };                                                                                             \
  static struct uc8175_data uc8175_data_##n = {};                                                \
  DEVICE_DT_INST_DEFINE(n, uc8175_init, NULL, &uc8175_data_##n, &uc8175_config_##n, POST_KERNEL, \
                        CONFIG_APPLICATION_INIT_PRIORITY, &uc8175_display_api);

DT_INST_FOREACH_STATUS_OKAY(UC8175_INIT)
