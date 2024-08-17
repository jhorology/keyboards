/*
 * Copyright (c) 2020 PHYTEC Messtechnik GmbH, Peter Johanson
 * Copyright (c) 2024 The ZMK Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_DRIVERS_DISPLAY_UC8175_REGS_H_
#define ZEPHYR_DRIVERS_DISPLAY_UC8175_REGS_H_

#define UC8175_CMD_PSR 0x00
#define UC8175_CMD_PWR 0x01
#define UC8175_CMD_POF 0x02
#define UC8175_CMD_PFS 0x03
#define UC8175_CMD_PON 0x04
#define UC8175_CMD_PMES 0x05
#define UC8175_CMD_CPSET 0x06
#define UC8175_CMD_DSLP 0x07
#define UC8175_CMD_DTM1 0x10
#define UC8175_CMD_DSP 0x11
#define UC8175_CMD_DRF 0x12
#define UC8175_CMD_DTM2 0x13
#define UC8175_CMD_AUTO 0x17
#define UC8175_CMD_LUTW 0x23
#define UC8175_CMD_LUTB 0x24
#define UC8175_CMD_LUTOPT 0x2A
#define UC8175_CMD_PLL 0x30
#define UC8175_CMD_TSC 0x40
#define UC8175_CMD_TSE 0x41
#define UC8175_CMD_PBC 0x44
#define UC8175_CMD_CDI 0x50
#define UC8175_CMD_LPD 0x51
#define UC8175_CMD_TCON 0x60
#define UC8175_CMD_TRES 0x61
#define UC8175_CMD_GSST 0x65
#define UC8175_CMD_REV 0x70
#define UC8175_CMD_FLG 0x71
#define UC8175_CMD_CRC 0x72
#define UC8175_CMD_AMV 0x80
#define UC8175_CMD_VV 0x81
#define UC8175_CMD_VDCS 0x82
#define UC8175_CMD_PTL 0x90
#define UC8175_CMD_PIN 0x91
#define UC8175_CMD_POUT 0x92
#define UC8175_CMD_PGM 0xA0
#define UC8175_CMD_APG 0xA1
#define UC8175_CMD_ROTP 0xA2
#define UC8175_CMD_CCSET 0xE0
#define UC8175_CMD_PWS 0xE3
#define UC8175_CMD_LVSEL 0xE4
#define UC8175_CMD_TSSET 0xE5

#define UC8175_PWR_REG_LENGTH 4U
#define UC8175_LUTOPT_REG_LENGTH 2U
#define UC8175_LUT_REG_LENGTH 42U
#define UC8175_TRES_REG_LENGTH 2U

#define UC8175_PTL_REG_LENGTH 5U
#define UC8175_PTL_PT_SCAN BIT(0)

#define UC8175_DSLP_CODE 0xa5
#define UC8175_AUTO_POF 0xa5
#define UC8175_AUTO_DSLP 0xa7

/* Time constants in ms */
#define UC8175_RESET_DELAY 10U
#define UC8175_PON_DELAY 100U
#define UC8175_BUSY_DELAY 1U

#endif /* ZEPHYR_DRIVERS_DISPLAY_UC8175_REGS_H_ */
