#include <zephyr/arch/arm/aarch32/cortex_m/cmsis.h>
#include <zephyr/kernel.h>

#define BOOTLOADER_ADDRESS 0x1FFF0000
void sys_arch_reboot(int type) {
  if (type != 0) {
    /*
     * TODO
     *  de-init + direct-jump
     *   or
     *  magic code + SYS_INIT
     */
  }
  NVIC_SystemReset();
}
