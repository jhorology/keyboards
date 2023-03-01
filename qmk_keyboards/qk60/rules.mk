MCU = STM32F103
MCU_LDSCRIPT = STM32F103x8_0x08006000_bootloader
BOARD = STM32_F103_STM32DUINO

#dfu started address for download firmware: 0x08006000
DFU_ARGS = -d 1688:2220 -a 0 -s 0x08006000:leave
#PID
DFU_SUFFIX_ARGS = -v 1688 -p 2220
# Bootloader selection
BOOTLOADER_TYPE = stm32duino

#Enter lower-power sleep mode when on the chibios idle thread
OPT_DEFS += -DCORTEX_ENABLE_WFI_IDLE=TRUE
OPT_DEFS += -DBOOTLOADER_STM32DUINO

# os detection debug
OS_FINGERPRINT_DEBUG_ENABLE = yes
OPT_DEFS += -DOS_FINGERPRINT_DEBUG_ENABLE
SEND_STRING_ENABLE = yes
