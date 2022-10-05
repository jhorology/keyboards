# MCU name
MCU = atmega32u4

# Bootloader selection
BOOTLOADER = atmel-dfu

RGBLIGHT_ENABLE = yes       # Enable keyboard RGB underglow

# radial controller
RADIAL_CONTROLLER_ENABLE = yes
# radial controller requires DEFERRED_EXEC_ENABLE
DEFERRED_EXEC_ENABLE = yes
SRC += lib/radial_controller.c
