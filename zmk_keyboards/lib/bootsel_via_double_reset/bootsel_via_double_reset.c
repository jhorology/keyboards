// based on https://github.com/raspberrypi/pico-sdk/tree/master/src/rp2_common/pico_bootsel_via_double_reset

#include <zephyr/init.h>
#include <zephyr/sys/reboot.h>

static const uint32_t magic_token = 0xf01681de;

static __noinit uint32_t magic;

static int boot_double_tap_check(const struct device *port) {
  ARG_UNUSED(port);

  if (magic != magic_token) {
    // Arm, wait, then disarm and continue booting
    magic = magic_token;
    k_msleep(CONFIG_BOOTSEL_VIA_DOUBLE_RESET_TIMEOUT_MS);
    magic = 0;
    return 0;
  }
  sys_reboot(CONFIG_BOOTSEL_VIA_DOUBLE_RESET_TYPE);
  return 0;
}

SYS_INIT(boot_double_tap_check, POST_KERNEL, 0);
