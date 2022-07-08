# MCU name
MCU = STM32F103
MCU_LDSCRIPT = STM32F103xB_uf2
FIRMWARE_FORMAT = uf2
BOARD = STM32_F103_STM32DUINO

# Bootloader selection
BOOTLOADER = custom

# Build Options
#   comment out to disable the options.
#
RGB_MATRIX_ENABLE = yes        # Use RGB matrix
RGB_MATRIX_DRIVER = WS2812
WS2812_DRIVER = pwm
