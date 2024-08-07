/*
 * Copyright (c) 2020 PHYTEC Messtechnik GmbHH, Peter Johanson
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT gooddisplay_uc8175

#include <string.h>
#include <zephyr/device.h>
#include <zephyr/init.h>
#include <zephyr/drivers/display.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/sys/byteorder.h>

#include "uc8175_regs.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(uc8175, CONFIG_DISPLAY_LOG_LEVEL);

/**
 * UC8175 compatible EPD controller driver.
 *
 */

#define EPD_PANEL_WIDTH DT_INST_PROP(0, width)
#define EPD_PANEL_HEIGHT DT_INST_PROP(0, height)
#define UC8175_PIXELS_PER_BYTE 8U

/* Horizontally aligned page! */
#define UC8175_NUMOF_PAGES (EPD_PANEL_WIDTH / UC8175_PIXELS_PER_BYTE)
#define UC8175_PANEL_FIRST_GATE 0U
#define UC8175_PANEL_LAST_GATE (EPD_PANEL_HEIGHT - 1)
#define UC8175_PANEL_FIRST_PAGE 0U
#define UC8175_PANEL_LAST_PAGE (UC8175_NUMOF_PAGES - 1)
#define UC8175_BUFFER_SIZE 1280

// clang-format off
/* white look-up table  */
static const uint8_t lut_w[] = {
  // stage format
  // -------------------------------------------
  // 00H bit 7-6 Phase 0
  //     bit 5-4 Phase 2
  //     bit 3-2 Phase 3
  //     bit 1-0 Phase 4
  // 01  Number of frame 0
  // 02  Number of frame 1
  // 03  Number of frame 2
  // 04  Number of frame 3
  // 05  times to repeat
  //---------------------------------------------
  // Phase
  //   00b: GND
  //   01b: VDH
  //   10b: VDL
  //   11:  FLoating
  //--------------------------------------------
  // stage 0
  // Phase 0 -> VDH 1 frame
  // Phase 1 -> VDL 1 frame
  // repat 1
  UC8175_LUT_PHASE_0_VDH | UC8175_LUT_PHASE_1_VDL, 0x01, 0x01, 0x00, 0x00, 0x01,
  // stage 1
  // Phase 0 -> VDL 15 frames
  // repat 1
  UC8175_LUT_PHASE_0_VDL, 0x0f, 0x00, 0x00, 0x00, 0x01,
  // stage 2 none
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  // stage 3 none
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  // stage 4 none
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  // stage 5 none
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  // stage 6 none
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

/* black look-up table  */
static const uint8_t lut_b[] = {
  // stage 0
  // Phase 0 -> VDL 1 frame
  // Phase 1 -> VDH 1 frame
  // repat 1
  UC8175_LUT_PHASE_0_VDL | UC8175_LUT_PHASE_1_VDH, 0x01, 0x01, 0x00, 0x00, 0x01,
  // stage 1
  // Phase 0 -> VDH 15 frames
  // repat 1
  UC8175_LUT_PHASE_0_VDH, 0x0f, 0x00, 0x00, 0x00, 0x01,
  // stage 2
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  // stage 3
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  // stage 4
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  // stage 5
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  // stage 6
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
// clang-format on

struct uc8175_cfg {
  struct gpio_dt_spec reset;
  struct gpio_dt_spec dc;
  struct gpio_dt_spec busy;
  struct spi_dt_spec spi;
};

static uint8_t uc8175_pwr[] = DT_INST_PROP(0, pwr);

static uint8_t last_buffer[UC8175_BUFFER_SIZE];
static bool blanking_on = true;
static bool init_clear_done = false;

static inline int uc8175_write_cmd(const struct uc8175_cfg *cfg, uint8_t cmd, uint8_t *data,
                                   size_t len) {
  struct spi_buf buf = {.buf = &cmd, .len = sizeof(cmd)};
  struct spi_buf_set buf_set = {.buffers = &buf, .count = 1};

  gpio_pin_set_dt(&cfg->dc, 1);
  if (spi_write_dt(&cfg->spi, &buf_set)) {
    return -EIO;
  }

  if (data != NULL) {
    buf.buf = data;
    buf.len = len;
    gpio_pin_set_dt(&cfg->dc, 0);
    if (spi_write_dt(&cfg->spi, &buf_set)) {
      return -EIO;
    }
  }

  return 0;
}

static inline void uc8175_busy_wait(const struct uc8175_cfg *cfg) {
  int pin = gpio_pin_get_dt(&cfg->busy);

  while (pin > 0) {
    __ASSERT(pin >= 0, "Failed to get pin level");
    // LOG_DBG("wait %u", pin);
    k_msleep(UC8175_BUSY_DELAY);
    pin = gpio_pin_get_dt(&cfg->busy);
  }
}

static int uc8175_update_display(const struct device *dev) {
  const struct uc8175_cfg *cfg = dev->config;

  LOG_DBG("Trigger update sequence");
  if (uc8175_write_cmd(cfg, UC8175_CMD_DRF, NULL, 0)) {
    return -EIO;
  }

  k_msleep(UC8175_BUSY_DELAY);

  return 0;
}

static int uc8175_write(const struct device *dev, const uint16_t x, const uint16_t y,
                        const struct display_buffer_descriptor *desc, const void *buf) {
  const struct uc8175_cfg *cfg = dev->config;
  uint16_t x_end_idx = x + desc->width - 1;
  uint16_t y_end_idx = y + desc->height - 1;
  uint8_t ptl[UC8175_PTL_REG_LENGTH] = {0};
  size_t buf_len;

  LOG_DBG("x %u, y %u, height %u, width %u, pitch %u", x, y, desc->height, desc->width,
          desc->pitch);

  buf_len = MIN(desc->buf_size, desc->height * desc->width / UC8175_PIXELS_PER_BYTE);
  __ASSERT(desc->width <= desc->pitch, "Pitch is smaller then width");
  __ASSERT(buf != NULL, "Buffer is not available");
  __ASSERT(buf_len != 0U, "Buffer of length zero");
  __ASSERT(!(desc->width % UC8175_PIXELS_PER_BYTE), "Buffer width not multiple of %d",
           UC8175_PIXELS_PER_BYTE);

  LOG_DBG("buf_len %d", buf_len);
  if ((y_end_idx > (EPD_PANEL_HEIGHT - 1)) || (x_end_idx > (EPD_PANEL_WIDTH - 1))) {
    LOG_ERR("Position out of bounds");
    return -EINVAL;
  }

  /* Setup Partial Window and enable Partial Mode */
  ptl[UC8175_PTL_HRST_IDX] = x;
  ptl[UC8175_PTL_HRED_IDX] = x_end_idx;
  ptl[UC8175_PTL_VRST_IDX] = y;
  ptl[UC8175_PTL_VRED_IDX] = y_end_idx;
  ptl[sizeof(ptl) - 1] = UC8175_PTL_PT_SCAN;

  LOG_HEXDUMP_DBG(ptl, sizeof(ptl), "ptl");

  uc8175_busy_wait(cfg);
  if (uc8175_write_cmd(cfg, UC8175_CMD_PIN, NULL, 0)) {
    return -EIO;
  }

  if (uc8175_write_cmd(cfg, UC8175_CMD_PTL, ptl, sizeof(ptl))) {
    return -EIO;
  }

  if (uc8175_write_cmd(cfg, UC8175_CMD_DTM1, last_buffer, UC8175_BUFFER_SIZE)) {
    return -EIO;
  }

  if (uc8175_write_cmd(cfg, UC8175_CMD_DTM2, (uint8_t *)buf, buf_len)) {
    return -EIO;
  }

  memcpy(last_buffer, (uint8_t *)buf, UC8175_BUFFER_SIZE);

  /* Update partial window and disable Partial Mode */
  if (blanking_on == false) {
    if (uc8175_update_display(dev)) {
      return -EIO;
    }
  }

  if (uc8175_write_cmd(cfg, UC8175_CMD_POUT, NULL, 0)) {
    return -EIO;
  }

  return 0;
}

static int uc8175_read(const struct device *dev, const uint16_t x, const uint16_t y,
                       const struct display_buffer_descriptor *desc, void *buf) {
  LOG_ERR("not supported");
  return -ENOTSUP;
}

static int uc8175_clear_and_write_buffer(const struct device *dev, uint8_t pattern, bool update) {
  struct display_buffer_descriptor desc = {
    .buf_size = UC8175_NUMOF_PAGES,
    .width = EPD_PANEL_WIDTH,
    .height = 1,
    .pitch = EPD_PANEL_WIDTH,
  };
  uint8_t *line;

  line = k_malloc(UC8175_NUMOF_PAGES);
  if (line == NULL) {
    LOG_ERR("Failed to allocate memory for the clear");
    return -ENOMEM;
  }

  memset(line, pattern, UC8175_NUMOF_PAGES);
  for (int i = 0; i < EPD_PANEL_HEIGHT; i++) {
    uc8175_write(dev, 0, i, &desc, line);
  }

  k_free(line);

  if (update == true) {
    if (uc8175_update_display(dev)) {
      return -EIO;
    }
  }

  return 0;
}

static int uc8175_blanking_off(const struct device *dev) {
  const struct uc8175_cfg *cfg = dev->config;

  if (!init_clear_done) {
    /* Update EPD panel in normal mode */
    uc8175_busy_wait(cfg);
    if (uc8175_clear_and_write_buffer(dev, 0xff, true)) {
      return -EIO;
    }
    init_clear_done = true;
  }

  blanking_on = false;

  if (uc8175_update_display(dev)) {
    return -EIO;
  }

  return 0;
}

static int uc8175_blanking_on(const struct device *dev) {
  blanking_on = true;

  return 0;
}

static void *uc8175_get_framebuffer(const struct device *dev) {
  LOG_ERR("not supported");
  return NULL;
}

static int uc8175_set_brightness(const struct device *dev, const uint8_t brightness) {
  LOG_WRN("not supported");
  return -ENOTSUP;
}

static int uc8175_set_contrast(const struct device *dev, uint8_t contrast) {
  LOG_WRN("not supported");
  return -ENOTSUP;
}

static void uc8175_get_capabilities(const struct device *dev, struct display_capabilities *caps) {
  memset(caps, 0, sizeof(struct display_capabilities));
  caps->x_resolution = EPD_PANEL_WIDTH;
  caps->y_resolution = EPD_PANEL_HEIGHT;
  caps->supported_pixel_formats = PIXEL_FORMAT_MONO10 | PIXEL_FORMAT_MONO01;
  caps->current_pixel_format = PIXEL_FORMAT_MONO10;
  caps->screen_info = SCREEN_INFO_MONO_MSB_FIRST | SCREEN_INFO_EPD;
}

static int uc8175_set_orientation(const struct device *dev,
                                  const enum display_orientation orientation) {
  LOG_ERR("Unsupported");
  return -ENOTSUP;
}

static int uc8175_set_pixel_format(const struct device *dev, const enum display_pixel_format pf) {
  if ((pf == PIXEL_FORMAT_MONO10) || (pf == PIXEL_FORMAT_MONO10)) {
    return 0;
  }

  LOG_ERR("not supported");
  return -ENOTSUP;
}

static int uc8175_controller_init(const struct device *dev) {
  const struct uc8175_cfg *cfg = dev->config;
  uint8_t tmp[UC8175_TRES_REG_LENGTH];

  LOG_DBG("");

  gpio_pin_set_dt(&cfg->reset, 1);
  k_msleep(UC8175_RESET_DELAY);
  gpio_pin_set_dt(&cfg->reset, 0);
  k_msleep(UC8175_RESET_DELAY);
  uc8175_busy_wait(cfg);

  LOG_DBG("Initialize UC8175 controller");

  // unknown cmd 0xd2
  tmp[0] = 0x3f;
  if (uc8175_write_cmd(cfg, 0xd2, tmp, 1)) {
    return -EIO;
  }

  /* Pannel settings, KW mode */
  tmp[0] = UC8175_PSR_UD | UC8175_PSR_LUT_REG | UC8175_PSR_SHL | UC8175_PSR_SHD | UC8175_PSR_RST;
#if EPD_PANEL_WIDTH == 80

#  if EPD_PANEL_HEIGHT == 128
  tmp[0] |= UC8175_PSR_RES_HEIGHT;
#  endif /* panel height */

#else
  tmp[0] |= UC8175_PSR_RES_WIDTH;
#  if EPD_PANEL_HEIGHT == 96
  tmp[0] |= UC8175_PSR_RES_HEIGHT;
#  else
#  endif /* panel height */

#endif /* panel width */

  LOG_HEXDUMP_DBG(tmp, 1, "PSR");
  if (uc8175_write_cmd(cfg, UC8175_CMD_PSR, tmp, 1)) {
    return -EIO;
  }

  if (uc8175_write_cmd(cfg, UC8175_CMD_PWR, uc8175_pwr, sizeof(uc8175_pwr))) {
    return -EIO;
  }

  tmp[0] = 0x3f;
  if (uc8175_write_cmd(cfg, UC8175_CMD_CPSET, tmp, 1)) {
    return -EIO;
  }

  tmp[0] = 0x0;
  tmp[1] = 0x0;
  if (uc8175_write_cmd(cfg, UC8175_CMD_LUTOPT, tmp, 2)) {
    return -EIO;
  }

  tmp[0] = 0x13;  // 50Hz
  if (uc8175_write_cmd(cfg, UC8175_CMD_PLL, tmp, 1)) {
    return -EIO;
  }

  tmp[UC8175_CDI_CDI_IDX] = DT_INST_PROP(0, cdi);
  LOG_HEXDUMP_DBG(tmp, UC8175_CDI_REG_LENGTH, "CDI");
  if (uc8175_write_cmd(cfg, UC8175_CMD_CDI, tmp, UC8175_CDI_REG_LENGTH)) {
    return -EIO;
  }

  /* Set panel resolution */
  tmp[UC8175_TRES_HRES_IDX] = EPD_PANEL_WIDTH;
  tmp[UC8175_TRES_VRES_IDX] = EPD_PANEL_HEIGHT;
  LOG_HEXDUMP_DBG(tmp, UC8175_TRES_REG_LENGTH, "TRES");
  if (uc8175_write_cmd(cfg, UC8175_CMD_TRES, tmp, UC8175_TRES_REG_LENGTH)) {
    return -EIO;
  }

  tmp[0] = DT_INST_PROP(0, tcon);
  if (uc8175_write_cmd(cfg, UC8175_CMD_TCON, tmp, 1)) {
    return -EIO;
  }

  tmp[0] = 0x12;  // -1.0v
  if (uc8175_write_cmd(cfg, UC8175_CMD_VDCS, tmp, 1)) {
    return -EIO;
  }

  tmp[0] = 0x33;
  if (uc8175_write_cmd(cfg, UC8175_CMD_PWS, tmp, 1)) {
    return -EIO;
  }

  if (uc8175_write_cmd(cfg, UC8175_CMD_LUTW, (uint8_t *)lut_w, sizeof(lut_w))) {
    return -EIO;
  }

  if (uc8175_write_cmd(cfg, UC8175_CMD_LUTB, (uint8_t *)lut_b, sizeof(lut_b))) {
    return -EIO;
  }

  /* Turn on: booster, controller, regulators, and sensor. */

  if (uc8175_write_cmd(cfg, UC8175_CMD_PON, NULL, 0)) {
    return -EIO;
  }

  k_msleep(UC8175_PON_DELAY);
  uc8175_busy_wait(cfg);

  /* Enable Auto Sequence */
  tmp[0] = UC8175_AUTO_PON_DRF_POF_DSLP;
  if (uc8175_write_cmd(cfg, UC8175_CMD_AUTO, tmp, 1)) {
    return -EIO;
  }

  return 0;
}

static int uc8175_init(const struct device *dev) {
  const struct uc8175_cfg *cfg = dev->config;

  if (!spi_is_ready_dt(&cfg->spi)) {
    LOG_ERR("SPI device not ready for UC8175");
    return -EIO;
  }

  if (!device_is_ready(cfg->reset.port)) {
    LOG_ERR("Could not get GPIO port for UC8175 reset");
    return -EIO;
  }

  gpio_pin_configure_dt(&cfg->reset, GPIO_OUTPUT_INACTIVE);

  if (!device_is_ready(cfg->dc.port)) {
    LOG_ERR("Could not get GPIO port for UC8175 DC signal");
    return -EIO;
  }

  gpio_pin_configure_dt(&cfg->dc, GPIO_OUTPUT_INACTIVE);

  if (!device_is_ready(cfg->busy.port)) {
    LOG_ERR("Could not get GPIO port for UC8175 busy signal");
    return -EIO;
  }

  gpio_pin_configure_dt(&cfg->busy, GPIO_INPUT);

  return uc8175_controller_init(dev);
}

static struct uc8175_cfg uc8175_config = {
  .spi = SPI_DT_SPEC_INST_GET(0, SPI_OP_MODE_MASTER | SPI_WORD_SET(8), 0),
  .reset = GPIO_DT_SPEC_INST_GET(0, reset_gpios),
  .busy = GPIO_DT_SPEC_INST_GET(0, busy_gpios),
  .dc = GPIO_DT_SPEC_INST_GET(0, dc_gpios),
};

static struct display_driver_api uc8175_driver_api = {
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

DEVICE_DT_INST_DEFINE(0, uc8175_init, NULL, NULL, &uc8175_config, POST_KERNEL,
                      CONFIG_APPLICATION_INIT_PRIORITY, &uc8175_driver_api);
