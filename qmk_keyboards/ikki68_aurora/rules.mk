# MCU name
MCU = atmega32u4

# Bootloader selection
BOOTLOADER = atmel-dfu

RGBLIGHT_ENABLE = yes       # Enable keyboard RGB underglow

# radial controller
RADIAL_CONTROLLER_ENABLE = yes
SRC += lib/radial_controller.c
