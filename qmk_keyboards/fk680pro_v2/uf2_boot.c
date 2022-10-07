/* Copyright 2022 ZhaQian
 * Modified 2022 Masafumi
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <ch.h>

#include "bootloader.h"

// see https://github.com/mmoskal/uf2-stm32f103/blob/master/src/stm32f103/target_stm32f103.c#L53
#define CMD_BOOT 0x544F4F42UL
#define CMD_APP 0x3f82722aUL

#define MAGIC_REG *(volatile uint32_t*)0x20004000

void bootloader_jump(void) {
  MAGIC_REG = CMD_BOOT;
  NVIC_SystemReset();
}

void mcu_reset(void) {
  MAGIC_REG = CMD_APP;
  NVIC_SystemReset();
}

void enter_bootloader_mode_if_requested(void) {}
