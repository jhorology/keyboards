
// TODO tested only on L432

#include <zephyr/arch/arm/aarch32/cortex_m/cmsis.h>
#include <zephyr/fatal.h>
#include <zephyr/init.h>

// static const uint32_t magic_token = 0xDEADBEAFUL;
static __noinit uint32_t magic;
extern void arm_core_mpu_disable(void);

void sys_arch_reboot(int type) {
  magic = type != 0 ? CONFIG_BOOTSEL_STM32_MAGIC : 0;
  NVIC_SystemReset();
}

int bootsel_stm32_check(const struct device *port) {
  ARG_UNUSED(port);
  if (magic == CONFIG_BOOTSEL_STM32_MAGIC) {
    magic = 0;
    arm_core_mpu_disable();
    __set_MSP(*(uint32_t *)CONFIG_BOOTSEL_STM32_BOOTLOADER_ADDRESS);
    ((void (*)(void))(*((uint32_t *)(CONFIG_BOOTSEL_STM32_BOOTLOADER_ADDRESS + 4))))();
    while (1)
      ;
  }
  return 0;
}
SYS_INIT(bootsel_stm32_check, PRE_KERNEL_1, 0);

#if IS_ENABLED(CONFIG_BOOTSEL_STM32_ON_FATAL_ERROR)
void k_sys_fatal_error_handler(unsigned int reason, const z_arch_esf_t *esf) {
  magic = CONFIG_BOOTSEL_STM32_MAGIC;
  NVIC_SystemReset();
}
#endif /* CONFIG_BOOTSEL_STM32_ON_FATAL_ERROR */
