// based on
// https://github.com/raspberrypi/pico-sdk/tree/master/src/rp2_common/pico_bootsel_via_double_reset

#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/linker/section_tags.h>
#include <zephyr/sys/reboot.h>

static __noinit uint32_t magic;

static int boot_double_tap_check(void) {
  if (magic != CONFIG_BOOTSEL_VIA_DOUBLE_RESET_MAGIC) {
    magic = CONFIG_BOOTSEL_VIA_DOUBLE_RESET_MAGIC;
    k_msleep(CONFIG_BOOTSEL_VIA_DOUBLE_RESET_TIMEOUT_MS);
    magic = 0;
    return 0;
  }
  sys_reboot(CONFIG_BOOTSEL_VIA_DOUBLE_RESET_TYPE);
  return 0;
}

SYS_INIT(boot_double_tap_check, POST_KERNEL, CONFIG_BOOTSEL_VIA_DOUBLE_RESET_INIT_PRIORITY);
