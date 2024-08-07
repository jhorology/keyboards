#include <cmsis_core.h>
#include <zephyr/kernel.h>

// uf2-stm32f103 bootloader
// See https://github.com/mmoskal/uf2-stm32f103

#define MAGIC *(volatile uint32_t *)0x20004000

#if IS_ENABLED(CONFIG_ARM_MPU)
extern void arm_core_mpu_disable(void);
#endif  // CONFIG_ARM_MPU

void sys_arch_reboot(int type) {
#if IS_ENABLED(CONFIG_ARM_MPU)
  arm_core_mpu_disable();
#endif  // CONFIG_ARM_MPU
  MAGIC = (type & CONFIG_BOOTSEL_UF2_STM32F103_MAGIC_UF2_REBOOT_TYPE_MASK)
            ? CONFIG_BOOTSEL_UF2_STM32F103_MAGIC_UF2
            : CONFIG_BOOTSEL_UF2_STM32F103_MAGIC_APP;
  NVIC_SystemReset();
}

#if IS_ENABLED(CONFIG_BOOTSEL_UF2_STM32F103_ON_FATAL_ERROR)
void k_sys_fatal_error_handler(unsigned int reason, const z_arch_esf_t *esf) {
  ARG_UNUSED(reason);
  ARG_UNUSED(esf);

  MAGIC = CONFIG_BOOTSEL_UF2_STM32F103_MAGIC_UF2;
  NVIC_SystemReset();
}
#endif  // CONFIG_BOOTSEL_UF2_STM32F103_ON_FATAL_ERROR
