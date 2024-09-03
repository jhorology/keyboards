/*
 * Copyright (c) 2020 PHYTEC Messtechnik GmbHH, Peter Johanson
 * Copyright (c) 2024 The ZMK Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "zephyr/kernel.h"
#define DT_DRV_COMPAT ultrachip_uc8175_ya

#include <zephyr/device.h>
#include <zephyr/init.h>
#include <zephyr/drivers/display.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/pm/device.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/logging/log.h>

#include "uc8175_regs.h"

LOG_MODULE_REGISTER(uc8175, CONFIG_DISPLAY_LOG_LEVEL);

/*
 * CDI bit5 DDX[1]
 *   0: refresh all pixel
 *   1: refresh only changed peixl
 */
#define IS_XOR_REFRESH(config) ((config->cdi & UC8175_CDI_MASK_REFRESH_MODE) != 0)

/* border is explicitly defined */
#define IS_BORDERED(config) \
  ((config->cdi & UC8175_CDI_MASK_VBD) == 0x40 || (config->cdi & UC8175_CDI_MASK_VBD) == 0x80)

// ZMK dosen't wait for pm_device_busy
// #define PM_DEVICE_BUSY_SET(dev) pm_device_busy_set(dev)
// #define PM_DEVICE_BUSY_CLEAR(dev) pm_device_busy_clear(dev)
#define PM_DEVICE_BUSY_SET(dev)
#define PM_DEVICE_BUSY_CLEAR(dev)

/*
 *  power_saving mode
 *
 *   0: none
 *   1: POF (power-off) on blanking_on()
 *   2: POF (power-off) on blanking_on() and write()
 *   3: POF (power-off) on blanking_on() and write()
 *      DSLP (deep sleep) on blanking_on()
 *   4: POF (power-off) on blanking_on() and write()
 *      DSLP (deep sleep) on blanking_on() and write()
 *
 *       init       blanking_off            blanking_on             write
 *    0  wake,PON   DRF                     DRF                     DRF
 *    1  wake       PON,DRF                 DRF,POF                 DRF
 *    2  wake       PON,DRF,POF             PON,DRF,POF             PON,DRF,POF
 *    3  ___        wake,PON,DRF,POF        PON,DRF,POF,DSLP        PON,DRF,POF
 *    4  ___        wake,PON,DRF,POF,DSLP   wake,PON,DRF,POF,DSLP   wake,PON,DRF,POF,DSLP
 *
 *     default 3
 *
 * TODO add following mode
 *       init       blanking_off            blanking_on             write
 *    x  wake,PON   _wake,PON,DRF           DRF,POF,DSLP            DRF
 */
enum power_saving {
  POWER_SAVING_NONE,
  POWER_SAVING_POF_ON_BLANKING,
  POWER_SAVING_POF_ON_WRITE,
  POWER_SAVING_DSLP_ON_BLANKING,
  POWER_SAVING_DSLP_ON_WRITE,
};

/*
 * Blanking mode
 *   0 Keep display content.
 *   1 Fill white(background color)
 *   2 Fill black(foreground color)
 *   3 Invert display content
 */
enum blanking { KEEP_CONTENT, BLANKING_WHITE, BLANKING_BLACK, BLANKING_INVERT };

struct uc8175_config {
  // include: [spi-device.yaml]
  struct spi_dt_spec spi;

  // include: [display-controller.yaml]
  uint16_t height;
  uint16_t width;

  struct gpio_dt_spec reset;
  struct gpio_dt_spec dc;
  struct gpio_dt_spec busy;

  enum blanking blanking;
  enum blanking blanking_on_suspend;

  enum power_saving power_saving;

  bool anti_ghosting;

  uint8_t psr;
  uint8_t pwr[UC8175_PWR_REG_LENGTH];
  uint8_t cpset;
  uint8_t lutopt[UC8175_LUTOPT_REG_LENGTH];
  uint8_t pll;
  uint8_t cdi;
  uint8_t tcon;
  uint8_t vdcs;
  uint8_t pws;
  uint8_t lutw[UC8175_LUTW_REG_LENGTH];
  uint8_t lutb[UC8175_LUTB_REG_LENGTH];
};

struct uc8175_data {
  bool blanking_on;
  bool sleep;
  bool initialized;
  uint8_t cur_cdi;
  uint8_t *cur_lutw;
  uint8_t *cur_lutb;
#if IS_ENABLED(CONFIG_PM_DEVICE)
  struct k_mutex lock;
#endif
};

static inline void _busy_wait(const struct device *dev, bool command) {
  const struct uc8175_config *config = dev->config;
  // The waiting interval form BUSY_N falling to the first FLG command must be longer than 200uS.
  if (command) {
    k_usleep(UC8175_BUSY_DELAY);
  }
  int pin = gpio_pin_get_dt(&config->busy);
  while (pin > 0) {
    __ASSERT(pin >= 0, "Failed to get pin level");
    // LOG_DBG("wait %u", pin);
    k_usleep(UC8175_BUSY_DELAY);
    pin = gpio_pin_get_dt(&config->busy);
  }
}

static inline void _reset(const struct device *dev) {
  const struct uc8175_config *config = dev->config;
  gpio_pin_set_dt(&config->reset, 1);
  k_msleep(UC8175_RESET_DELAY);
  gpio_pin_set_dt(&config->reset, 0);
  k_msleep(UC8175_RESET_DELAY);
  _busy_wait(dev, false);
}

static int _write_cmd_data(const struct device *dev, uint8_t cmd, void *data, size_t len,
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
      LOG_ERR("Failed to send fill data: %02x err: %d", fill_data, err);
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

static inline int _power_on(const struct device *dev) {
  int err = _write_cmd(dev, UC8175_CMD_PON);
  if (err < 0) {
    return err;
  }
  // k_msleep(UC8175_PON_DELAY);
  _busy_wait(dev, true);
  return 0;
}

static inline int _power_off(const struct device *dev) {
  int err = _write_cmd(dev, UC8175_CMD_POF);
  if (err < 0) {
    return err;
  }
  return 0;
}

static inline int _sleep(const struct device *dev) {
  struct uc8175_data *data = dev->data;
  int err;

  err = _write_cmd_uint8_data(dev, UC8175_CMD_DSLP, UC8175_DSLP_CODE);
  if (err < 0) {
    return err;
  }
  data->sleep = true;

  return 0;
}

/**
 *  x0  top left horizontal position of partial window
 *  x1  bottom right horizontal position of partial window
 *  y0  top left vertical position of partial window
 *  y1  bottom right vertical position of wpartial window
 *  ptl_scan   true:  Gates scan only inside of the partial window
 *             false: Gates scan both inside and outside of the partial window.
 */
static inline int _window(const struct device *dev, uint16_t x0, uint16_t x1, uint16_t y0,
                          uint16_t y1, bool ptl_scan) {
  uint8_t ptl[UC8175_PTL_REG_LENGTH] = {x0, x1, y0, y1, ptl_scan ? 0 : 1};
  int err;
  // set partial window
  err = _write_cmd_block_data(dev, UC8175_CMD_PTL, ptl, sizeof(ptl));
  if (err < 0) {
    return err;
  }

  return 0;
}

static inline int _window_partial(const struct device *dev, uint16_t x0, uint16_t x1, uint16_t y0,
                                  uint16_t y1) {
  //  const struct uc8175_config *config = dev->config;
  // return _window(dev, x0, x1, y0, y1, !IS_XOR_REFRESH(config));
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

static int _write_cdi_lut(const struct device *dev, uint8_t cdi, void *lutw, void *lutb) {
  const struct uc8175_config *config = dev->config;
  struct uc8175_data *data = dev->data;
  int err;

  if (data->cur_cdi != cdi) {
    err = _write_cmd_uint8_data(dev, UC8175_CMD_CDI, cdi);
    if (err < 0) {
      return err;
    }
    LOG_DBG("CDI is %s.  %u -> %u.", cdi == config->cdi ? "restored" : "changed", data->cur_cdi,
            cdi);
    data->cur_cdi = cdi;
  }

  if (data->cur_lutw != lutw) {
    err = _write_cmd_block_data(dev, UC8175_CMD_LUTW, lutw, UC8175_LUTW_REG_LENGTH);
    if (err < 0) {
      return err;
    }
    LOG_DBG("LUTW is %s.", lutw == config->lutw ? "restored to white" : "changed to black");
    data->cur_lutw = lutw;
  }

  if (data->cur_lutb != lutb) {
    err = _write_cmd_block_data(dev, UC8175_CMD_LUTB, lutb, UC8175_LUTB_REG_LENGTH);
    if (err < 0) {
      return err;
    }
    LOG_DBG("LUTB is %s.", lutb == config->lutb ? "restored to black" : "changed to white");
    data->cur_lutb = lutb;
  }

  return 0;
}

static int _override_cdi_lut(const struct device *dev, enum blanking blanking) {
  const struct uc8175_config *config = dev->config;
  uint8_t cdi = config->cdi;
  void *lutw = (void *)config->lutw;
  void *lutb = (void *)config->lutb;
  int err;

  if (IS_XOR_REFRESH(config)) {
    cdi &= ~UC8175_CDI_MASK_REFRESH_MODE;
  }

  switch (blanking) {
    case BLANKING_WHITE:
      lutb = lutw;
      break;
    case BLANKING_BLACK:
      lutw = lutb;
      break;
    case BLANKING_INVERT:
      cdi ^= UC8175_CDI_MASK_DATA_POLARITY;
      // prevent inverting border
      if (IS_BORDERED(config)) {
        cdi ^= UC8175_CDI_MASK_VBD;
      }
      break;
    default:
      break;
  }

  err = _write_cdi_lut(dev, cdi, lutw, lutb);
  if (err < 0) {
    return err;
  }

  return 0;
}

static int _restore_cdi_lut(const struct device *dev) {
  const struct uc8175_config *config = dev->config;
  int err;

  err = _write_cdi_lut(dev, config->cdi, (void *)config->lutw, (void *)config->lutb);
  if (err < 0) {
    return err;
  }

  return 0;
}

/**
 * @param full for fullscreen refresh
 * @param blanking blanking mode
 * @param post_process need post process
 */
static int _refresh_full(const struct device *dev, bool blanking_on, enum blanking blanking) {
  const struct uc8175_config *config = dev->config;
  struct uc8175_data *data = dev->data;
  int err;

  err = _override_cdi_lut(dev, blanking_on ? blanking : KEEP_CONTENT);
  if (err < 0) {
    return err;
  }

  switch (config->power_saving) {
    case POWER_SAVING_NONE:

      err = _write_cmd(dev, UC8175_CMD_DRF);
      if (err < 0) {
        return err;
      }
      break;

    case POWER_SAVING_POF_ON_BLANKING:

      if (blanking_on) {
        err = _write_cmd_uint8_data(dev, UC8175_CMD_AUTO, UC8175_AUTO_POF);
        if (err < 0) {
          return err;
        }

      } else {
        err = _power_on(dev);
        if (err < 0) {
          return err;
        }
        err = _write_cmd(dev, UC8175_CMD_DRF);
        if (err < 0) {
          return err;
        }
      }
      break;

    case POWER_SAVING_POF_ON_WRITE:

      // 0xa5 = PON -> DRF -> POF
      err = _write_cmd_uint8_data(dev, UC8175_CMD_AUTO, UC8175_AUTO_POF);
      if (err < 0) {
        return err;
      }
      break;

    case POWER_SAVING_DSLP_ON_BLANKING:

      err = _write_cmd_uint8_data(dev, UC8175_CMD_AUTO,
                                  blanking_on ? UC8175_AUTO_DSLP : UC8175_AUTO_POF);
      if (err < 0) {
        return err;
      }
      if (blanking_on) {
        data->sleep = true;
      }
      break;

    case POWER_SAVING_DSLP_ON_WRITE:

      err = _write_cmd_uint8_data(dev, UC8175_CMD_AUTO, UC8175_AUTO_DSLP);
      if (err < 0) {
        return err;
      }

      data->sleep = true;
      break;
  }

  return 0;
}

static int _anti_ghosting(const struct device *dev, enum blanking blanking) {
  const struct uc8175_config *config = dev->config;
  bool is_black = blanking == BLANKING_BLACK || blanking == BLANKING_INVERT;
  int err;

  for (int i = 0; i < 2; i++) {
    err = _window_full(dev);
    if (err < 0) {
      return err;
    }

    err = _override_cdi_lut(dev, is_black ? BLANKING_BLACK : BLANKING_WHITE);
    is_black = !is_black;
    if (err < 0) {
      return err;
    }
    switch (config->power_saving) {
      case POWER_SAVING_NONE:
        err = _write_cmd(dev, UC8175_CMD_DRF);
        if (err < 0) {
          return err;
        }
        break;
      case POWER_SAVING_POF_ON_BLANKING:
      case POWER_SAVING_POF_ON_WRITE:
      case POWER_SAVING_DSLP_ON_BLANKING:
      case POWER_SAVING_DSLP_ON_WRITE:
        err = _write_cmd_uint8_data(dev, UC8175_CMD_AUTO, UC8175_AUTO_POF);
        if (err < 0) {
          return err;
        }
        break;
    }

    _busy_wait(dev, true);
  }

  return 0;
}

/**
 * @param post_process need post process
 */
static int _refresh_partial(const struct device *dev, bool post_process) {
  const struct uc8175_config *config = dev->config;
  struct uc8175_data *data = dev->data;
  int err;

  err = _restore_cdi_lut(dev);

  if (err < 0) {
    return err;
  }
  switch (config->power_saving) {
    case POWER_SAVING_NONE:
    case POWER_SAVING_POF_ON_BLANKING:

      err = _write_cmd(dev, UC8175_CMD_DRF);
      if (err < 0) {
        return err;
      }
      if (post_process) {
        _busy_wait(dev, true);
      }
      break;

    case POWER_SAVING_POF_ON_WRITE:
    case POWER_SAVING_DSLP_ON_BLANKING:

      err = _write_cmd_uint8_data(dev, UC8175_CMD_AUTO, UC8175_AUTO_POF);
      if (err < 0) {
        return err;
      }
      if (post_process) {
        _busy_wait(dev, true);
      }
      break;

    case POWER_SAVING_DSLP_ON_WRITE:

      err = _write_cmd(dev, UC8175_CMD_POUT);
      if (err < 0) {
        return err;
      }
      err = _write_cmd_uint8_data(dev, UC8175_CMD_AUTO,
                                  post_process ? UC8175_AUTO_POF : UC8175_AUTO_DSLP);
      if (err < 0) {
        return err;
      }

      if (post_process) {
        _busy_wait(dev, true);
        err = _write_cmd(dev, UC8175_CMD_PIN);
        if (err < 0) {
          return err;
        }
      } else {
        data->sleep = true;
      }
      break;
  }

  return 0;
}

static inline int _clear_frame_buffer(const struct device *dev) {
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

  LOG_DBG("start");

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

  data->cur_cdi = 0;
  data->cur_lutw = NULL;
  data->cur_lutb = NULL;

  switch (config->power_saving) {
    case POWER_SAVING_NONE:
    case POWER_SAVING_POF_ON_BLANKING:
    case POWER_SAVING_POF_ON_WRITE:
      err = _restore_cdi_lut(dev);
      if (err < 0) {
        return err;
      }
      break;
    case POWER_SAVING_DSLP_ON_BLANKING:
    case POWER_SAVING_DSLP_ON_WRITE:
      break;
  }

  err = _write_cmd(dev, UC8175_CMD_PIN);
  if (err < 0) {
    return err;
  }

  if (!data->initialized) {
    _clear_frame_buffer(dev);
    data->initialized = true;
  }

  data->sleep = false;

  LOG_DBG("end");
  return 0;
}

static int _resume(const struct device *dev) {
  const struct uc8175_config *config = dev->config;
  struct uc8175_data *data = dev->data;
  int err;

  switch (config->power_saving) {
    case POWER_SAVING_NONE:
      err = _wake(dev);
      if (err < 0) {
        return err;
      }
      err = _power_on(dev);
      if (err < 0) {
        return err;
      }
      break;

    case POWER_SAVING_POF_ON_BLANKING:
    case POWER_SAVING_POF_ON_WRITE:
      err = _wake(dev);
      if (err < 0) {
        return err;
      }

      err = _power_off(dev);
      if (err < 0) {
        return err;
      }
      break;

    case POWER_SAVING_DSLP_ON_BLANKING:
    case POWER_SAVING_DSLP_ON_WRITE:
      data->sleep = true;
      break;
  }
  data->blanking_on = true;

  return 0;
}

#if IS_ENABLED(CONFIG_PM_DEVICE)
static int _suspend(const struct device *dev) {
  const struct uc8175_config *config = dev->config;
  struct uc8175_data *data = dev->data;
  int err;

  if (data->sleep && config->blanking_on_suspend == KEEP_CONTENT && !config->anti_ghosting) {
    return 0;
  }

  _busy_wait(dev, false);

  if (config->blanking_on_suspend == KEEP_CONTENT && !config->anti_ghosting) {
    err = _power_off(dev);
    if (err < 0) {
      return err;
    }
    _busy_wait(dev, true);

    err = _sleep(dev);
    if (err < 0) {
      return err;
    }
    return 0;
  }

  if (data->sleep) {
    err = _wake(dev);
    if (err < 0) {
      return err;
    }
  }

  // anti ghosting
  if (config->anti_ghosting) {
    err = _anti_ghosting(dev, config->blanking_on_suspend);
    if (err < 0) {
      return err;
    }
  }

  err = _window_full(dev);
  if (err < 0) {
    return err;
  }

  err = _override_cdi_lut(dev, config->blanking_on_suspend);
  if (err < 0) {
    return err;
  }

  err = _write_cmd_uint8_data(dev, UC8175_CMD_AUTO, UC8175_AUTO_DSLP);
  if (err < 0) {
    return err;
  }
  data->sleep = true;
  data->blanking_on = true;

  return 0;
}
#endif

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
  size_t buf_len;
  int err;

  buf_len = MIN(desc->buf_size, config->height * config->width / UC8175_PIXELS_PER_BYTE);

  LOG_DBG(
    "start x %u, y %u, height %u, width %u, buf_size %u, buf_len %u, pitch %u, PS %u, sleep %u, "
    "blanking %u, ",
    x, y, desc->height, desc->width, desc->buf_size, buf_len, desc->pitch, config->power_saving,
    data->sleep, data->blanking_on);

  __ASSERT(desc->width <= desc->pitch, "Pitch is smaller then width");
  __ASSERT(buf != NULL, "Buffer is not available");
  __ASSERT(buf_len != 0U, "Buffer of length zero");
  __ASSERT(!(desc->width % UC8175_PIXELS_PER_BYTE), "Buffer width not multiple of %d",
           UC8175_PIXELS_PER_BYTE);
  __ASSERT((y + desc->height) <= config->height && (x + desc->width) <= config->width,
           "Position out of bounds");

  if (data->sleep) {
    err = _wake(dev);
    if (err < 0) {
      return err;
    }
  } else {
    _busy_wait(dev, false);
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

#if IS_ENABLED(CONFIG_PM_DEVICE)
static int uc8175_write_pm(const struct device *dev, const uint16_t x, const uint16_t y,
                           const struct display_buffer_descriptor *desc, const void *buf) {
  struct uc8175_data *data = dev->data;
  enum pm_device_state state;
  int ret;

  (void)pm_device_state_get(dev, &state);
  if (state == PM_DEVICE_STATE_SUSPENDED || state == PM_DEVICE_STATE_SUSPENDING) {
    return -EIO;
  }

  (void)k_mutex_lock(&data->lock, K_FOREVER);

#  if IS_ENABLED(CONFIG_PM_DEVICE_RUNTIME)
  (void)pm_device_runtime_get(dev);
#  else
  PM_DEVICE_BUSY_SET(dev);
#  endif

  ret = uc8175_write(dev, x, y, desc, buf);

#  if IS_ENABLED(CONFIG_PM_DEVICE_RUNTIME)
  (void)pm_device_runtime_put(dev);
#  else
  PM_DEVICE_BUSY_CLEAR(dev);
#  endif

  (void)k_mutex_unlock(&data->lock);

  return ret;
}
#endif

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
  const struct uc8175_config *config = dev->config;
  struct uc8175_data *data = dev->data;
  int err;

  LOG_DBG("start PS %u, sleep %u, blanking %u", config->power_saving, data->sleep,
          data->blanking_on);

  /* avoid flooding calls */
  if (data->blanking_on) {
    return 0;
  }
  _busy_wait(dev, false);

  if (data->sleep) {
    err = _wake(dev);
    if (err < 0) {
      return err;
    }
  }

  // anti ghosting
  if (config->anti_ghosting) {
    err = _anti_ghosting(dev, config->blanking);
    if (err < 0) {
      return err;
    }
  }

  err = _window_full(dev);
  if (err < 0) {
    return err;
  }

  err = _refresh_full(dev, true, config->blanking);
  if (err < 0) {
    return err;
  }

  data->blanking_on = true;

  LOG_DBG("end");

  return 0;
}

#if IS_ENABLED(CONFIG_PM_DEVICE)
static int uc8175_blanking_on_pm(const struct device *dev) {
  struct uc8175_data *data = dev->data;
  enum pm_device_state state;
  int ret;

  (void)pm_device_state_get(dev, &state);
  if (state == PM_DEVICE_STATE_SUSPENDED || state == PM_DEVICE_STATE_SUSPENDING) {
    return -EIO;
  }

  (void)k_mutex_lock(&data->lock, K_FOREVER);

#  if IS_ENABLED(CONFIG_PM_DEVICE_RUNTIME)
  (void)pm_device_runtime_get(dev);
#  else
  PM_DEVICE_BUSY_SET(dev);
#  endif

  ret = uc8175_blanking_on(dev);

#  if IS_ENABLED(CONFIG_PM_DEVICE_RUNTIME)
  (void)pm_device_runtime_put(dev);
#  else
  PM_DEVICE_BUSY_CLEAR(dev);
#  endif

  (void)k_mutex_unlock(&data->lock);

  return ret;
}
#endif

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
  const struct uc8175_config *config = dev->config;
  struct uc8175_data *data = dev->data;
  int err;

  LOG_DBG("start PS %u, sleep %u, blanking %u", config->power_saving, data->sleep,
          data->blanking_on);

  if (!data->blanking_on) {
    return 0;
  }

  if (data->sleep) {
    err = _wake(dev);
    if (err < 0) {
      return err;
    }
  } else {
    _busy_wait(dev, false);
  }

  err = _window_full(dev);
  if (err < 0) {
    return err;
  }

  err = _refresh_full(dev, false, KEEP_CONTENT);
  if (err < 0) {
    return err;
  }

  data->blanking_on = false;

  LOG_DBG("end");

  return 0;
}

#if IS_ENABLED(CONFIG_PM_DEVICE)
static int uc8175_blanking_off_pm(const struct device *dev) {
  struct uc8175_data *data = dev->data;
  enum pm_device_state state;
  int ret;

  (void)pm_device_state_get(dev, &state);
  if (state == PM_DEVICE_STATE_SUSPENDED || state == PM_DEVICE_STATE_SUSPENDING) {
    return -EIO;
  }

  (void)k_mutex_lock(&data->lock, K_FOREVER);

#  if IS_ENABLED(CONFIG_PM_DEVICE_RUNTIME)
  (void)pm_device_runtime_get(dev);
#  else
  PM_DEVICE_BUSY_SET(dev);
#  endif

  ret = uc8175_blanking_off(dev);

#  if IS_ENABLED(CONFIG_PM_DEVICE_RUNTIME)
  (void)pm_device_runtime_put(dev);
#  else
  PM_DEVICE_BUSY_CLEAR(dev);
#  endif

  (void)k_mutex_unlock(&data->lock);

  return ret;
}
#endif

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
  // ignore SCREEN_INFO_EPD I don't like LVGL behaivior for EPD
  // caps->screen_info = SCREEN_INFO_MONO_MSB_FIRST | SCREEN_INFO_EPD;
  caps->screen_info = SCREEN_INFO_MONO_MSB_FIRST | (config->anti_ghosting ? 0 : SCREEN_INFO_EPD);
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

  uint8_t res;
  if (config->width == 80 && config->height == 160) {
    res = 0;
  } else if (config->width == 80 && config->height == 128) {
    res = 1;
  } else if (config->width == 64 && config->height == 128) {
    res = 2;
  } else if (config->width == 64 && config->height == 96) {
    res = 3;
  } else {
    LOG_ERR("Unsupported panel resolution");
    return -ENOTSUP;
  }

  if ((config->psr >> 6) != res) {
    LOG_ERR("PSR attribute is conflict with resolution");
    return -ENOTSUP;
  }

  __ASSERT(resn > == 0, "The psr atribute is conflict with resolution");

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

  k_mutex_init(&data->lock);

#if IS_ENABLED(CONFIG_PM_DEVICE_RUNTIME)
  /* enable device runtime power management */
  pm_device_init_suspended(dev);
  err = pm_device_runtime_enable(dev);
  if ((err < 0) && (err != -ENOSYS)) {
    return err;
  }
#else
  err = _resume(dev);
  if (err < 0) {
    return err;
  }
#endif  // CONFIG_PM_DEVICE_RUNTIME

  LOG_DBG("end PS %u, sleep %u, blanking %u", config->power_saving, data->sleep, data->blanking_on);

  return 0;
}

#if IS_ENABLED(CONFIG_PM_DEVICE)

static int uc8175_suspend(const struct device *dev) {
  struct uc8175_data *data = dev->data;
  int ret;
  (void)k_mutex_lock(&data->lock, K_FOREVER);
  ret = _suspend(dev);
  (void)k_mutex_unlock(&data->lock);
  return ret;
}

static int uc8175_resume(const struct device *dev) {
  struct uc8175_data *data = dev->data;
  int ret;
  (void)k_mutex_lock(&data->lock, K_FOREVER);
  ret = _resume(dev);
  (void)k_mutex_unlock(&data->lock);
  return ret;
}

/**
 * @brief Device PM action callback.
 *
 * @param dev Device instance.
 * @param action Requested action.
 *
 * @retval 0 If successful.
 * @retval -ENOTSUP If the requested action is not supported.
 * @retval Errno Other negative errno on failure.
 */
static int uc8175_pm_action(const struct device *dev, enum pm_device_action action) {
  const struct uc8175_config *config = dev->config;
  struct uc8175_data *data = dev->data;
  int err;
  LOG_DBG("start action %u, PS %u, sleep %u, blanking %u", action, config->power_saving,
          data->sleep, data->blanking_on);
  switch (action) {
    case PM_DEVICE_ACTION_SUSPEND:
      err = uc8175_suspend(dev);
      if (err < 0) {
        return err;
      }
      break;
    case PM_DEVICE_ACTION_RESUME:
      err = uc8175_resume(dev);
      if (err < 0) {
        return err;
      }
      break;
    case PM_DEVICE_ACTION_TURN_OFF:
    case PM_DEVICE_ACTION_TURN_ON:
    default:
      return -ENOTSUP;
      break;
  }
  LOG_DBG("end action %u, PS %u, sleep %u, blanking %u", action, config->power_saving, data->sleep,
          data->blanking_on);
  return 0;
}
#endif /* CONFIG_PM_DEVICE  */

static const struct display_driver_api uc8175_display_api = {
#if IS_ENABLED(CONFIG_PM_DEVICE)
  .blanking_on = uc8175_blanking_on_pm,
  .blanking_off = uc8175_blanking_off_pm,
  .write = uc8175_write_pm,
#else
  .blanking_on = uc8175_blanking_on,
  .blanking_off = uc8175_blanking_off,
  .write = uc8175_write,
#endif
  .read = uc8175_read,
  .get_framebuffer = uc8175_get_framebuffer,
  .set_brightness = uc8175_set_brightness,
  .set_contrast = uc8175_set_contrast,
  .get_capabilities = uc8175_get_capabilities,
  .set_pixel_format = uc8175_set_pixel_format,
  .set_orientation = uc8175_set_orientation,
};

#define UC8175_INIT(n)                                                                     \
  static struct uc8175_config uc8175_config_##n = {                                        \
    .spi = SPI_DT_SPEC_INST_GET(n, SPI_OP_MODE_MASTER | SPI_LOCK_ON | SPI_WORD_SET(8), 0), \
    .width = DT_INST_PROP(n, width),                                                       \
    .height = DT_INST_PROP(n, height),                                                     \
    .reset = GPIO_DT_SPEC_INST_GET(n, reset_gpios),                                        \
    .busy = GPIO_DT_SPEC_INST_GET(n, busy_gpios),                                          \
    .dc = GPIO_DT_SPEC_INST_GET(n, dc_gpios),                                              \
    .power_saving = DT_INST_PROP(n, power_saving),                                         \
    .blanking = DT_INST_PROP(n, blanking),                                                 \
    .blanking_on_suspend = DT_INST_PROP(n, blanking_on_suspend),                           \
    .anti_ghosting = DT_INST_PROP(n, anti_ghosting),                                       \
    .psr = DT_INST_PROP(n, psr),                                                           \
    .pwr = DT_INST_PROP(n, pwr),                                                           \
    .cpset = DT_INST_PROP(n, cpset),                                                       \
    .lutopt = DT_INST_PROP(n, lutopt),                                                     \
    .pll = DT_INST_PROP(n, pll),                                                           \
    .cdi = DT_INST_PROP(n, cdi),                                                           \
    .tcon = DT_INST_PROP(n, tcon),                                                         \
    .vdcs = DT_INST_PROP(n, vdcs),                                                         \
    .pws = DT_INST_PROP(n, pws),                                                           \
    .lutw = DT_INST_PROP(n, lutw),                                                         \
    .lutb = DT_INST_PROP(n, lutb),                                                         \
  };                                                                                       \
  static struct uc8175_data uc8175_data_##n = {};                                          \
  PM_DEVICE_DT_INST_DEFINE(n, uc8175_pm_action);                                           \
  DEVICE_DT_INST_DEFINE(n, uc8175_init, PM_DEVICE_DT_INST_GET(n), &uc8175_data_##n,        \
                        &uc8175_config_##n, POST_KERNEL, CONFIG_DISPLAY_INIT_PRIORITY,     \
                        &uc8175_display_api);

DT_INST_FOREACH_STATUS_OKAY(UC8175_INIT)
