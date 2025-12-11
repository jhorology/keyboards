
// TODO tested only on L432

#include <cmsis_core.h>
#include <zephyr/kernel.h>
#include <zephyr/linker/sections.h>

// TODO use retention API
static __boot_noinit uint32_t magic;

#if IS_ENABLED(CONFIG_ARM_MPU)
extern void arm_core_mpu_disable(void);
#endif  // CONFIG_ARM_MPU

void sys_arch_reboot(int type) {
  magic = (type & CONFIG_BOOTSEL_STM32_REBOOT_TYPE_MASK) ? CONFIG_BOOTSEL_STM32_MAGIC : 0;
  NVIC_SystemReset();
}

static int bootsel_stm32_check(void) {
  if (magic == CONFIG_BOOTSEL_STM32_MAGIC) {
    magic = 0;
#if IS_ENABLED(CONFIG_ARM_MPU)
    arm_core_mpu_disable();
#endif  // CONFIG_ARM_MPU
    __set_MSP(*(uint32_t*)CONFIG_BOOTSEL_STM32_BOOTLOADER_ADDRESS);
    ((void (*)(void))(*((uint32_t*)(CONFIG_BOOTSEL_STM32_BOOTLOADER_ADDRESS + 4))))();
    while (1);
  }
  return 0;
}

SYS_INIT(bootsel_stm32_check, PRE_KERNEL_1, CONFIG_BOOTSEL_STM32_INIT_PRIORITY);

#if IS_ENABLED(CONFIG_BOOTSEL_STM32_ON_FATAL_ERROR)
void k_sys_fatal_error_handler(unsigned int reason, const struct arch_esf* esf) {
  ARG_UNUSED(reason);
  ARG_UNUSED(esf);

  magic = CONFIG_BOOTSEL_STM32_MAGIC;
  NVIC_SystemReset();
}
#endif /* CONFIG_BOOTSEL_STM32_ON_FATAL_ERROR */
