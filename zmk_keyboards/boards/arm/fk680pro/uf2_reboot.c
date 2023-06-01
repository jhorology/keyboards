#include <zephyr/arch/arm/aarch32/cortex_m/cmsis.h>
#include <zephyr/kernel.h>

// uf2-stm32f103 bootloader
// See
// https://github.com/mmoskal/uf2-stm32f103
// https://github.com/mmoskal/uf2-stm32f103/blob/cfd41057e267f826a8277c390a21dd5d4bd8a271/src/stm32f103/target_stm32f103.c#LL53C1-L55C1

#define CMD_BOOT 0x544F4F42UL;
#define CMD_APP 0x3f82722aUL;
#define MAGIC_REG *(volatile uint32_t *)0x20004000

void sys_arch_reboot(int type) {
    if (type != 0) {
        MAGIC_REG = CMD_BOOT;
    } else {
        MAGIC_REG = CMD_APP;
    }
    NVIC_SystemReset();
}

void k_sys_fatal_error_handler(unsigned int reason, const z_arch_esf_t *esf) {
    MAGIC_REG = CMD_BOOT;
    NVIC_SystemReset();
}
